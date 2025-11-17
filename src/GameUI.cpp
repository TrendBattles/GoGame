#include <GameUI.hpp>
#include <Board.hpp>
#include <Helper.hpp>
#include <iostream>
#include <vector>

void GameUI::init() {
	const std::filesystem::path CHINESE_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	chinese_font.openFromFile(CHINESE_FONT_PATH.c_str());
	const std::filesystem::path ENGLISH_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/english.otf");
	english_font.openFromFile(ENGLISH_FONT_PATH.c_str());

	ui_color = sf::Color::White;

	const std::filesystem::path BOARD_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Board/board.png");
	const std::filesystem::path PIECE_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Board/go_pieces.png");


	goboard.loadFromFile(BOARD_PATH.c_str());
	for (int it = 0; it < 2; ++it) {
		go_piece[it].loadFromFile(PIECE_PATH.c_str(), 0,
			sf::IntRect(sf::Vector2i(100 * it, 0), sf::Vector2i(100, 100)));
	}

	//Undo-Redo
	const std::filesystem::path MOVE_BUTTON =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Board/move_button.png");
	
	for (int it = 0; it < 2; ++it) {
		sf::Texture moveTexture;
		moveTexture.loadFromFile(MOVE_BUTTON.c_str(), 0,
			sf::IntRect(sf::Vector2i(150 * it, 0), sf::Vector2i(150, 150)));

		functional_buttons.emplace_back(moveTexture);
	}

	swap(functional_buttons[0], functional_buttons[1]);

	//Pass
	PIECE_SCALE = sf::Vector2f(0.4, 0.4);
	offset = sf::Vector2f(40, 40);
	board_offset = sf::Vector2f(200, 0);
	gapX = sf::Vector2f(65, 0);
	gapY = sf::Vector2f(0, 65);
}

void GameUI::resetGame() {
	system("cls");

	board = Board();
	board.setSize(9, 9);

	std::cerr << "Player 1's turn\n";
}

void GameUI::setCenter(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.size * 0.5f);;
}


void GameUI::draw_back_button(sf::RenderWindow& appwindow) {
	// draw the text
	sf::Text back_button(chinese_font);
	back_button.setString("GO BACK");
	back_button.setCharacterSize(25);
	back_button.setFillColor(ui_color);
	back_button.setPosition(sf::Vector2f(20, 20));
	appwindow.draw(back_button);
}

void GameUI::draw_UI(sf::RenderWindow& appwindow) {
	sf::Sprite board_sprite(goboard);
	board_sprite.setPosition(board_offset);
	appwindow.draw(board_sprite);

	//Draw the go board
	for(int r = 0; r < board.getSize().first; ++r)
	for (int c = 0; c < board.getSize().second; ++c) {
		int cur = board.getState(r, c);
		if (cur != -1) {
			sf::Sprite current_piece(go_piece[cur]);
			sf::Vector2f current_pos = board_offset + offset + gapX * float(c) + gapY * float(r);
			current_piece.setPosition(current_pos);
			current_piece.setScale(PIECE_SCALE);

			sf::Vector2u sz = go_piece[cur].getSize();
			current_piece.setOrigin(sf::Vector2f(0.5f * sz.x, 0.5f * sz.y));
			appwindow.draw(current_piece);
		}
	}
}

void GameUI::draw_game_buttons(sf::RenderWindow& appWindow) {
	//Undo-Redo button
	sf::Vector2f origin = board_offset + sf::Vector2f(0, static_cast <float> (goboard.getSize().y) + 20);

	for (int it = 0; it < 2; ++it) {
		sf::Sprite button(functional_buttons[it]);
		button.setScale({ 0.75f, 0.75f });


		sf::Vector2f size = convertToFloat(functional_buttons[it].getSize()) * 0.75f;
		button.setPosition(origin + sf::Vector2f{ size.x + 20, 0 } *float(it));

		appWindow.draw(button);
	}

	origin.x += functional_buttons[0].getSize().x * 0.75f * 2 + 20 * 2;

	//Pass + Resign button
	auto Specific_Draw = [&](const std::string label) -> void {
		sf::Vector2f buttonSize(155, 75);

		sf::RectangleShape passButton(buttonSize);
		passButton.setFillColor(sf::Color(67, 70, 75));
		passButton.setPosition(origin);

		sf::Text passText(chinese_font);
		passText.setString(label);
		passText.setCharacterSize(30);
		passText.setFillColor(sf::Color::White);

		sf::Vector2f textSize = passText.getLocalBounds().size;
		

		passText.setOrigin(textSize * 0.5f);
		passText.setPosition(origin + buttonSize * 0.5f);
		passText.move({ 0, -10.f });

		appWindow.draw(passButton);
		appWindow.draw(passText);

		origin += sf::Vector2f(buttonSize.x + 20, 0);
	};

	Specific_Draw("PASS");
	Specific_Draw("RESIGN");
}

