#include <Board.hpp>
#include <Helper.hpp>
#include <iostream>


Board::Board() {
	PIECE_SCALE = sf::Vector2f(0.4, 0.4);
	offset = sf::Vector2f(40, 40);
	gapX = sf::Vector2f(65, 0);
	gapY = sf::Vector2f(0, 65);

	current_turn = 0;
}

void Board::loadBoard(const std::string _str_source) {
	surface.load(_str_source);
}
void Board::loadPieces() {
	const std::filesystem::path CHESS_PIECE = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/go_pieces.png");
	
	for (int i = 0; i < 2; ++i) {
		pieces[i] = Texture(CHESS_PIECE.string(), sf::IntRect({ 100 * i, 0 }, { 100, 100 }));
		pieces[i].setScale(PIECE_SCALE);
		pieces[i].setCentered(true);
	}
}

void Board::setSize(int _i_row, int _i_column) {
	row = _i_row;
	column = _i_column;

	state = std::vector <std::string>(row, std::string(column, '.'));
}

std::pair <int, int> Board::getSize() {
	return std::make_pair(row, column);
}

void Board::drawBoard() {
	surface.draw();
}

void Board::drawState() {
	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < column; ++c) {
			if (emptyCell(r, c)) continue;

			int piece_type = getState(r, c);

			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			pieces[piece_type].draw(cur_pos);
		}
	}
}

void Board::placeAt(sf::Vector2f mouse_pos) {
	const float RADIUS = 25;

	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < column; ++c) {
			if (!emptyCell(r, c)) continue;

			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) <= RADIUS) {
				std::cout << r << " " << c << "\n";

				setState(r, c, current_turn);

				current_turn = 1 ^ current_turn;
				return;
			}
		}
	}
}

void Board::drawShadow(sf::Vector2f mouse_pos) {
	const float RADIUS = 25;

	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < column; ++c) {
			if (!emptyCell(r, c)) continue;

			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) <= RADIUS) {
				pieces[current_turn].draw(cur_pos, 0.6);
			}
		}
	}
}

void Board::setState(int x, int y, int c) {
	if (x < 0 || x >= row || y < 0 || y >= column) {
		std::cerr << "Setting a null cell (" << x << "," << y << ")\n";
		return;
	}
	
	state[x][y] = c == -1 ? '.' : c + '0';
}

int Board::getState(int x, int y) {
	if (x < 0 || x >= row || y < 0 || y >= column) {
		std::cerr << "Calling state of a null cell (" << x << "," << y << ")\n";
		return -1;
	}

	return state[x][y] == '.' ? -1 : state[x][y] - '0';
}

bool Board::emptyCell(int x, int y) {
	if (x < 0 || x >= row || y < 0 || y >= column) {
		std::cerr << "Calling state of a null cell (" << x << "," << y << ")\n";
		return -1;
	}

	return state[x][y] == '.';
}

void Board::setTurn(bool who) {
	current_turn = who;
}

bool Board::getTurn() {
	return current_turn;
}