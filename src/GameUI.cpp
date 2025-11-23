#include <GameUI.hpp>
#include <Board.hpp>
#include <Helper.hpp>
#include <iostream>
#include <vector>

GameConfig::GameConfig(int sizeState) {
	vertical_offset = sf::Vector2f(0, 100);
	horizontal_offset = sf::Vector2f(virtualWindowSize.x * 0.5f, 0);
	boardTopLeft = (convertToFloat(virtualWindowSize) - sf::Vector2f(boardEdge, boardEdge)) * 0.5f;
	
	boardTopLeft.y = vertical_offset.y;

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

void GameConfig::setBorderLimit(int x) {
	borderLimit = x;
	boardTopLeft = (convertToFloat(virtualWindowSize) - sf::Vector2f(borderLimit, borderLimit)) * 0.5f;
	boardTopLeft.y = vertical_offset.y;
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

	autoSaveToggle = 1;
}

//Gameplay initialization
void GameUI::initGame() {
	board.clearGame();

	gameConfig = GameConfig(boardOption);
	board.setSize(gameConfig.gridSize, gameConfig.gridSize);
	
	//When initialized, force to remove the message
	fileNotification = "";

	//Loading the game
	autoLoad();

	//Initialize the clock
	if (timeLimitSet) {
		board.resetClock();
		deltaClock.restart();
	}

	PIECE_SCALE = sf::Vector2f(gameConfig.stoneRadius() / (go_piece[0].getSize().x * 0.5f), gameConfig.stoneRadius() / (go_piece[0].getSize().y * 0.5f));

	savedEndGame = playedEndGameSound = false;
	endPopup.clearCache();
}

void GameUI::resetGame() {
	//Remove traces of the last game in the file
	board.clearGame();
	saveGame();

	//Load the game
	initGame();
}

void GameUI::setCenter(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.size * 0.5f);
}


void GameUI::draw_back_button(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos) {
	//End game -> Only focus on end popup
	if (!board.isInGame()) return;

	sf::Text back_button(chinese_font);
	back_button.setString("Menu");

	back_button.setCharacterSize(25);
	back_button.setFillColor(ui_color);
	back_button.setPosition(sf::Vector2f(20, 20));


	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 80) {
		back_button.setFillColor(accent_color);
	}

	appWindow.draw(back_button);


	sf::Text game_button(chinese_font);
	game_button.setString("Options");

	game_button.setCharacterSize(25);
	game_button.setFillColor(ui_color);
	game_button.setPosition(sf::Vector2f(1070, 20));

	mouse_pos.x = virtualWindowSize.x - mouse_pos.x;
	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 80) {
		game_button.setFillColor(accent_color);
	}

	appWindow.draw(game_button);
}

