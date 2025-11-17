#ifndef BOARD_HPP
#define BOARD_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>

class Board {
public:
	Board();
	void setSize(int _i_row, int _i_column);
	std::pair <int, int> getSize();

	void setState(int x, int y, int c);
	int getState(int x, int y);
	bool emptyCell(int x, int y);
	bool outsideBoard(int x, int y);
	bool getTurn();
	
	std::vector <std::pair <int, int>> findComponent(int x, int y, int cell_id);
	std::vector <std::pair <int, int>> capturedPositions(int x, int y, bool turn);
	
	bool possibleToPlace(int x, int y);
	void placePieceAt(int x, int y);

	std::string getState();

	bool isInGame();
	bool getPassState();

	bool undo();
	bool redo();
	bool pass();

	void resign();

	std::array <int, 2> getScore();
private:
	std::vector <std::string> state_list;

	int state_pointer;

	int row = -1, column = -1;
	bool current_turn;
	bool playing;

	std::array <int, 2> score;
};

#endif