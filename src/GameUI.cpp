#include <GameUI.hpp>
#include <Board.hpp>
#include <Helper.hpp>
#include <iostream>
#include <vector>

GameConfig::GameConfig(int sizeState) {
	boardTopLeft = (convertToFloat(virtualWindowSize) - sf::Vector2f(borderLimit, borderLimit)) * 0.5f;
	
	boardTopLeft.x += 20;
	boardTopLeft.y = 50;

	switch (sizeState) {
		case 0:
			gridSize = 9;
			break;
		case 1:
			gridSize = 13;
			break;
		case 2:
			gridSize = 19;
			break;
		default:
			gridSize = -1;
			break;
	}
}
int GameConfig::piece_offset() {
	return boardEdge / (gridSize - 1);
}
float GameConfig::board_offset() {
	return (borderLimit - boardEdge) * 0.5f;
}
float GameConfig::stoneRadius() {
	return std::min(20.0f, piece_offset() * 0.5f - 5);
}

void GameUI::init() {
	const std::filesystem::path CHINESE_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	chinese_font.openFromFile(CHINESE_FONT_PATH.c_str());
	const std::filesystem::path ENGLISH_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/english.otf");
	english_font.openFromFile(ENGLISH_FONT_PATH.c_str());

	ui_color = sf::Color::White;

	const std::filesystem::path PIECE_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Board/go_pieces.png");

	for (int it = 0; it < 2; ++it) {
		go_piece[it].loadFromFile(PIECE_PATH.c_str(), 0,
			sf::IntRect(sf::Vector2i(100 * it, 0), sf::Vector2i(100, 100)));
	}

	PIECE_SCALE = sf::Vector2f(0.4, 0.4);
}

//Gameplay initialization
void GameUI::initGame(int autoSaveFlag) {
	board = Board();
	gameConfig = GameConfig(0);


	autoSaveToggle = autoSaveFlag;
	board.setSize(gameConfig.gridSize, gameConfig.gridSize);
	
	PIECE_SCALE = sf::Vector2f(gameConfig.stoneRadius() / (go_piece[0].getSize().x * 0.5f), gameConfig.stoneRadius() / (go_piece[0].getSize().y * 0.5f));

	savedEndGame = false;
	endPopup.clearCache();

	fileNotification = "";
	system("cls");
}

void GameUI::resetGame() {
	initGame();
	
	saveGame();
}

void GameUI::setCenter(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.size * 0.5f);
}


void GameUI::draw_back_button(sf::RenderWindow& appWindow) {
	// draw the text
	delete back_button;

	back_button = new sf::Text(chinese_font);
	back_button -> setString("GO BACK");
	back_button -> setCharacterSize(25);
	back_button -> setFillColor(ui_color);
	back_button -> setPosition(sf::Vector2f(20, 20));

	appWindow.draw(*back_button);
}

void GameUI::draw_option_button(sf::RenderWindow& appWindow) {
	// draw the text
	delete option_button;

	option_button = new sf::Text(chinese_font);
	option_button->setString("OPTIONS");
	option_button->setCharacterSize(25);
	option_button->setFillColor(ui_color);
	option_button->setPosition(sf::Vector2f(869, 20));

	appWindow.draw(*option_button);
}