void GameUI::draw_UI(sf::RenderWindow& appWindow) {
	switch (gameConfig.gridSize) {
	case 9:
		gameConfig.setBorderLimit(640);
		break;
	case 13:
		gameConfig.setBorderLimit(630);
		break;
	case 19:
		gameConfig.setBorderLimit(620);
		break;
	}


	sf::Text menu_title(chinese_font);
	menu_title.setString("GO GAME");
	menu_title.setCharacterSize(40);
	menu_title.setFillColor(ui_color);
	menu_title.setPosition(gameConfig.horizontal_offset + gameConfig.vertical_offset * 0.5f);
	setCenter(menu_title);

	appWindow.draw(menu_title);

	sf::RectangleShape rect(sf::Vector2f(gameConfig.borderLimit, gameConfig.borderLimit));
	rect.setPosition(gameConfig.boardTopLeft);
	rect.setFillColor(sf::Color(246, 178, 107));

	appWindow.draw(rect);
	
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


sf::FloatRect GameUI::expandHitbox(sf::FloatRect bounds, float margin)
{
	sf::Vector2f sz = bounds.size;
	bounds.size *= margin;
	bounds.position += sz * (1.0f - margin) * 0.5f;
	return bounds;
}

void GameUI::draw_game_buttons(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos) {
	//Pass, resign, save, load button

	auto Specific_Draw = [&](const std::string label, sf::Vector2f topLeft, int default_size) -> void {
		sf::Text passText(chinese_font);
		passText.setString(label);
		passText.setFillColor(ui_color);
		passText.setCharacterSize(default_size);

		sf::Vector2f textSize = passText.getLocalBounds().size;

		passText.setOrigin(textSize * 0.5f);
		passText.setPosition(topLeft);

		if (expandHitbox(passText.getGlobalBounds(), 3).contains(mouse_pos))
			passText.setFillColor(accent_color);

		appWindow.draw(passText);
	};
	
	sf::Vector2f horizontal_offset(virtualWindowSize.x * 0.5f, 0), content_offset(horizontal_offset.x - gameConfig.boardTopLeft.x * 0.5f, 0);
	sf::Vector2f vertical_offset(0, 400), vertical_gap(0, 75);
	
	if (!timeLimitSet) {
		//Case for no time limit
		Specific_Draw("SAVE", 
			horizontal_offset - content_offset + vertical_offset + vertical_gap * 0.0f, 25);
		Specific_Draw("LOAD",
			horizontal_offset - content_offset + vertical_offset + vertical_gap * 1.0f, 25);
		Specific_Draw("PASS",
			horizontal_offset - content_offset + vertical_offset + vertical_gap * 2.0f, 25);
		Specific_Draw("RESIGN",
			horizontal_offset - content_offset + vertical_offset + vertical_gap * 3.0f, 25);

		Specific_Draw("UNDO",
			horizontal_offset + content_offset + vertical_offset + vertical_gap * 0.0f, 25);
		Specific_Draw("REDO",
			horizontal_offset + content_offset + vertical_offset + vertical_gap * 1.0f, 25);
		Specific_Draw("UNDO ALL",
			horizontal_offset + content_offset + vertical_offset + vertical_gap * 2.0f, 25);
		Specific_Draw("REDO ALL",
			horizontal_offset + content_offset + vertical_offset + vertical_gap * 3.0f, 25);
	}
	else {
		Specific_Draw("PASS",
			horizontal_offset + content_offset + vertical_offset + vertical_gap * 0.0f, 25);
		Specific_Draw("RESIGN",
			horizontal_offset + content_offset + vertical_offset + vertical_gap * 1.0f, 25);
	}
}

int GameUI::tryClickingAt(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos) {
	//End game -> Disable functions and only check New Game ?
	if (!board.isInGame()) {
		if (newGamePopup.clickedOn(mouse_pos)) {
			resetGame();
			return 4;
		}

		if (menuPopup.clickedOn(mouse_pos)) {
			return 10;
		}

		return -1;
	}

	//Go back
	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 80) {
		return 10;
	}

	//Transit to options
	sf::Vector2f tmp = mouse_pos;
	tmp.x = virtualWindowSize.x - tmp.x;
	if (tmp.x >= 0 && tmp.x <= 150 && tmp.y >= 0 && tmp.y <= 80) {
		return 20;
	}

	const float RADIUS = gameConfig.stoneRadius();

	for (int r = 0; r < board.getSize().first; ++r) {
		for (int c = 0; c < board.getSize().second; ++c) {
			sf::Vector2f cur_pos = gameConfig.boardTopLeft + sf::Vector2f(gameConfig.board_offset() + c * gameConfig.piece_offset(), gameConfig.board_offset() + r * gameConfig.piece_offset());
			if (dist(cur_pos, mouse_pos) > RADIUS) continue;

			if (!board.possibleToPlace(r, c)) {
				return 2;
			}

			//Bonus time
			if (timeLimitSet) board.addTime(board.getTurn());

			std::string tmp = board.getState();
			board.placePieceAt(r, c);


			//AutoSaving
			if (autoSaveToggle) saveGame();

			std::string cur = board.getState();
			int cnt = 0;

			// check if there were capture, really inefficient.
			//The last chacracter is the pass toggle
			for (int i = 0; i < (int) tmp.size() - 1; ++i)
				cnt += (cur[i] != tmp[i]);

			if (cnt > 1) return 1;

			return 0;
		}
	}
	
	sf::Vector2f buttonSize(155, 75);

	auto check_inside = [&](sf::Vector2f a, sf::Vector2f b) -> bool {
		return (abs(a.x) <= b.x) && (abs(a.y) <= b.y);
	};
	if (!timeLimitSet) {
		//No time limit
		sf::Vector2f horizontal_offset(virtualWindowSize.x * 0.5f, 0), content_offset(horizontal_offset.x - 150, 0);
		sf::Vector2f vertical_offset(0, 400), vertical_gap(0, 75);


		sf::Vector2f func_button = horizontal_offset - content_offset + vertical_offset;
		sf::Vector2f hitbox(100, 35);


		//Save region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			saveGame();
			return 3;
		}
		func_button += vertical_gap;

		//Load region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			loadGame();
			return 3;
		}

		func_button += vertical_gap;

		//Pass region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			board.pass();

			if (autoSaveToggle) saveGame();
			return 0;
		}

		func_button += vertical_gap;

		//Resign region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			board.resign();
			return -1;
		}

		func_button = horizontal_offset + content_offset + vertical_offset;

		//Undo region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			board.undo();

			if (autoSaveToggle) saveGame();
			return 0;
		}

		func_button += vertical_gap;

		//Redo region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			board.redo();

			if (autoSaveToggle) saveGame();
			return 0;
		}

		func_button += vertical_gap;

		//Undo All region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			board.undoAll();

			if (autoSaveToggle) saveGame();
			return 0;
		}

		func_button += vertical_gap;

		//Redo All region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			board.redoAll();

			if (autoSaveToggle) saveGame();
			return 0;
		}
	}
	else {
		sf::Vector2f horizontal_offset(virtualWindowSize.x * 0.5f, 0), content_offset(horizontal_offset.x - 150, 0);
		sf::Vector2f vertical_offset(0, 400), vertical_gap(0, 75);
		sf::Vector2f hitbox(100, 35);

		sf::Vector2f func_button = horizontal_offset + vertical_offset + content_offset;
		
		//Pass region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			board.pass();
			return 0;
		}
		func_button += vertical_gap;

		//Resign region
		if (check_inside(mouse_pos - func_button, hitbox)) {
			board.resign();
			return -1;
		}
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
			fileNotification = "	 Saved\nsuccessfully";
			break;
		case FileStatus::FileNotFound:
			fileNotification = "File not found";
			break;
		case FileStatus::CorruptedFile:
			fileNotification = "Corrupted file";
			break;
		case FileStatus::WrongFormat:
			fileNotification = "Wrong game's\n		format";
			break;
	}

	notificationTimer.restart();
}
void GameUI::loadGame() {
	switch (board.loadGame()) {
		case FileStatus::Success:
			fileNotification = "   Loaded\nsuccessfully";
			break;
		case FileStatus::FileNotFound:
			fileNotification = "File not found";
			board.saveGame();
			break;
		case FileStatus::CorruptedFile:
			fileNotification = "Corrupted file";
			board.saveGame();
			break;
		case FileStatus::WrongFormat:
			fileNotification = "Wrong game's\n		format";
			board.saveGame();
			break;
	}

	savedEndGame = !board.isInGame();

	notificationTimer.restart();
}

