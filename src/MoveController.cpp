#include <MoveController.hpp>
#include <Helper.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <random>


void MoveController::init() {
	// --- Configuration --- (KataGo)
	const std::string BASE_DIR = std::string(PROJECT_DIR) + "KataGo/";
	referee.set(BASE_DIR);

	// Wait a moment for startup
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Check if it's still alive (Status should be -1)
	int status = referee.get_exit_status();
	if (status == 0) {
		std::cerr << "[FAIL] KataGo exited immediately (Pipe Issue).\n";
	} else {
		std::cerr << "[PASS] KataGo is running! (Status: " << status << ")\n";
	}

	gridSize = -1;
	botTimePassed.restart();
	botMoveRequest = false;
	botTurn = -1;
}

void MoveController::setBotTurn(int id) {
	botTurn = id;
}
int MoveController::getBotTurn() {
	return botTurn;
}

void MoveController::setGameMode(int id) {
	//Setting the game mode
	modeID = GameMode(id);
	if (id == 0) botTurn = -1;
	else {
		std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
		botTurn = rng() % 2;
	}
}

int MoveController::getGameMode() {
	return (int) (modeID);
}

void MoveController::markAsReady() {
	isReady = true;
}
void MoveController::markAsLoading() {
	isReady = false;
}
bool MoveController::isAIReady() {
	return isReady;
}
bool MoveController::isBotRespondingMove() {
	return botMoveRequest;
}

void MoveController::setBoardSize(int size) {
	if (gridSize != -1) {
		referee.sendCommand("clear_board");

		std::string reply = "";
		do {
			reply = referee.waitForReply(5000);
		} while (reply.empty());

		std::cerr << reply << "\n";
	}

	if (gridSize != size) {
		gridSize = size;
		referee.sendCommand("boardsize " + std::to_string(size));

		std::string reply = "";
		do {
			reply = referee.waitForReply(5000);
		} while (reply.empty());

		std::cerr << reply << '\n';
	}

	//Reset bot's spent time
	botTimePassed.restart();
	botMoveRequest = false;
}

int MoveController::getBoardSize() {
	return gridSize;
}

void MoveController::playTurn(int turn, std::pair <int, int> position) {
	//Input the game states into the KataGo
	if (position == std::make_pair(-1, -1)) {
		referee.sendCommand(std::string("play") + (turn ? " W " : " B ") + "pass");
	}
	else {
		referee.sendCommand(std::string("play") + (turn ? " W " : " B ") + cellPosConversion(position.first, position.second, gridSize, gridSize));
	}

	std::string reply = "";
	do {
		reply = referee.waitForReply(5000);
	} while (reply.empty());

	std::cerr << reply << '\n';
}

void MoveController::undo() {
	referee.sendCommand("undo");

	//Undoing the game state

	std::string reply = "";
	do {
		reply = referee.waitForReply(5000);
	} while (reply.empty());

	std::cerr << reply << '\n';
}

void MoveController::loadState() {
	const std::string pgn_path = std::string(PROJECT_DIR) + "KataGo/data.sgf";

	//Load the game data from the saved game states.
	referee.sendCommand("loadsgf " + pgn_path);

	std::string reply = "";
	do {
		reply = referee.waitForReply(5000);
	} while (reply.empty());

	std::cerr << reply << '\n';
}

std::string MoveController::genMove() {
	//If the program hasn't requested the move generation, ask to do that.
	if (!botMoveRequest) {
		botMoveRequest = true;
		botTimePassed.restart();
		referee.sendCommand(std::string("genmove ") + (botTurn ? "W" : "B"));
	}


	//If we haven't reached 3 seconds, don't get the response yet
	if (botTimePassed.getElapsedTime() < sf::seconds(3.0f)) return "";

	positionResponse = referee.getReply();
	
	//If we haven't got the reply, try later.
	if (positionResponse.empty()) return "";


	//Reset the request status
	if (modeID != GameMode::PvP && modeID != GameMode::Easy) {
		positionResponse = positionResponse.substr(2);
		positionResponse.pop_back();
	}

	return positionResponse;
}