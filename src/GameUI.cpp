#include <GameUI.hpp>
#include <Board.hpp>
#include <Helper.hpp>
#include <iostream>
#include <vector>

void GameUI::init() {
	const std::filesystem::path CHINESE_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/chinese.otf");
	chinese_font.openFromFile(CHINESE_FONT_PATH.c_str());
	const std::filesystem::path ENGLISH_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/english.otf");
	english_font.openFromFile(ENGLISH_FONT_PATH.c_str());

	ui_color = sf::Color::White;

	const std::filesystem::path BOARD_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/board.png");
	const std::filesystem::path PIECE_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/go_pieces.png");


	chessboard.loadFromFile(BOARD_PATH.c_str());
	for (int it = 0; it < 2; ++it) {
		sf::Texture cur; cur.loadFromFile(PIECE_PATH.c_str(), 0,
			sf::IntRect(sf::Vector2i(100 * it, 0), sf::Vector2i(100, 100)));
		chess_piece.emplace_back(cur);
	}


	PIECE_SCALE = sf::Vector2f(0.4, 0.4);
	offset = sf::Vector2f(40, 40);
	board_offset = sf::Vector2f(200, 0);
	gapX = sf::Vector2f(65, 0);
	gapY = sf::Vector2f(0, 65);

	board.setSize(9, 9);
	
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
	sf::Sprite board_sprite(chessboard);
	board_sprite.setPosition(board_offset);
	appwindow.draw(board_sprite);

	for(int r = 0; r < board.getSize().first; ++r)
	for (int c = 0; c < board.getSize().second; ++c) {
		int cur = board.getState(r, c);
		if (cur != -1) {
			sf::Sprite current_piece(chess_piece[cur]);
			sf::Vector2f current_pos = board_offset + offset + gapX * float(c) + gapY * float(r);
			current_piece.setPosition(current_pos);
			current_piece.setScale(PIECE_SCALE);

			sf::Vector2u sz = chess_piece[cur].getSize();
			current_piece.setOrigin(sf::Vector2f(0.5f * sz.x, 0.5f * sz.y));
			appwindow.draw(current_piece);
		}
	}
}


int GameUI::tryClickingAt(sf::Vector2f mouse_pos) {
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
	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 40)
		return 10;
	return -1;
}

void GameUI::drawShadow(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos) {
	const float RADIUS = 25;

	for (int r = 0; r < board.getSize().first; ++r) {
		for (int c = 0; c < board.getSize().second; ++c) {
			sf::Vector2f cur_pos = board_offset + offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) > RADIUS) continue;

			if (!board.possibleToPlace(r, c)) continue;

			int cur_turn = board.getTurn();
			sf::Sprite current_piece(chess_piece[cur_turn]);
			sf::Vector2f current_pos = board_offset + offset + gapX * float(c) + gapY * float(r);
			current_piece.setPosition(current_pos);
			current_piece.setScale(PIECE_SCALE);
			
			sf::Color color = current_piece.getColor();
			color.a = round(color.a * 0.6f);
			current_piece.setColor(color);

			sf::Vector2u sz = chess_piece[cur_turn].getSize();
			current_piece.setOrigin(sf::Vector2f(0.5f * sz.x, 0.5f * sz.y));
			appwindow.draw(current_piece);
			return;
		}
	}
}