//Showing whose turn is it and game saves/loads
void GameUI::loadTurnIndicator() {
	messageBox.setPosition(sf::Vector2f(50, 100));
	messageBox.setSize(sf::Vector2f(200, 240));
	messageBox.setCornerRadius(30);

	if (timeLimitSet) {
		//Saving game states aren't allowed in time limit mode.
		messageBox.addObject(createText(board.getPassState() ? "Pass clicked" : "Pass unclicked", true, sf::Color::White), { messageBox.getSize().x * 0.5f, 60 });


		if (board.getMoveLimit() > 0) {
			messageBox.addObject(createText("Move " + std::to_string(board.getPointer() / 2 + 1) + "/" + std::to_string(board.getMoveLimit()), true, sf::Color::White), {messageBox.getSize().x * 0.5f, 140});
		}
		else {
			messageBox.addObject(createText("Move " + std::to_string(board.getPointer() / 2 + 1), true, sf::Color::White), { messageBox.getSize().x * 0.5f, 140 });
		}
	}
	else {
		messageBox.addObject(createText(board.getPassState() ? "Pass clicked" : "Pass unclicked", true, sf::Color::White), { messageBox.getSize().x * 0.5f, 20 });


		if (board.getMoveLimit() > 0) {
			messageBox.addObject(createText("Move " + std::to_string(board.getPointer() / 2 + 1) + "/" + std::to_string(board.getMoveLimit()), true, sf::Color::White), { messageBox.getSize().x * 0.5f, 100 });
		}
		else {
			messageBox.addObject(createText("Move " + std::to_string(board.getPointer() / 2 + 1), true, sf::Color::White), { messageBox.getSize().x * 0.5f, 100 });
		}

		if (notificationTimer.getElapsedTime() > notificationDuration) {
			fileNotification = "";
		}

		messageBox.addObject(createText(fileNotification, true, sf::Color::White), { messageBox.getSize().x * 0.5f, 180 });
	}
}