void GameUI::draw_UI(sf::RenderWindow& appWindow) {
	/*
		Setting the board to be centered
	*/
	
	/*
		Drawing board lines, horizontally and vertically
	*/
	for (int r = 0; r < board.getSize().first; ++r) {
		for (int c = 0; c + 1 < board.getSize().second; ++c) {
			sf::RectangleShape thickLine(sf::Vector2f(gameConfig.piece_offset(), 2));
			thickLine.setOrigin(sf::Vector2f(0, thickLine.getSize().y * 0.5f));
			thickLine.setFillColor(sf::Color::Black);

			thickLine.setPosition(gameConfig.boardTopLeft + sf::Vector2f(gameConfig.board_offset() + c * gameConfig.piece_offset(), gameConfig.board_offset() + r * gameConfig.piece_offset()));
			appWindow.draw(thickLine);
		}
	}
	for (int r = 0; r + 1 < board.getSize().first; ++r) {
		for (int c = 0; c < board.getSize().second; ++c) {
			sf::RectangleShape thickLine(sf::Vector2f(2, gameConfig.piece_offset()));
			thickLine.setOrigin(sf::Vector2f(thickLine.getSize().x * 0.5f, 0));
			thickLine.setFillColor(sf::Color::Black);

			thickLine.setPosition(gameConfig.boardTopLeft + sf::Vector2f(gameConfig.board_offset() + c * gameConfig.piece_offset(), gameConfig.board_offset() + r * gameConfig.piece_offset()));
			appWindow.draw(thickLine);
		}
	}

	//Draw the go board
	for(int r = 0; r < board.getSize().first; ++r)
	for (int c = 0; c < board.getSize().second; ++c) {
		int cur = board.getState(r, c);
		if (cur != -1) {
			sf::Sprite current_piece(go_piece[cur]);
			sf::Vector2f current_pos = gameConfig.boardTopLeft + sf::Vector2f(gameConfig.board_offset() + c * gameConfig.piece_offset(), gameConfig.board_offset() + r * gameConfig.piece_offset());
			
			current_piece.setPosition(current_pos);
			current_piece.setScale(PIECE_SCALE);

			sf::Vector2u sz = go_piece[cur].getSize();
			current_piece.setOrigin(convertToFloat(sz) * 0.5f);

			appWindow.draw(current_piece);
		}
	}
}

void GameUI::draw_game_buttons(sf::RenderWindow& appWindow) {
	//Pass, resign, save, load button

	sf::Vector2f buttonSize(155, 75);
	auto Specific_Draw = [&](const std::string label, sf::Vector2f topLeft) -> void {
		sf::RectangleShape passButton(buttonSize);
		passButton.setFillColor(sf::Color(67, 70, 75));
		passButton.setPosition(topLeft);

		sf::Text passText(chinese_font);
		passText.setString(label);
		passText.setFillColor(sf::Color::White);

		sf::Vector2f textSize = passText.getLocalBounds().size;

		passText.setOrigin(textSize * 0.5f);
		passText.setPosition(topLeft + buttonSize * 0.5f);

		appWindow.draw(passButton);
		appWindow.draw(passText);
	};
	
	Specific_Draw("SAVE", sf::Vector2f(20, 369));
	Specific_Draw("LOAD", sf::Vector2f(20, 369 + buttonSize.y + 20));
	Specific_Draw("PASS", sf::Vector2f(20, 369 + 2 * (buttonSize.y + 20)));
	Specific_Draw("RESIGN", sf::Vector2f(20, 369 + 3 * (buttonSize.y + 20)));

	Specific_Draw("UNDO", sf::Vector2f(gameConfig.boardTopLeft.x + gameConfig.borderLimit + 20, 369));
	Specific_Draw("REDO", sf::Vector2f(gameConfig.boardTopLeft.x + gameConfig.borderLimit + 20, 369 + buttonSize.y + 20));
	Specific_Draw("UNDO ALL", sf::Vector2f(gameConfig.boardTopLeft.x + gameConfig.borderLimit + 20, 369 + 2 * (buttonSize.y + 20)));
	Specific_Draw("REDO ALL", sf::Vector2f(gameConfig.boardTopLeft.x + gameConfig.borderLimit + 20, 369 + 3 * (buttonSize.y + 20)));
}

