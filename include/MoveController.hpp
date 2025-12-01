#ifndef MOVECONTROLLER_HPP
#define MOVECONTROLLER_HPP

#include <SFML/Graphics.hpp>
#include <KataGo.hpp>
#include <utility>
#include <atomic>
#include <string>

//Receiving Go move signals and game mode management

enum GameMode {
	PvP = 0,
	Easy = 1,
	Medium = 2,
	Hard = 3
};
class MoveController {
public:
	void init();

	void markAsReady();
	void markAsLoading();
	bool isAIReady();

	void setBoardSize(int size);
	int getBoardSize();

	void setGameMode(int id = 0);
	void playTurn(int turn, std::pair <int, int> position);

	void undo();

	void loadState();
private:
	KataGoManager referee;
	GameMode modeID;

	int gridSize;

	std::atomic <bool> isReady{ true };
};
#endif