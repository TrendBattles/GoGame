#ifndef BOARD_HPP
#define BOARD_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>

static enum FileStatus {
	Success = 0,
	FileNotFound = 101,
	CorruptedFile = 102,
	WrongFormat = 103
};

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
	
	void setGame(bool gameOn = true);
	void setMoveLimit(int target = 50);
	int getMoveLimit();

	int getPointer();
	
	std::vector <std::pair <int, int>> findComponent(int x, int y, int cell_id);
	std::vector <std::pair <int, int>> capturedPositions(int x, int y, bool turn);
	
	bool possibleToPlace(int x, int y);
	void placePieceAt(int x, int y);

	std::string getState();

	bool isInGame();
	bool getPassState();

	bool undo();
	void undoAll();
	bool redo();
	void redoAll();
	bool pass();

	void resign();
	
	std::array <int, 2> getCapture();
	std::array <int, 2> getScore();

	int saveGame();
	int loadGame();
	void clearGame();

	void setTimeLimit(int id = 0);
	void resetClock();
	void addTime(int turn = 0);
	
	sf::Time getTime(int turn = 0);
	bool subtractTime(sf::Time deltaClock, int turn);
	void setWinByTime(int turn);
private:
	std::vector <std::string> state_list;

	int state_pointer;

	int row = -1, column = -1;
	bool current_turn;
	bool playing;

	//Move Limit parameters
	int moveLimit;

	std::array <int, 2> score;
	std::vector <std::array <int, 2>> numCapture;
		
	//Time limit parameters
	sf::Time timeRemaining[2], timeAdd, timeLimit;
};

#endif