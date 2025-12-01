#include <MoveController.hpp>
#include <Helper.hpp>
#include <iostream>
#include <string>

void MoveController::init() {
	// --- Configuration --- (KataGo)
	const std::string BASE_DIR = std::string(PROJECT_DIR) + "KataGo/";
	referee.set(BASE_DIR);

	// Wait a moment for startup
	std::this_thread::sleep_for(std::chrono::seconds(1));

	// Check if it's still alive (Status should be -1)
	int status = referee.get_exit_status();
	if (status == 0) {
		std::cerr << "[FAIL] KataGo exited immediately (Pipe Issue).\n";
	} else {
		std::cerr << "[PASS] KataGo is running! (Status: " << status << ")\n";
	}

	gridSize = -1;
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

void MoveController::setBoardSize(int size) {
	if (gridSize != -1) {
		referee.sendCommand("clear_board");

		std::cerr << "clear_board\n";

		std::string reply = "";
		do {
			reply = referee.waitForReply(5000);
		} while (reply.empty());

		std::cerr << reply << "\n";

		if (reply.empty()) {
			std::cerr << "[WARNING] Bot failed to clear_board.\n";
		}
	}

	if (gridSize != size) {
		gridSize = size;
		referee.sendCommand("boardsize " + std::to_string(size));

		std::cerr << "boardsize " + std::to_string(size) << '\n';

		std::string reply = "";
		do {
			reply = referee.waitForReply(5000);
		} while (reply.empty());

		std::cerr << reply << '\n';

		if (reply.empty()) {
			std::cerr << "[WARNING] Bot failed to set boardsize.\n";
		}
	}
}

int MoveController::getBoardSize() {
	return gridSize;
}

void MoveController::setGameMode(int id) {
	modeID = GameMode(id);
}

void MoveController::playTurn(int turn, std::pair <int, int> position) {
	if (position == std::make_pair(-1, -1)) {
		std::cerr << std::string("play") + (turn ? " W " : " B ") + "pass" << '\n';
		referee.sendCommand(std::string("play") + (turn ? " W " : " B ") + "pass");
	}
	else {
		std::cerr << std::string("play") + (turn ? " W " : " B ") + cellPosConversion(position.first, position.second, gridSize, gridSize) << '\n';
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

	std::cerr << "undo\n";

	std::string reply = "";
	do {
		reply = referee.waitForReply(5000);
	} while (reply.empty());

	std::cerr << reply << '\n';
}

void MoveController::loadState() {
	const std::string pgn_path = std::string(PROJECT_DIR) + "KataGo/data.sgf";

	referee.sendCommand("loadsgf " + pgn_path);

	std::cerr << "loadsgf " + pgn_path << '\n';

	std::string reply = "";
	do {
		reply = referee.waitForReply(5000);
	} while (reply.empty());

	std::cerr << reply << '\n';
}