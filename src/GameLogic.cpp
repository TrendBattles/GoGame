#include <Render.hpp>
#include <Helper.hpp>
#include <Board.hpp>
#include <Menu.hpp>

#include <iostream>
#include <filesystem>

const int COLUMN = 9, ROW = 9;

Board go_board;

enum MouseState {
	RELEASE = 0,
	CLICK = 1,
	HOLD = 2
};

enum GameScene {
	MENU = 0,
	GAME = 1
};

GameScene current_scene;
Menu menu;
int mouse_state;

sf::Vector2f get_mouse_position() {
	return convertToFloat(sf::Mouse::getPosition(appWindow));
}

void load_texture() {
	const std::filesystem::path BOARD_PATH = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/board.png");
	const std::filesystem::path CHESS_PIECE = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/go_pieces.png");

	go_board.loadBoard(BOARD_PATH.string());
	go_board.setSize(ROW, COLUMN);
	go_board.loadPieces();
}

void appStart() {
	load_texture();
	mouse_state = MouseState::RELEASE;
	current_scene = GameScene::MENU;
	menu.init();
}

int pollEvent() { // if window is closed, return 0
	bool alreadyClick = mouse_state == MouseState::CLICK;
	bool hasEvent = false;

	while (const std::optional event = appWindow.pollEvent()) {
		hasEvent = true;

		if (event->is <sf::Event::Closed>()) {
			std::cerr << "Closing the window" << std::endl;
			appWindow.close();

			return 0;
		}
		else if (event->is <sf::Event::MouseButtonPressed>()) {
			mouse_state = MouseState::CLICK;
		}
		else if (event->is <sf::Event::MouseButtonReleased>()) {
			alreadyClick = false;
			mouse_state = MouseState::RELEASE;
		}
	}

	if (alreadyClick && hasEvent) {
		mouse_state = MouseState::HOLD;
	}

	return 1;
}

void handle_menu() {
	// drawing the background

	// drawing the logo
	menu.draw_game_text(appWindow);

	// drawing the buttons
	menu.draw_button(appWindow);
	
	if (mouse_state == MouseState::CLICK) {
		int new_signal = menu.tryClickingAt(get_mouse_position());
		if (new_signal == 0) {
			current_scene = GameScene::GAME;
		}
		mouse_state = MouseState::HOLD;
	}
}

void handle_game_scene() {
	go_board.drawBoard();

	if (mouse_state == MouseState::CLICK) {
		go_board.tryPlacingAt(get_mouse_position());
	}

	if (mouse_state == MouseState::RELEASE) {
		go_board.drawShadow(get_mouse_position());
	}

	go_board.drawState();
}

void appLoop() {
	if (pollEvent() == 0) {
		return;
	}

	appWindow.clear(sf::Color(112, 59, 59));

	switch (current_scene) {
		case GameScene::MENU:
			handle_menu();
			break;
		case GameScene::GAME:
			handle_game_scene();
			break;
	}

	appWindow.display();
}