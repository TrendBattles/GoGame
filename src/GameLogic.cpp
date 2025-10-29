#include <Render.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <Helper.hpp>

const sf::Vector2f PIECE_SCALE = sf::Vector2f(0.4, 0.4);
const int COLUMN = 9, ROW = 9;
const sf::Vector2f offset(40, 40);
const sf::Vector2f gapX(65, 0), gapY(0, 65);

std::string board_state(9 * 9, '.');

Texture _tex_board;
Texture pieces[2];
// 0 means black, 1 means white
bool current_turn;
int mouse_state;

sf::Vector2f get_mouse_position() {
	return convertToFloat(sf::Mouse::getPosition(appWindow));
}

void load_texture() {
	const std::filesystem::path BOARD_PATH = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/board.png");
	const std::filesystem::path CHESS_PIECE = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/go_pieces.png");

	_tex_board = Texture(BOARD_PATH.string());

	for (int i = 0; i < 2; ++i) {
		pieces[i] = Texture(CHESS_PIECE.string(), sf::IntRect({ 100 * i, 0 }, { 100, 100 }));
		pieces[i].setScale(PIECE_SCALE);
		pieces[i].setCentered(true);
	}
}

void draw_chess_board() {
	_tex_board.draw();
}

void draw_board_state() {
	for (int i = 0; i < ROW * COLUMN; ++i) {
		if (board_state[i] == '.') continue;

		int r = i / COLUMN, c = i % COLUMN;
		sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
		int piece_type = board_state[i] - '0';
		pieces[piece_type].draw(cur_pos);
	}
}

void handle_move() { 
	// IF mouse_state = 1: when hovering a cell, place one there.
	// Then switch to mouse_state = 2 (holding mode), where it won't do anything until you released

	if (mouse_state != 1) return;
	const float RADIUS = 25;
	sf::Vector2f mouse_pos = get_mouse_position();
	for (int i = 0; i < ROW * COLUMN; ++i) if (board_state[i] == '.') {
		int r = i / COLUMN, c = i % COLUMN;
		sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
		if (dist(cur_pos, mouse_pos) <= RADIUS) {
			std::cout << r << " " << c << "\n";
			board_state[i] = current_turn + '0';
			current_turn = 1 ^ current_turn;
			break;
		}
	}
	mouse_state = 2;
}

void draw_shadow() { // when hovering over a cell, you can see it's shadow.
	const float RADIUS = 25;
	sf::Vector2f mouse_pos = get_mouse_position();
	for (int i = 0; i < ROW * COLUMN; ++i) if (board_state[i] == '.'){
		int r = i / COLUMN, c = i % COLUMN;
		sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
		if (dist(cur_pos, mouse_pos) <= RADIUS) {
			pieces[current_turn].draw(cur_pos, 0.6);
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
			if (mouse_state == 0) mouse_state = 1;
		}
		else if (event->is <sf::Event::MouseButtonReleased>()) {
			mouse_state = 0;
		}
	}
	return 1;
}

void appLoop() {
	if (pollEvent() == 0) {
		return;
	}
	appWindow.clear(sf::Color::White);

	draw_chess_board();
	handle_move();
	draw_shadow();
	draw_board_state();

	appWindow.display();

}