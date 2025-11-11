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
	void tryPlacingAt(sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });
	void drawShadow(sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });

	void setState(int x, int y, int c);
	int getState(int x, int y);
	bool emptyCell(int x, int y);
	bool outsideBoard(int x, int y);

	void setTurn(bool who);
	bool getTurn();
	
	std::vector <std::pair <int, int>> findComponent(int x, int y, int cell_id);
	std::vector <std::pair <int, int>> capturedPositions(int x, int y, bool turn);

	bool possibleToPlace(int x, int y, bool turn);
private:
	Texture surface;
	Texture pieces[2];

	std::vector <std::string> state_list;
	int state_pointer;

	int row = -1, column = -1;
	bool current_turn;

	sf::Vector2f PIECE_SCALE;
	sf::Vector2f offset, board_offset;
	sf::Vector2f gapX, gapY;

	void placePieceAt(int x, int y);
};

#endif