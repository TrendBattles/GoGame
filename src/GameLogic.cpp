#include <Render.hpp>
#include <Helper.hpp>
#include <Board.hpp>

#include <iostream>
#include <filesystem>

const int COLUMN = 9, ROW = 9;

Board go_board;

enum MouseState {
	RELEASE = 0,
	CLICK = 1,
	HOLD = 2
};

// 0 means black, 1 means white
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

//void draw_board_state() {
//	for (int r = 0; r < ROW; ++r) {
//		for (int c = 0; c < COLUMN; ++c) {
//			if (go_board.emptyCell(r, c)) continue;
//
//			int piece_type = go_board.getState(r, c);
//			 
//			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
//			pieces[piece_type].draw(cur_pos);
//		}
//	}
//}


//void handle_move() { 
//	// IF mouse_state = 1: when hovering a cell, place one there.
//	// Then switch to mouse_state = 2 (holding mode), where it won't do anything until you RELEASE
//
//	if (mouse_state != MouseState::CLICK) return;
//
//	mouse_state = MouseState::HOLD;
//
//	const float RADIUS = 25;
//	sf::Vector2f mouse_pos = get_mouse_position();
//
//	for (int r = 0; r < ROW; ++r) {
//		for (int c = 0; c < COLUMN; ++c) {
//			if (!go_board.emptyCell(r, c)) continue;
//
//			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
//			if (dist(cur_pos, mouse_pos) <= RADIUS) {
//				std::cout << r << " " << c << "\n";
//				
//				go_board.setState(r, c, current_turn);
//				
//				current_turn = 1 ^ current_turn;
//				
//				return;
//			}
//		}
//	}
//}
//
//void draw_shadow() { // when hovering over a cell, you can see it's shadow.
//	const float RADIUS = 25;
//	sf::Vector2f mouse_pos = get_mouse_position();
//
//	for (int r = 0; r < ROW; ++r) {
//		for (int c = 0; c < COLUMN; ++c) {
//			if (!go_board.emptyCell(r, c)) continue;
//
//			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
//			if (dist(cur_pos, mouse_pos) <= RADIUS) {
//				pieces[current_turn].draw(cur_pos, 0.6);
//			}
//		}
//	}
//}

void appStart() {
	load_texture();
	mouse_state = MouseState::RELEASE;
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

void appLoop() {
	if (pollEvent() == 0) {
		return;
	}

	appWindow.clear(sf::Color::White);

	go_board.drawBoard();

	if (mouse_state == MouseState::CLICK) {
		go_board.placeAt(get_mouse_position());
	}

	if (mouse_state == MouseState::RELEASE) {
		go_board.drawShadow(get_mouse_position());
	}

	go_board.drawState();

	appWindow.display();
}