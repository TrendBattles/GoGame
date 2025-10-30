#include <Render.hpp>
#include <Helper.hpp>
#include <Board.hpp>

#include <iostream>
#include <filesystem>

const sf::Vector2f PIECE_SCALE = sf::Vector2f(0.4, 0.4);
const int COLUMN = 9, ROW = 9;
const sf::Vector2f offset(40, 40);
const sf::Vector2f gapX(65, 0), gapY(0, 65);

Board go_board;
Texture pieces[2];

enum MouseState {
	RELEASED = 0,
	CLICK = 1,
	HOLD = 2
};

// 0 means black, 1 means white
bool current_turn;
int mouse_state;

sf::Vector2f get_mouse_position() {
	return convertToFloat(sf::Mouse::getPosition(appWindow));
}

void load_texture() {
	const std::filesystem::path BOARD_PATH = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/board.png");
	const std::filesystem::path CHESS_PIECE = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/go_pieces.png");

	go_board.loadBoard(BOARD_PATH.string());
	go_board.setSize(ROW, COLUMN);

	for (int i = 0; i < 2; ++i) {
		pieces[i] = Texture(CHESS_PIECE.string(), sf::IntRect({ 100 * i, 0 }, { 100, 100 }));
		pieces[i].setScale(PIECE_SCALE);
		pieces[i].setCentered(true);
	}
}

void draw_board_state() {
	for (int r = 0; r < ROW; ++r) {
		for (int c = 0; c < COLUMN; ++c) {
			if (go_board.emptyCell(r, c)) continue;

			int piece_type = go_board.getState(r, c);
			 
			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			pieces[piece_type].draw(cur_pos);
		}
	}
}


void handle_move() { 
	// IF mouse_state = 1: when hovering a cell, place one there.
	// Then switch to mouse_state = 2 (holding mode), where it won't do anything until you released

	if (mouse_state != MouseState::CLICK) return;

	const float RADIUS = 25;
	sf::Vector2f mouse_pos = get_mouse_position();

	for (int r = 0; r < ROW; ++r) {
		for (int c = 0; c < COLUMN; ++c) {
			if (!go_board.emptyCell(r, c)) continue;

			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) <= RADIUS) {
				std::cout << r << " " << c << "\n";
				
				go_board.setState(r, c, current_turn);
				mouse_state = MouseState::HOLD;
				current_turn = 1 ^ current_turn;
				

				return;
			}
		}
	}
}

void draw_shadow() { // when hovering over a cell, you can see it's shadow.
	if (mouse_state != MouseState::RELEASED) return;

	const float RADIUS = 25;
	sf::Vector2f mouse_pos = get_mouse_position();

	for (int r = 0; r < ROW; ++r) {
		for (int c = 0; c < COLUMN; ++c) {
			if (!go_board.emptyCell(r, c)) continue;
			
			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) <= RADIUS) {
				pieces[current_turn].draw(cur_pos, 0.6);
			}
		}
	}
}

void appStart() {
	load_texture();
	current_turn = 0;
	mouse_state = 0;
}

int pollEvent() { // if window is closed, return 0
	while (const std::optional event = appWindow.pollEvent()) {
		if (event->is <sf::Event::Closed>()) {
			std::cerr << "Closing the window" << std::endl;
			appWindow.close();

			return 0;
		}
		else if (event->is <sf::Event::MouseButtonPressed>()) {
			if (mouse_state == MouseState::RELEASED) mouse_state = MouseState::CLICK;
		}
		else if (event->is <sf::Event::MouseButtonReleased>()) {
			mouse_state = MouseState::RELEASED;
		}
	}
	return 1;
}

void appLoop() {
	if (pollEvent() == 0) {
		return;
	}

	appWindow.clear(sf::Color::White);

	go_board.drawBoard();

	handle_move();
	draw_shadow();
	draw_board_state();

	appWindow.display();
}