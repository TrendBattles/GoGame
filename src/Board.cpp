#include <Board.hpp>
#include <iostream>

void Board::loadBoard(const std::string _str_source) {
	surface.load(_str_source);
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