int GameUI::tryClickingAt(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos) {
	//Go back
	if (back_button->getGlobalBounds().contains(mouse_pos)) {
		return 10;
	}

	if (option_button->getGlobalBounds().contains(mouse_pos)) {
		return 20;
	}

	//End game -> Disable functions and only check New Game ?
	if (!board.isInGame()) {
		if (endPopup.clickedOn( "New Game?", mouse_pos)) {
			resetGame();
		}

		return -1;
	}

	const float RADIUS = gameConfig.stoneRadius();

	for (int r = 0; r < board.getSize().first; ++r) {
		for (int c = 0; c < board.getSize().second; ++c) {
			sf::Vector2f cur_pos = gameConfig.boardTopLeft + sf::Vector2f(gameConfig.board_offset() + c * gameConfig.piece_offset(), gameConfig.board_offset() + r * gameConfig.piece_offset());
			if (dist(cur_pos, mouse_pos) > RADIUS) continue;

			if (!board.possibleToPlace(r, c)) {
				return 2;
			}

			// check if there were capture, really inefficient.
			
			std::string tmp = board.getState();
			board.placePieceAt(r, c);


			//AutoSaving
			if (autoSaveToggle) saveGame();

			std::string cur = board.getState();
			int cnt = 0;

			//The last chacracter is the pass toggle
			for (int i = 0; i < (int) tmp.size() - 1; ++i)
				cnt += (cur[i] != tmp[i]);

			if (cnt > 1) return 1;

			return 0;
		}
	}
	
	sf::Vector2f buttonSize(155, 75);
	sf::Vector2f funcButton(20, 369);

	//Save region
	if ((mouse_pos.x - funcButton.x) * (mouse_pos.x - (funcButton.x + buttonSize.x)) <= 0 && (mouse_pos.y - funcButton.y) * (mouse_pos.y - (funcButton.y + buttonSize.y)) <= 0) {
		saveGame();
		return -1;
	}

	funcButton += sf::Vector2f(0, buttonSize.y + 20);

	//Load region
	if ((mouse_pos.x - funcButton.x) * (mouse_pos.x - (funcButton.x + buttonSize.x)) <= 0 && (mouse_pos.y - funcButton.y) * (mouse_pos.y - (funcButton.y + buttonSize.y)) <= 0) {
		loadGame();
		return -1;
	}

	funcButton += sf::Vector2f(0, buttonSize.y + 20);

	//Pass region
	if ((mouse_pos.x - funcButton.x) * (mouse_pos.x - (funcButton.x + buttonSize.x)) <= 0 && (mouse_pos.y - funcButton.y) * (mouse_pos.y - (funcButton.y + buttonSize.y)) <= 0) {
		board.pass();
		return -1;
	}

	funcButton += sf::Vector2f(0, buttonSize.y + 20);

	//Resign region
	if ((mouse_pos.x - funcButton.x) * (mouse_pos.x - (funcButton.x + buttonSize.x)) <= 0 && (mouse_pos.y - funcButton.y) * (mouse_pos.y - (funcButton.y + buttonSize.y)) <= 0) {
		board.resign();
		return -1;
	}

	funcButton = sf::Vector2f(gameConfig.boardTopLeft.x + gameConfig.borderLimit + 20, 369);

	//Undo region
	if ((mouse_pos.x - funcButton.x) * (mouse_pos.x - (funcButton.x + buttonSize.x)) <= 0 && (mouse_pos.y - funcButton.y) * (mouse_pos.y - (funcButton.y + buttonSize.y)) <= 0) {
		board.undo();
		return -1;
	}

	funcButton += sf::Vector2f(0, buttonSize.y + 20);

	//Redo region
	if ((mouse_pos.x - funcButton.x) * (mouse_pos.x - (funcButton.x + buttonSize.x)) <= 0 && (mouse_pos.y - funcButton.y) * (mouse_pos.y - (funcButton.y + buttonSize.y)) <= 0) {
		board.redo();
		return -1;
	}

	funcButton += sf::Vector2f(0, buttonSize.y + 20);

	//Undo All region
	if ((mouse_pos.x - funcButton.x) * (mouse_pos.x - (funcButton.x + buttonSize.x)) <= 0 && (mouse_pos.y - funcButton.y) * (mouse_pos.y - (funcButton.y + buttonSize.y)) <= 0) {
		board.undoAll();
		return -1;
	}

	funcButton += sf::Vector2f(0, buttonSize.y + 20);

	//Redo All region
	if ((mouse_pos.x - funcButton.x) * (mouse_pos.x - (funcButton.x + buttonSize.x)) <= 0 && (mouse_pos.y - funcButton.y) * (mouse_pos.y - (funcButton.y + buttonSize.y)) <= 0) {
		board.redoAll();
		return -1;
	}

	return -1;
}