sf::Text GameUI::createText(std::string message, bool centered, sf::Color textColor, int characterSize) {
	sf::Text messageTexture(english_font);
	messageTexture.setString(message);
	messageTexture.setFillColor(textColor);
	messageTexture.setCharacterSize(characterSize);

	if (centered) {
		messageTexture.setOrigin(messageTexture.getGlobalBounds().size * 0.5f);
	}
	return messageTexture;
}

//This will displaying the timer of black/white and whose turn is it
//Also return a signal to play end game sound
bool GameUI::loadTimer() {
	if (!board.isInGame()) {
		bool state = playedEndGameSound;
		playedEndGameSound = true;
		return state;
	}

	int turn = board.getTurn();
	sf::Time timePassed = deltaClock.restart();

	if (board.isInGame()) {
		if (timeLimitSet && board.subtractTime(timePassed, turn) == false) {
			board.setGame(false);
			board.setWinByTime(turn ^ 1);
			return false;
		}
	}
	
	blackSide.setSize(sf::Vector2f(200, 100));
	blackSide.setCornerRadius(20);
	whiteSide.setSize(sf::Vector2f(200, 100));
	whiteSide.setCornerRadius(20);

	if (turn != 0) {
		blackSide.setBackgroundColor();
		whiteSide.setBackgroundColor(sf::Color(70, 63, 56, 150));
	} else {
		blackSide.setBackgroundColor(sf::Color(70, 63, 56, 150));
		whiteSide.setBackgroundColor();
	}

	blackSide.setPosition(sf::Vector2f(gameConfig.boardTopLeft.x + gameConfig.borderLimit + 50, 100));
	blackSide.addObject(createText("Black", true, sf::Color::White), sf::Vector2f(blackSide.getSize().x * 0.5f, 20));
	blackSide.addObject(createText(timeLimitSet ? convertTime(board.getTime(0)) : "--:--", true, sf::Color::White), { blackSide.getSize().x * 0.5f, 60 });

	whiteSide.setPosition(sf::Vector2f(gameConfig.boardTopLeft.x + gameConfig.borderLimit + 50, 220));
	whiteSide.addObject(createText("White", true, sf::Color::White), sf::Vector2f(whiteSide.getSize().x * 0.5f, 20));
	whiteSide.addObject(createText(timeLimitSet ? convertTime(board.getTime(1)) : "--:--", true, sf::Color::White), { whiteSide.getSize().x * 0.5f, 60 });
	
	return true;
}

//In-game Annoucement
void GameUI::annouceInGame(sf::RenderWindow& appWindow) {
	if (!board.isInGame()) return;

	loadTurnIndicator();

	messageBox.drawOn(appWindow);
	messageBox.clearCache();

	blackSide.drawOn(appWindow);
	whiteSide.drawOn(appWindow);

	blackSide.clearCache();
	whiteSide.clearCache();
}

