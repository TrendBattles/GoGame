#ifndef BOARD_HPP
#define BOARD_HPP

#include <SFML/Graphics.hpp>
#include <Texture.hpp>
#include <vector>
#include <utility>

class Board {
public:
	Board() {}
	
	void loadBoard(const std::string _str_source = "");
	void setSize(int _i_row, int _i_column);
	std::pair <int, int> getSize();

	void drawBoard();
	void drawState();
	
	void setState(int x, int y, int c);
	int getState(int x, int y);
	bool emptyCell(int x, int y);
private:
	Texture surface;

	std::vector <std::string> state;
	int row = -1, column = -1;
};

#endif