void GameUI::drawShadow(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos) {
	if (!board.isInGame()) return;

	const float RADIUS = gameConfig.stoneRadius();

	//Draw shadow if possible to place the piece in that cell	
	for (int r = 0; r < board.getSize().first; ++r) {
		for (int c = 0; c < board.getSize().second; ++c) {
			sf::Vector2f current_pos = gameConfig.boardTopLeft + sf::Vector2f(gameConfig.board_offset() + c * gameConfig.piece_offset(), gameConfig.board_offset() + r * gameConfig.piece_offset());
			if (dist(current_pos, mouse_pos) > RADIUS) continue;

			if (!board.possibleToPlace(r, c)) continue;

			int cur_turn = board.getTurn();
			sf::Sprite current_piece(go_piece[cur_turn]);
	
			current_piece.setPosition(current_pos);
			current_piece.setScale(PIECE_SCALE);
			
			sf::Color color = current_piece.getColor();
			color.a = round(color.a * 0.6f);
			current_piece.setColor(color);

			sf::Vector2u sz = go_piece[cur_turn].getSize();
			current_piece.setOrigin(convertToFloat(sz) * 0.5f);

			appWindow.draw(current_piece);
			return;
		}
	}
}

void GameUI::saveGame() {
	switch (board.saveGame()) {
		case FileStatus::Success:
			fileNotification = "Saved\nsuccessfully";
			break;
		case FileStatus::FileNotFound:
			fileNotification = "File not found";
			break;
		case FileStatus::CorruptedFile:
			fileNotification = "Corrupted file";
			break;
		case FileStatus::WrongFormat:
			fileNotification = "Wrong game's\nformat";
			break;
	}

	notificationTimer.restart();
}
void GameUI::loadGame() {
	switch (board.loadGame()) {
		case FileStatus::Success:
			fileNotification = "Loaded\nsuccessfully";
			break;
		case FileStatus::FileNotFound:
			fileNotification = "File not found";
			break;
		case FileStatus::CorruptedFile:
			fileNotification = "Corrupted file";
			break;
		case FileStatus::WrongFormat:
			fileNotification = "Wrong game's\nformat";
			break;
	}

	savedEndGame = !board.isInGame();

	notificationTimer.restart();
}

//Showing whose turn is it and game saves/loads
void GameUI::loadTurnIndicator() {
	messageBox.setPosition(sf::Vector2f(10, 100));

	messageBox.setSize(sf::Vector2f(200, 200));

	messageBox.addObject("Player " + std::to_string(board.getTurn() + 1) + "'s turn", { 10, 20 });
	messageBox.addObject(board.getPassState() ? "Pass clicked" : "Pass unclicked", { 10, 60 });

	if (notificationTimer.getElapsedTime() <= notificationDuration) {
		messageBox.addObject(fileNotification, { 10, 100 });
	}
}

//In-game Annoucement
void GameUI::annouceInGame(sf::RenderWindow& appWindow) {
	if (!board.isInGame()) return;

	loadTurnIndicator();
	messageBox.drawOn(appWindow);

	messageBox.clearCache();
}

//End Popup
void GameUI::loadEndPopup() {
	endPopup = Popup();
	endPopup.setSize(sf::Vector2f(300, 200));

	endPopup.setPosition((convertToFloat(virtualWindowSize) - endPopup.getSize()) * 0.5f);

	std::array <int, 2> score = board.getScore();

	if (score[0] == 0x3f3f3f3f) {
		endPopup.addObject("Player 1 won\nby resignation.", { 20, 20 });
	}
	else if (score[1] == 0x3f3f3f3f) {
		endPopup.addObject("Player 2 won\nby resignation.", { 20, 20 });
	}
	else {
		endPopup.addObject("Player 1: " + std::to_string(score[0]), { 20, 20 });
		endPopup.addObject("Player 2: " + std::to_string(score[1]), { 20, 60 });

		if (score[0] == score[1]) {
			endPopup.addObject("Draw", { 20, 100 });
		}
		else {
			endPopup.addObject("Player " + std::to_string((score[0] < score[1]) + 1) + " wins", { 20, 100 });
		}
	}

	endPopup.addObject("New Game?", { 75, 150 });
}

//End-game annoucement
void GameUI::annouceEndGame(sf::RenderWindow& appWindow) {
	if (board.isInGame()) return;

	//GGWP, end game!

	loadEndPopup();

	if (!savedEndGame) {
		board.saveGame();
		savedEndGame = true;
	}

	endPopup.drawOn(appWindow);
}