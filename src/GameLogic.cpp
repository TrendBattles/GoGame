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
#include <MoveController.hpp>
#include <iostream>
#include <filesystem>
#include <chrono>


extern sf::RenderWindow appWindow;

MediaPlayer sound_board;

enum MouseState {
	RELEASE = 0,
	CLICKING = 1,
	HOLD = 2
};

enum GameScene {
	MENU =		0b000001,
	GAME =		0b000010,
	OPTION =	0b000100,
	RULES =		0b001000,
	ABOUT =		0b010000,
	LOADING =	0b100000,
	STARTLOAD =	0b100001,
	ENDLOAD =	0b100010
};

GameScene current_scene;

Menu menu;
OptionMenu optionmenu;
AboutMenu aboutmenu;
RulesMenu rulesmenu;

GameUI gameui;
Board board;
BoardTimer Timer;
MoveController moveController;

int mouse_state;

sf::Vector2f get_mouse_position() {
	return convertToFloat(sf::Mouse::getPosition(appWindow));
}

void appStart() {
	mouse_state = MouseState::RELEASE;
	current_scene = GameScene::MENU;

	moveController.init();

	menu.init();
	optionmenu.init();
	aboutmenu.init();
	gameui.init();
	rulesmenu.init();

	sound_board.init();
	sound_board.setAudioType(optionmenu.getAttribute("MUSIC THEME") + 1);
	sound_board.play_background_music();

	sound_board.setAudioVolume(optionmenu.getSoundVolume());
	sound_board.setMusicVolume(optionmenu.getMusicVolume());
}

int pollEvent() { // if window is closed, return 0
	bool alreadyClick = mouse_state == MouseState::CLICKING;
	bool hasEvent = false;

	while (const std::optional event = appWindow.pollEvent()) {
		hasEvent = true;

		if (event->is <sf::Event::Closed>()) {
			std::cerr << "Closing the window" << std::endl;
			appWindow.close();

			return 0;
		}
		else if (event->is <sf::Event::MouseButtonPressed>()) {
			mouse_state = MouseState::CLICKING;
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
	
	if (mouse_state == MouseState::CLICKING) {
		int signal = menu.tryClickingAt(get_mouse_position());
		if (signal >= 0) {
			sound_board.play_audio(SoundEffect::CLICK);
		}
		switch (signal) {
			case 0:
				current_scene = GameScene::STARTLOAD;
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
	optionmenu.draw_selection_button(appWindow, get_mouse_position());
	optionmenu.draw_back_button(appWindow, get_mouse_position());

	if (mouse_state == MouseState::CLICKING) {
		int signal = optionmenu.tryClickingAt(get_mouse_position());
		switch (signal) {
			case 0:
				break;
			case 1:
				current_scene = GameScene::MENU;
				break;
			case 2:
				current_scene = GameScene::STARTLOAD;
				break;
		}
		if (signal >= 0) {
			sound_board.play_audio(SoundEffect::CLICK);
		}
		if (signal == -2) {
			sound_board.play_audio(SoundEffect::ILLEGAL);
		}

		if (sound_board.setAudioType(optionmenu.getAttribute("MUSIC THEME") + 1))
			sound_board.play_background_music();
	}

	if (mouse_state == MouseState::HOLD) {
		optionmenu.fixAudio(get_mouse_position());
	}

	sound_board.setAudioVolume(optionmenu.getSoundVolume());
	sound_board.setMusicVolume(optionmenu.getMusicVolume());
}

void handle_about_menu() {
	aboutmenu.draw_back_button(appWindow, get_mouse_position());
	aboutmenu.draw_UI(appWindow);

	if (mouse_state == MouseState::CLICKING) {
		int signal = aboutmenu.tryClickingAt(get_mouse_position());
		if (signal >= 0) {
			sound_board.play_audio(SoundEffect::CLICK);
		}
		switch (signal) {
		case 0:
			break;
		case 1:
			current_scene = GameScene::MENU;
			break;
		case 2:
			current_scene = GameScene::STARTLOAD;
			break;
		}
	}
}

void handle_rules_menu() {
	rulesmenu.draw_back_button(appWindow, get_mouse_position());
	rulesmenu.draw_UI(appWindow);

	if (mouse_state == MouseState::CLICKING) {
		int signal = rulesmenu.tryClickingAt(get_mouse_position());
		if (signal >= 0){
			sound_board.play_audio(SoundEffect::CLICK);
		}
		switch (signal) {
		case 0:
			break;
		case 1:
			current_scene = GameScene::MENU;
			break;
		case 2:
			current_scene = GameScene::STARTLOAD;
			break;
		}
	}
}

void handle_game_scene() {
	gameui.draw_back_button(appWindow, get_mouse_position());
	gameui.draw_UI(appWindow);
	gameui.draw_game_buttons(appWindow, get_mouse_position());

	gameui.botPlay();

	if (mouse_state == MouseState::CLICKING) {
		int signal = gameui.tryClickingAt(appWindow, get_mouse_position());

		if (signal >= 100) {
			//Thresold for creating a new loading thread
			
			current_scene = GameScene::LOADING;
			signal -= 100;
		}

		if (signal != -1) {
			switch (signal) {
				case 10:
					current_scene = GameScene::MENU;
					sound_board.play_audio(SoundEffect::CLICK);
					break;
				case 20:
					current_scene = GameScene::OPTION;
					sound_board.play_audio(SoundEffect::CLICK);
					break;
				default:
					sound_board.play_audio((SoundEffect)signal);
					break;
			}
		}
	}

	gameui.annouceInGame(appWindow);

	if (!gameui.loadTimer()) {
		sound_board.play_audio(SoundEffect::ENDGAME);
	}

	gameui.annouceEndGame(appWindow);

	gameui.drawShadow(appWindow, get_mouse_position());
}

void startLoading() {
	board.setMoveLimit(50 * optionmenu.getAttribute("MOVE LIMIT"));
	gameui.setAutoSaveToggle(optionmenu.getSaveToggle());
	gameui.setGameMode(optionmenu.getAttribute("GAME MODE"));
	gameui.initGame(optionmenu.getAttribute("BOARD SIZE"));
	
	current_scene = GameScene::LOADING;

	//When the game is not automatically saaved, we create a new loading thread.
	if (!gameui.autoLoad()) {
		std::thread([=]() {
			// Set Board Size (The Blocking Operation!)
			// Since we are on a background thread, blocking here is FINE.
			// It won't freeze the "Loading..." animation.
			auto minEndTime = std::chrono::steady_clock::now() + std::chrono::seconds(2);

			moveController.markAsLoading();

			moveController.setBoardSize(board.getSize().first);

			// Wait for the reminder 
			std::this_thread::sleep_until(minEndTime);

			// Signal Completion
			// You need to add a method to MoveController to trigger the "Ready" flag

			moveController.markAsReady();
		}).detach();
	}
}

void waitLoading() {
	gameui.drawLoadingScreen(appWindow);

	if (!moveController.isAIReady()) return;

	Timer.setTimeLimit(optionmenu.getAttribute("TIME LIMIT"));
	Timer.resetClock();

	sound_board.play_audio(SoundEffect::STARTGAME);
	current_scene = GameScene::GAME;
}

void appLoop() {
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
			handle_rules_menu();
			break;
		case GameScene::OPTION:
			handle_option_menu();
			break;
		case GameScene::ABOUT:
			handle_about_menu();
			break;
		case GameScene::LOADING:
			waitLoading();
			break;
		case GameScene::STARTLOAD:
			startLoading();
			break;
	}

	appWindow.display();
}