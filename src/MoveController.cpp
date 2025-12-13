#include <MoveController.hpp>
#include <Helper.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <random>


void MoveController::init() {
	// --- Configuration --- (KataGo)
	const std::string BASE_DIR = std::string(PROJECT_DIR) + "KataGo/";
	hardBot.set(BASE_DIR, "model_hard.txt.gz");
	mediumBot.set(BASE_DIR, "model_medium.txt.gz");

	// Wait a moment for startup
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Check if it's still alive (Status should be -1)
	int status = hardBot.get_exit_status();
	if (status == 0) {
		std::cerr << "[FAIL] KataGo Hard exited immediately (Pipe Issue).\n";
	} else {
		std::cerr << "[PASS] KataGo Hard is running! (Status: " << status << ")\n";
	}

	status = mediumBot.get_exit_status();
	if (status == 0) {
		std::cerr << "[FAIL] KataGo Medium exited immediately (Pipe Issue).\n";
	}
	else {
		std::cerr << "[PASS] KataGo Medium is running! (Status: " << status << ")\n";
	}

	gridSize = -1;
	botTimePassed.restart();
	botMoveRequest = false;
	botTurn = -1;
}

void MoveController::clearRequest() {
	//Cleaning leftover replies
	mediumBot.clearQueue();
	hardBot.clearQueue();
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
		hardBot.sendCommand("clear_board");
		mediumBot.sendCommand("clear_board");
		

		std::string reply = "";
		do {
			reply = hardBot.waitForReply(5000);
		} while (reply.empty());


		std::cerr << "clear " << (reply[0] == '=' ? "OK" : "FAIL") << "\n";

		reply = "";
		do {
			reply = mediumBot.waitForReply(5000);
		} while (reply.empty());

		std::cerr << "clear " << (reply[0] == '=' ? "OK" : "FAIL") << "\n";
	}

	if (gridSize != size) {
		gridSize = size;
		mediumBot.sendCommand("boardsize " + std::to_string(size));
		hardBot.sendCommand("boardsize " + std::to_string(size));

		std::string reply = "";
		do {
			reply = hardBot.waitForReply(5000);
		} while (reply.empty());


		std::cerr << "set " << (reply[0] == '=' ? "OK" : "FAIL") << "\n";

		reply = "";
		do {
			reply = mediumBot.waitForReply(5000);
		} while (reply.empty());

		std::cerr << "set " << (reply[0] == '=' ? "OK" : "FAIL") << "\n";
	}

	//Reset bot's spent time
	botTimePassed.restart();
	botMoveRequest = false;
}

int MoveController::getBoardSize() {
	return gridSize;
}

void MoveController::playTurn(int turn, std::pair <int, int> position, int botID) {
	//Input the game states into the KataGo
	if (position == std::make_pair(-1, -1)) {
		if (GameMode::Hard != botID) hardBot.sendCommand(std::string("play") + (turn ? " W " : " B ") + "pass");
		if (GameMode::Medium != botID) mediumBot.sendCommand(std::string("play") + (turn ? " W " : " B ") + "pass");
	}
	else {
		if (GameMode::Hard != botID) hardBot.sendCommand(std::string("play") + (turn ? " W " : " B ") + cellPosConversion(position.first, position.second, gridSize, gridSize));
		if (GameMode::Medium != botID) mediumBot.sendCommand(std::string("play") + (turn ? " W " : " B ") + cellPosConversion(position.first, position.second, gridSize, gridSize));
	}

	std::string reply = "";
	
	while (reply.empty() && GameMode::Hard != botID) {
		reply = hardBot.waitForReply(5000);
	} 

	std::cerr << "play " << (GameMode::Hard == botID || reply[0] == '=' ? "OK" : "FAIL") << "\n";

	reply = "";
	while (reply.empty() && GameMode::Medium != botID) {
		reply = mediumBot.waitForReply(5000);
	}

	std::cerr << "play " << (GameMode::Medium == botID || reply[0] == '=' ? "OK" : "FAIL") << "\n";
}

void MoveController::undo() {
	hardBot.sendCommand("undo");
	mediumBot.sendCommand("undo");

	//Undoing the game state

	std::string reply = "";

	while (reply.empty()) {
		reply = hardBot.waitForReply(5000);
	}

	std::cerr << "undo " << (reply[0] == '=' ? "OK" : "FAIL") << "\n";

	reply = "";
	while (reply.empty()) {
		reply = mediumBot.waitForReply(5000);
	}

	std::cerr << "undo " << (reply[0] == '=' ? "OK" : "FAIL") << "\n";
}

void MoveController::loadState() {
	const std::string pgn_path = std::string(PROJECT_DIR) + "KataGo/data.sgf";

	//Load the game data from the saved game states.
	mediumBot.sendCommand("loadsgf " + pgn_path);
	hardBot.sendCommand("loadsgf " + pgn_path);

	std::string reply = "";

	while (reply.empty()) {
		reply = hardBot.waitForReply(5000);
	}

	std::cerr << "save " << (reply[0] == '=' ? "OK" : "FAIL") << "\n";

	reply = "";
	while (reply.empty()) {
		reply = mediumBot.waitForReply(5000);
	}

	std::cerr << "save " << (reply[0] == '=' ? "OK" : "FAIL") << "\n";
}

std::string MoveController::genMove() {
	//If the program hasn't requested the move generation, ask to do that.
	if (!botMoveRequest) {
		botMoveRequest = true;
		botTimePassed.restart();


		if (modeID == GameMode::Hard) hardBot.sendCommand(std::string("genmove ") + (botTurn ? "W" : "B"));
		if (modeID == GameMode::Medium) mediumBot.sendCommand(std::string("genmove ") + (botTurn ? "W" : "B"));
	}


	//If we haven't reached 3 seconds, don't get the response yet
	if (botTimePassed.getElapsedTime() < sf::seconds(3.0f)) return "";

	if (modeID == GameMode::Hard) positionResponse = hardBot.getReply();
	if (modeID == GameMode::Medium) positionResponse = mediumBot.getReply();

	//If we haven't got the reply, try later.
	if (positionResponse.empty()) return "";


	//Reset the request status
	if (modeID == GameMode::Medium || modeID == GameMode::Hard) {
		positionResponse = positionResponse.substr(2);
		positionResponse.pop_back();
	}

	return positionResponse;
}