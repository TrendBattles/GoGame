/*

Log:
- Added the functionality that tell me when I place the piece, what action was it (move, capture, illegal).

*/

#include <Helper.hpp>
#include <Menu.hpp>
#include <OptionMenu.hpp>
#include <AboutMenu.hpp>
#include <MediaPlayer.hpp>
#include <GameUI.hpp>
#include <iostream>
#include <filesystem>

extern sf::RenderWindow appWindow;
const int COLUMN = 9, ROW = 9;

MediaPlayer sound_board;

enum MouseState {
	RELEASE = 0,
	CLICK = 1,
	HOLD = 2
};

enum GameScene {
	MENU = 0,
	GAME = 1,
	OPTION = 2,
	ABOUT = 3
};

GameScene current_scene;

Menu menu;
OptionMenu optionmenu;
AboutMenu aboutmenu;
GameUI gameui;
int mouse_state;

sf::Vector2f get_mouse_position() {
	return convertToFloat(sf::Mouse::getPosition(appWindow));
}

//void load_texture() {
//	const std::filesystem::path BOARD_PATH = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/board.png");
//	const std::filesystem::path CHESS_PIECE = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/go_pieces.png");
//}

void appStart() {
	sound_board.init();
	sound_board.play_background_music();
	//load_texture();
	mouse_state = MouseState::RELEASE;
	current_scene = GameScene::MENU;

	menu.init();
	optionmenu.init();
	aboutmenu.init();
	gameui.init();
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

	if (alreadyClick) {
		mouse_state = MouseState::HOLD;
	}

	return 1;
}

void handle_menu() {
	menu.draw_game_text(appWindow);
	menu.draw_button(appWindow);
	
	if (mouse_state == MouseState::CLICK) {
		int new_signal = menu.tryClickingAt(get_mouse_position());
		switch (new_signal) {
			case 0:
				current_scene = GameScene::GAME;
				gameui.resetGame();
				break;
			case 1:
				current_scene = GameScene::OPTION;
				break;
			case 2:
				current_scene = GameScene::ABOUT;
		}
	}
}


void handle_option_menu() {
	optionmenu.draw_volume_button(appWindow);
	optionmenu.draw_back_button(appWindow);
	sound_board.setAudioVolume(optionmenu.getSoundVolume() * 20);
	sound_board.setMusicVolume(optionmenu.getMusicVolume() * 20);

	if (mouse_state == MouseState::CLICK) {
		int signal = optionmenu.tryClickingAt(get_mouse_position());
		switch (signal) {
			case 0:
				break;
			case 1:
				current_scene = GameScene::MENU;
				break;
		}
	}
}

void handle_about_menu() {
	aboutmenu.draw_back_button(appWindow);
	aboutmenu.draw_UI(appWindow);

	if (mouse_state == MouseState::CLICK) {
		int signal = aboutmenu.tryClickingAt(get_mouse_position());
		switch (signal) {
		case 0:
			break;
		case 1:
			current_scene = GameScene::MENU;
			break;
		}
	}
}

void handle_game_scene() {
	gameui.draw_back_button(appWindow);
	gameui.draw_UI(appWindow);
	gameui.draw_game_buttons(appWindow);

	//gameui.annouceEndGame();

	if (mouse_state == MouseState::CLICK) {
		int signal = gameui.tryClickingAt(get_mouse_position());
		if (signal != -1) {
			if (signal == 10) {
				current_scene = GameScene::MENU;
			}
			else {
				sound_board.play_audio((SoundEffect)signal);
			}
		}
	}
	gameui.drawShadow(appWindow, get_mouse_position());
	/*
	gameui.drawBoard();
	if (mouse_state == MouseState::CLICK) {
		int cur = go_board.tryPlacingAt(get_mouse_position());
		if (cur != -1) {
			sound_board.play_audio((SoundEffect)cur);
		}
		mouse_state == MouseState::HOLD;
	}

	if (mouse_state == MouseState::RELEASE) {
		go_board.drawShadow(get_mouse_position());
	}
	go_board.drawState();*/
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
		case GameScene::OPTION:
			handle_option_menu();
			break;
		case GameScene::ABOUT:
			handle_about_menu();
			break;
	}

	appWindow.display();
}