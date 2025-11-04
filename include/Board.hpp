#ifndef BOARD_HPP
#define BOARD_HPP

#include <SFML/Graphics.hpp>
#include <Texture.hpp>
#include <vector>
#include <utility>

class Board {
public:
	Board();
	
	void loadBoard(const std::string _str_source = "");
	void loadPieces();

	void setSize(int _i_row, int _i_column);
	std::pair <int, int> getSize();

	void drawBoard();
	void drawState();
	void placeAt(sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });
	void drawShadow(sf::Vector2f mouse_pos = sf::Vector2f { 0, 0 });
	
	void setState(int x, int y, int c);
	int getState(int x, int y);
	bool emptyCell(int x, int y);
	bool outsideBoard(int x, int y);

	void setTurn(bool who);
	bool getTurn();
	
	std::vector <std::pair <int, int>> findComponent(int x, int y, int cell_id);

	bool KORule(int x, int y, bool turn);
	bool existCapture(int x, int y, bool turn);
	bool possibleToPlace(int x, int y, bool turn);


private:
	Texture surface;
	Texture pieces[2];

	std::vector <std::string> state;

	int row = -1, column = -1;
	bool current_turn;

	std::pair <int, int> single_cell_capture;

	sf::Vector2f PIECE_SCALE;
	sf::Vector2f offset;
	sf::Vector2f gapX, gapY;
};

#endif