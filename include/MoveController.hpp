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

	int getBotTurn();
	void setGameMode(int id);
	int getGameMode();

	void markAsReady();
	void markAsLoading();
	bool isAIReady();
	bool isBotRespondingMove();
	

	void setBoardSize(int size);
	int getBoardSize();

	void playTurn(int turn, std::pair <int, int> position);

	void undo();
	void loadState();

	std::string genMove();

	sf::Clock botTimePassed;
	bool botMoveRequest;
	std::string positionResponse;

private:
	KataGoManager referee;
	GameMode modeID;
	int botTurn;

	int gridSize;
	std::atomic <bool> isReady{ true };
};
#endif