 /*
Log:
- Added the functionality that tell me when I place the piece, what action was it (move, capture, illegal).
*/

#include <Helper.hpp>
#include <GameLogic.hpp>
#include <Menu.hpp>
#include <OptionMenu.hpp>
#include <AboutMenu.hpp>
#include <RulesMenu.hpp>
#include <MediaPlayer.hpp>
#include <GameUI.hpp>
#include <iostream>
#include <filesystem>

extern sf::RenderWindow appWindow;

MediaPlayer sound_board;

enum MouseState {
	RELEASE = 0,
	CLICK = 1,
	HOLD = 2
};

enum GameScene {
	MENU = 0b0001,
	GAME = 0b0010,
	OPTION = 0b0100,
	RULES = 0b1000,
	ABOUT = 0b10000
};

GameScene current_scene;

Menu menu;
OptionMenu optionmenu;
AboutMenu aboutmenu;
RulesMenu rulesmenu;

GameUI gameui;
int mouse_state;

sf::Vector2f get_mouse_position() {
	return convertToFloat(sf::Mouse::getPosition(appWindow));
}

void appStart() {
	mouse_state = MouseState::RELEASE;
	current_scene = GameScene::MENU;

	menu.init();
	optionmenu.init();
	aboutmenu.init();
	gameui.init();
	rulesmenu.init();

	sound_board.init();
	sound_board.setAudioType(optionmenu.getAttribute("MUSIC THEME") + 1);
	sound_board.play_background_music();

	sound_board.setAudioVolume(optionmenu.getSoundVolume() * 20);
	sound_board.setMusicVolume(optionmenu.getMusicVolume() * 20);
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
	menu.draw_button(appWindow, get_mouse_position());
	
	if (mouse_state == MouseState::CLICK) {
		int new_signal = menu.tryClickingAt(get_mouse_position());
		switch (new_signal) {
			case 0:
				current_scene = GameScene::GAME;
				gameui.initGame();
				break;
			case 1:
				current_scene = GameScene::OPTION;
				break;
			case 2:
				current_scene = GameScene::RULES;
				break;
			case 3:
				current_scene = GameScene::ABOUT;
				break;
			case 4:
				std::cerr << "Closing the window" << std::endl;
				appWindow.close();
				break;
		}
	}
}


void handle_option_menu() {
	optionmenu.draw_feature_button(appWindow);
	optionmenu.draw_selection_button(appWindow);
	optionmenu.draw_back_button(appWindow);

	if (mouse_state == MouseState::CLICK) {
		int signal = optionmenu.tryClickingAt(get_mouse_position());
		switch (signal) {
			case 0:
				break;
			case 1:
				current_scene = GameScene::MENU;
				break;
		}

		if (sound_board.setAudioType(optionmenu.getAttribute("MUSIC THEME") + 1))
			sound_board.play_background_music();
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

void handle_rules_scene() {
	rulesmenu.draw_back_button(appWindow);
	rulesmenu.draw_UI(appWindow);

	if (mouse_state == MouseState::CLICK) {
		int signal = rulesmenu.tryClickingAt(get_mouse_position());
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

	if (mouse_state == MouseState::CLICK) {
		int signal = gameui.tryClickingAt(appWindow, get_mouse_position());

		if (signal != -1) {
			switch (signal) {
				case 10:
					current_scene = GameScene::MENU;
					break;
				case 20:
					current_scene = GameScene::MENU;
					break;
				default:
					sound_board.play_audio((SoundEffect)signal);
					break;
			}
		}
	}

	gameui.annouceInGame(appWindow);
	gameui.annouceEndGame(appWindow);

	gameui.drawShadow(appWindow, get_mouse_position());
}

void globalSetFunction() {
	//Sound settings
	sound_board.setAudioVolume(optionmenu.getSoundVolume() * 20);
	sound_board.setMusicVolume(optionmenu.getMusicVolume() * 20);

	//Game UI
	gameui.setAutoSaveToggle(optionmenu.getSaveToggle());
	gameui.setBoardOption(optionmenu.getAttribute("BOARD SIZE"));
	gameui.setMoveLimit(50 * optionmenu.getAttribute("MOVE LIMIT"));
	gameui.setTimeLimit(optionmenu.getAttribute("TIME LIMIT"));
}

void appLoop() {
	globalSetFunction();

	if (pollEvent() == 0) {
		return;
	}

	int cur_theme = optionmenu.getAttribute("MUSIC THEME");
	sf::Color BG_COLOR = sf::Color::White;
	switch (cur_theme) {
	case 0:
		BG_COLOR = sf::Color(0, 126, 110);
		break;
	case 1:		
		BG_COLOR = sf::Color(84, 8, 99);
		break;
	case 2: 
		BG_COLOR = sf::Color(118, 21, 60);
		break;
	}
	appWindow.clear(BG_COLOR);

	switch (current_scene) {
		case GameScene::MENU:
			handle_menu();
			break;
		case GameScene::GAME:
			handle_game_scene();
			break;
		case GameScene::RULES:
			handle_rules_scene();
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