//End Popup

void GameUI::loadEndPopup() {
	endPopup.setSize(sf::Vector2f(500, 350));
	endPopup.setCornerRadius(20);

	endPopup.setPosition((convertToFloat(virtualWindowSize) - endPopup.getSize()) * 0.5f);
	
	endPopup.addObject(createText("GAME OVER", true, sf::Color::White, 40), { endPopup.getSize().x * 0.5f, 40 });
;	std::array <int, 2> score = board.getScore();
	
	//Scoring cases
	if (score[0] == -0x3f3f3f3f) {
		endPopup.addObject(createText("White wins by out of time.", true, sf::Color::White, 30), { endPopup.getSize().x * 0.5f, 140 });
	}
	else if (score[1] == -0x3f3f3f3f) {
		endPopup.addObject(createText("Black wins by out of time.", true, sf::Color::White, 30), { endPopup.getSize().x * 0.5f, 140 });
	}
	else if (score[0] == 0x3f3f3f3f) {
		endPopup.addObject(createText("Black wins by resignation.", true, sf::Color::White, 30), { endPopup.getSize().x * 0.5f, 140 });
	}
	else if (score[1] == 0x3f3f3f3f) {
		endPopup.addObject(createText("White wins by resignation.", true, sf::Color::White, 30), { endPopup.getSize().x * 0.5f, 140 });
	}
	else {
		endPopup.addObject(createText("Result: " + std::to_string(score[0]) + "-" + std::to_string(score[1]), true, sf::Color::White, 30), {endPopup.getSize().x * 0.5f, 120});

		if (score[0] == score[1]) {
			endPopup.addObject(createText("Draw", true, sf::Color::White, 30), { endPopup.getSize().x * 0.5f, 160 });
		}
		else {
			endPopup.addObject(createText(std::string((score[0] < score[1] ? "White" : "Black")) + " wins", true, sf::Color::White, 30), { endPopup.getSize().x * 0.5f, 160});
		}
	}

	//Buttons for new game and menu
	newGamePopup.setSize(sf::Vector2f(200, 80));
	newGamePopup.setCornerRadius(20);
	newGamePopup.setBackgroundColor(sf::Color(106, 168, 79, 255));

	newGamePopup.setPosition(endPopup.getPosition() + sf::Vector2f(36, 220));
	newGamePopup.addObject(createText("New game", true, sf::Color::White, 30), newGamePopup.getSize() * 0.5f - sf::Vector2f(0, 5));
	
	menuPopup.setSize(sf::Vector2f(200, 80));
	menuPopup.setCornerRadius(20);
	menuPopup.setBackgroundColor(sf::Color(106, 168, 79, 255));

	menuPopup.setPosition(endPopup.getPosition() + sf::Vector2f(endPopup.getSize().x - (menuPopup.getSize().x + 36), 220));
	menuPopup.addObject(createText("Menu", true, sf::Color::White, 30), menuPopup.getSize() * 0.5f - sf::Vector2f(0, 6.7));
}

//End-game annoucement
void GameUI::annouceEndGame(sf::RenderWindow& appWindow) {
	if (board.isInGame()) return;

	loadEndPopup();

	if (!savedEndGame) {
		board.saveGame();
		savedEndGame = true;
	}

	endPopup.drawOn(appWindow);
	newGamePopup.drawOn(appWindow);
	menuPopup.drawOn(appWindow);

	endPopup.clearCache();
	newGamePopup.clearCache();
	menuPopup.clearCache();
}

void GameUI::autoSave() {
	if (autoSaveToggle) {
		saveGame();
	}
}


void GameUI::autoLoad() {
	if (autoSaveToggle) {
		loadGame();
	}
}

void GameUI::setAutoSaveToggle(int x) {
	autoSaveToggle = x;
}
void GameUI::setBoardOption(int x) {
	boardOption = x;
}

void GameUI::setMoveLimit(int x) {
	board.setMoveLimit(x);
}

void GameUI::setTimeLimit(int id) {
	timeLimitSet = id > 0;

	board.setTimeLimit(id);
}