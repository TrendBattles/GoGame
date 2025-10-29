#include <Render.hpp>
#include <iostream>
#include <filesystem>

const sf::Vector2f PIECE_SCALE = sf::Vector2f(0.4, 0.4);

std::string board_state(9 * 9, '.');

Texture _tex_board;
Texture black_pieces, white_pieces;

void load_texture() {
	const std::filesystem::path BOARD_PATH = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/board.png");
	const std::filesystem::path CHESS_PIECE = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/go_pieces.png");

	_tex_board = Texture(BOARD_PATH.string());
	black_pieces = Texture(CHESS_PIECE.string(), sf::IntRect({ 0, 0 }, { 100, 100 }));
	white_pieces = Texture(CHESS_PIECE.string(), sf::IntRect({ 100, 0 }, { 100, 100 }));

	
	black_pieces.setScale(PIECE_SCALE);
	white_pieces.setScale(PIECE_SCALE);
	black_pieces.setCentered(true);
	white_pieces.setCentered(true);
}

void draw_chess_board() {
	_tex_board.draw();
}

void draw_board_state() {
	const int COLUMN = 9, ROW = 9;
	sf::Vector2f offset(40, 40);
	sf::Vector2f gap(65, 65);

	for (int i = 0; i < 81; ++i) {
		if (i % 3 == 0) board_state[i] = 'W';
		else if (i % 3 == 1) board_state[i] = 'B';
	}
	for (int i = 0; i < ROW * COLUMN; ++i) {
		if (board_state[i] == '.') continue;

		int r = i / COLUMN, c = i % COLUMN;
		sf::Vector2f cur_pos = sf::Vector2f(offset.x + c * gap.x, offset.y + r * gap.y);
		if (board_state[i] == 'B') {
			black_pieces.draw(cur_pos);
		}
		else if (board_state[i] == 'W') {
			white_pieces.draw(cur_pos);
		}
	}
}

void appStart() {
	load_texture();
}

void appLoop() {
	appWindow.clear(sf::Color::White);

	draw_chess_board();
	draw_board_state();

	appWindow.display();

	while (const std::optional event = appWindow.pollEvent()) {
		if (event->is <sf::Event::Closed>()) {
			std::cerr << "Closing the window" << std::endl;
			appWindow.close();

			return;
		}
	}
}