#ifndef BOARD_HPP
#define BOARD_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include <MoveController.hpp>

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
	
	void setBotTurn(int id);
	
	void setMoveLimit(int target = 50);
	int getMoveLimit();
	bool isInGame();
	bool getPassState();

	std::string getState();
	int getPointer();
	
	std::vector <std::pair <int, int>> findComponent(int x, int y, int cell_id);
	std::vector <std::pair <int, int>> capturedPositions(int x, int y, bool turn);
	
	bool possibleToPlace(int x, int y);
	void placePieceAt(int x, int y);

	bool undo(MoveController& moveController);
	void undoAll(MoveController& moveController);
	bool redo(MoveController& moveController);
	void redoAll(MoveController& moveController);
	bool pass();
	void resign();
	
	std::array <int, 2> getScore();

	int saveGame();
	int loadGame(MoveController& moveController);
	void clearGame();

	void setWinByTime(int turn);
private:
	std::vector <std::string> state_list, moveHistory;

	int state_pointer;

	int row = -1, column = -1;
	bool current_turn;
	bool playing;

	//Move Limit parameters
	int moveLimit;

	std::array <int, 2> score;
	int botTurn;
};
#endif