int GameUI::tryClickingAt(sf::Vector2f mouse_pos) {
	//Go back
	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 40)
		return 10;

	if (!board.isInGame()) return -1;

	const float RADIUS = 25;

	for (int r = 0; r < board.getSize().first; ++r) {
		for (int c = 0; c < board.getSize().second; ++c) {
			sf::Vector2f cur_pos = board_offset + offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) > RADIUS) continue;

			if (!board.possibleToPlace(r, c)) {
				return 2;
			}

			// check if there were capture, really inefficient.
			
			std::string tmp = board.getState();
			board.placePieceAt(r, c);

			std::string cur = board.getState();
			int cnt = 0;
			for (int i = 0; i < (int)tmp.size(); ++i)
				cnt += (cur[i] != tmp[i]);

			if (cnt > 1) return 1;
			return 0;
		}
	}

	//Undo-Redo
	sf::Vector2f origin = board_offset + sf::Vector2f(0, static_cast <float> (goboard.getSize().y) + 20);

	for (int it = 0; it < 2; ++it) {
		sf::Vector2f size = convertToFloat(functional_buttons[it].getSize()) * 0.75f;
		
		sf::Vector2f target_center = origin + sf::Vector2f{ size.x + 20, 0 } * float(it) + size * 0.5f;
		if (dist(target_center, mouse_pos) > size.x / 2) continue;

		if (it == 0) {
			if (board.undo()) return 1;
			return 2;
		}

		if (board.redo()) return 1;
		return 2;
	}

	//Pass
	origin.x += functional_buttons[0].getSize().x * 0.75f * 2 + 20 * 2;
	if (origin.x <= mouse_pos.x && mouse_pos.x <= origin.x + 175 && origin.y <= mouse_pos.y && origin.y <= mouse_pos.y + 75) {
		board.pass();

		return -1;
	}

	origin += sf::Vector2f(175 + 20, 0);
	if (origin.x <= mouse_pos.x && mouse_pos.x <= origin.x + 175 && origin.y <= mouse_pos.y && origin.y <= mouse_pos.y + 75) {
		board.resign();

		return -1;
	}
	return -1;
}

void GameUI::drawShadow(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos) {
	if (!board.isInGame()) return;

	const float RADIUS = 25;

	for (int r = 0; r < board.getSize().first; ++r) {
		for (int c = 0; c < board.getSize().second; ++c) {
			sf::Vector2f cur_pos = board_offset + offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) > RADIUS) continue;

			if (!board.possibleToPlace(r, c)) continue;

			int cur_turn = board.getTurn();
			sf::Sprite current_piece(go_piece[cur_turn]);
			sf::Vector2f current_pos = board_offset + offset + gapX * float(c) + gapY * float(r);
			current_piece.setPosition(current_pos);
			current_piece.setScale(PIECE_SCALE);
			
			sf::Color color = current_piece.getColor();
			color.a = round(color.a * 0.6f);
			current_piece.setColor(color);

			sf::Vector2u sz = go_piece[cur_turn].getSize();
			current_piece.setOrigin(sf::Vector2f(0.5f * sz.x, 0.5f * sz.y));
			appwindow.draw(current_piece);
			return;
		}
	}
}

bool GameUI::annouceEndGame() {
	if (board.isInGame()) return false;

	std::array <int, 2> score = board.getScore();
	if (score[0] == 0x3f3f3f3f) {
		std::cout << "Player 1 won by resignation\n";
	}
	else if (score[1] == 0x3f3f3f3f) {
		std::cout << "Player 2 won by resignation\n";
	}
	else {
		std::cout << "Player 1: " << score[0] << '\n';
		std::cout << "Player 2: " << score[1] << '\n';

		if (score[0] == score[1]) {
			std::cout << "Draw\n";
		}
		else {
			std::cout << "Player " << (score[0] < score[1]) + 1 << " wins\n";
		}
	}

	return true;
}