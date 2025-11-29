#include <Board.hpp>
#include <Helper.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <bitset>


Board::Board() {
	current_turn = 0;
	state_pointer = 0;
	playing = true;
	moveLimit = 0;

	score = { -1, -1 };

	// --- Configuration --- (KataGo)
	const std::string BASE_DIR = std::string(PROJECT_DIR) + "KataGo/";
	gameTrace.set(BASE_DIR);

	// Wait a moment for startup
	std::this_thread::sleep_for(std::chrono::seconds(1));

	// Check if it's still alive (Status should be -1)
	int status = gameTrace.get_exit_status();
	if (status == 0) {
		std::cerr << "[FAIL] KataGo exited immediately (Pipe Issue).\n";
	} else {
		std::cerr << "[PASS] KataGo is running! (Status: " << status << ")\n";
	}
}



void Board::setSize(int _i_row, int _i_column) {
	if (row != -1) {
		gameTrace.sendCommand("clear_board");
		while (gameTrace.waitForReply(1000).empty());
	}

	if (_i_row != row || _i_column != column) {
		row = _i_row;
		column = _i_column;

		gameTrace.sendCommand("boardsize " + std::to_string(_i_row));
		while (gameTrace.waitForReply(1000).empty());
	}
	

	state_pointer = 0;
	state_list.assign(1, std::string(row * column, '.'));
	state_list.back().push_back('0');
}

std::pair <int, int> Board::getSize() {
	return std::make_pair(row, column);
}

void Board::placePieceAt(int x, int y) {
	setState(x, y, current_turn);
	current_turn = 1 ^ current_turn;
}

void Board::setState(int x, int y, int c) {
	/*
		This updates the board state (including rules)
	*/
	if (x < 0 || x >= row || y < 0 || y >= column) {
		std::cerr << "Setting a null cell (" << x << "," << y << ")\n";
		return;
	}

	state_list.erase(state_list.begin() + state_pointer + 1, state_list.end());
	std::vector <std::pair <int, int>> captured = capturedPositions(x, y, c);
	

	state_list.push_back(state_list.back());
	state_list[++state_pointer][x * column + y] = c + '0';

	for (std::pair <int, int> point : captured) {
		state_list[state_pointer][point.first * column + point.second] = '.';
	}

	state_list[state_pointer].back() = '0';


	gameTrace.sendCommand(std::string("play ") + (c == 1 ? "W " : "B ") + cellPosConversion(x, y, row, column));
	while (gameTrace.waitForReply(1000).empty());
}


//Only checks valid cells on the board
int Board::getState(int x, int y) {
	if (outsideBoard(x, y)) return -1;

	return state_list[state_pointer][x * column + y] == '.' ? -1 : state_list[state_pointer][x * column + y] - '0';
}

bool Board::emptyCell(int x, int y) {
	if (outsideBoard(x, y)) return false;

	return state_list[state_pointer][x * column + y] == '.';
}

bool Board::outsideBoard(int x, int y) {
	return x < 0 || x >= row || y < 0 || y >= column;
}

bool Board::getTurn() {
	return current_turn;
}

void Board::setMoveLimit(int target) {
	//Set move limit (50 minimum)
	moveLimit = target;
}
int Board::getMoveLimit() {
	return moveLimit;
}
int Board::getPointer() {
	return state_pointer;
}

std::vector <std::pair <int, int>> Board::findComponent(int x, int y, int cell_id) {
	/*
		This will return a connected component containing (x, y) that is marked <turn>
		Note that (x, y) is included regardless its state.
	*/

	std::vector <std::pair <int, int>> _cell_queue;
	std::bitset <32 * 12> visited;

	visited[x * column + y] = true;
	_cell_queue.emplace_back(x, y);

	const int delta_x[4] = { +1, -1, 0, 0 };
	const int delta_y[4] = { 0, 0, +1, -1 };

	for (int order = 0; order < (int)_cell_queue.size(); ++order) {
		int i, j; std::tie(i, j) = _cell_queue[order];

		for (int iter = 0; iter < 4; ++iter) {
			int new_i = i + delta_x[iter];
			int new_j = j + delta_y[iter];
			
			if (outsideBoard(new_i, new_j)) continue;
			if (getState(new_i, new_j) != cell_id) continue;

			if (visited[new_i * column + new_j]) continue;

			visited[new_i * column + new_j] = true;
			_cell_queue.emplace_back(new_i, new_j);
		}
	}

	return _cell_queue;
}

std::vector <std::pair <int, int>> Board::capturedPositions(int x, int y, bool turn) {
	/*
		Find all captures if place (x, y) in the target turn.
	*/

	std::vector <std::pair <int, int>> subset;
	
	const int delta_x[4] = { -1, +1, 0, 0 };
	const int delta_y[4] = { 0, 0, -1, +1 };
	std::vector <std::vector <int>> marked(row, std::vector <int>(column));

	for (int iter = 0; iter < 4; ++iter) {
		int new_x = x + delta_x[iter], new_y = y + delta_y[iter];
		if (getState(new_x, new_y) != (1 ^ turn)) continue;
		if (marked[new_x][new_y]) continue;

		std::vector <std::pair <int, int>> candidates = findComponent(new_x, new_y, 1 ^ turn);
		int any_liberty = false;

		for (std::pair <int, int> point : candidates) {
			marked[point.first][point.second] = true;

			for (int dir = 0; dir < 4 && !any_liberty; ++dir) {
				if (!emptyCell(point.first + delta_x[dir], point.second + delta_y[dir])) continue;

				any_liberty |= x != point.first + delta_x[dir] || y != point.second + delta_y[dir];
			}
		}

		if (any_liberty) continue;
		
		for (std::pair <int, int>& point : candidates) {
			subset.emplace_back(point);
		}
	}

	return subset;
}

bool Board::possibleToPlace(int x, int y) {
	bool turn = current_turn;
	if (!emptyCell(x, y)) return false;
	/*
		When is a position possible to place a specific color ?
		
		1/If the connected component with the same color has any liberty:
			No captures or captures with NO KO rule violation.
		2/If the component has no liberties:
			Check if this move is a capture that doesn't violate KO rule or not.
	*/

	std::vector <std::pair <int, int>> captured = capturedPositions(x, y, turn);

	std::string current_state = state_list[state_pointer];
	current_state[x * column + y] = turn + '0';

	if (captured.empty()) {
		const int delta_x[4] = { -1, +1, 0, 0 };
		const int delta_y[4] = { 0, 0, -1, +1 };

		std::vector <std::pair <int, int>> component = findComponent(x, y, turn);
		int any_liberty = false;

		for (std::pair <int, int> point : component) {
			for (int iter = 0; iter < 4; ++iter) {
				int adj_x = point.first + delta_x[iter];
				int adj_y = point.second + delta_y[iter];

				if (outsideBoard(adj_x, adj_y)) continue;

				any_liberty |= current_state[adj_x * column + adj_y] == '.';
			}
		}
		
		return any_liberty;
	}

	for (std::pair <int, int> point : captured) {
		current_state[point.first * column + point.second] = '.';
	}
	return state_pointer == 0 || current_state != state_list[state_pointer - 1];
}

//Full board state at the pointer
std::string Board::getState() {
	return state_list[state_pointer];
}

bool Board::isInGame() {
	if (!playing) return false;
	//If pass 2 times or resign -> End already
	
	//If move limit set, check if the stack size exceeds move limit.
	if (moveLimit > 0 && getPointer() / 2 >= moveLimit) return false;

	//No possible moves -> Also end game
	std::string current_pgn = getState();
	for (char x : current_pgn) {
		if (x == '.') {
			return true;
		}
	}

	return (playing = false);
}

//The opponent has already passed the turn or not
bool Board::getPassState() {
	return state_list[state_pointer].back() - '0';
}

bool Board::undo() {
	if (state_pointer > 0) {
		current_turn = 1 ^ current_turn;
		--state_pointer;

		gameTrace.sendCommand("undo");
		while (gameTrace.waitForReply(1000).empty());

		return true;
	}
	return false;
}
void Board::undoAll() {
	while (undo());

	current_turn = state_pointer & 1;
}
bool Board::redo() {
	if (state_pointer + 1 < (int)state_list.size()) {
		for (int i = 0; i < row * column; ++i) {
			if (state_list[state_pointer][i] == '.' && state_list[state_pointer + 1][i] != '.') {
				gameTrace.sendCommand(std::string("play ") + (current_turn ? "W " : "B ") + cellPosConversion(i / column, i % column, row, column));
				while (gameTrace.waitForReply(1000).empty());

				break;
			}
		}

		current_turn = 1 ^ current_turn;
		++state_pointer;
		return true;
	}
	return false;
}
void Board::redoAll() {
	while (redo());

	current_turn = state_pointer & 1;
}

bool Board::pass() {
	if (getPassState()) {
		playing = false;

		return true;
	}

	state_list.erase(state_list.begin() + state_pointer + 1, state_list.end());
	state_list.push_back(state_list.back());
	state_list.back().back() = '1';
	
	state_pointer = (int) state_list.size() - 1;

	current_turn = 1 ^ current_turn;
	return false;
}

void Board::resign() { 
	playing = false; 
	
	state_list.erase(state_list.begin() + state_pointer + 1, state_list.end());
	state_list.push_back(state_list.back());

	state_pointer = (int)state_list.size() - 1;

	score[current_turn ^ 1] = 0x3f3f3f3f;
	score[current_turn] = 0;
}

std::array <int, 2> Board::getScore() {
	if (score[0] != -1 && score[1] != -1) return score;

	/*
		Final score by both sides
		If +inf -> won by resignation
		Else if -inf -> out of time
		Else pass case
	*/

	auto Split = [&](std::string inputStr) -> std::vector <std::string> {
		std::istringstream stream(inputStr);

		std::vector <std::string> result;
		std::string token;

		while (stream >> token) {
			if (token != "=") result.push_back(token);
		}

		return result;
	};

	score = { 0, 0 };
	
	std::string currentState = getState();

	gameTrace.sendCommand("final_status_list dead");
	std::string deadStatus = "";
	do {
		deadStatus = gameTrace.waitForReply(1000);
	} while (deadStatus.empty());
	
	std::vector <std::string> deadStones = Split(deadStatus);
	for (std::string P : deadStones) {
		std::pair <int, int> position = cellPosGet(P, row, column);

		currentState[position.first * column + position.second] = '.';
	}

	state_list.push_back(currentState);
	state_pointer = (int)state_list.size() - 1;

	for (int i = 0; i < row * column; ++i) {
		if (currentState[i] != '.') score[currentState[i] - '0'] += 1;
	}


	//Territory area
	std::bitset <32 * 12> visited;
	const int delta_x[4] = { -1, +1, 0, 0 };
	const int delta_y[4] = { 0, 0, -1, +1 };

	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < column; ++j) {
			if (!emptyCell(i, j)) visited[i * column + j] = true;

			if (visited[i * column + j]) continue;

			std::vector <std::pair <int, int>> component = findComponent(i, j, -1);
			int mask_color = 0;

			for (std::pair <int, int> point : component) {
				visited[point.first * column + point.second] = true;

				for (int dir = 0; dir < 4; ++dir) {
					int adj_x = point.first + delta_x[dir], adj_y = point.second + delta_y[dir];

					if (outsideBoard(adj_x, adj_y)) continue;

					if (!emptyCell(adj_x, adj_y)) mask_color |= 1 << getState(adj_x, adj_y);
				}
			}

			if (mask_color == 1) score[0] += (int)component.size();
			if (mask_color == 2) score[1] += (int)component.size();
		}
	}

	//Komi = 4
	score[1] += 4;
	return score;
}

int Board::saveGame() {
	const std::string fileName = std::string(PROJECT_DIR) + "assets/game.cache";
	std::ofstream fout(fileName);

	/*
		Saving games for future uses
	*/

	if (!fout.is_open()) {
		return FileStatus::FileNotFound;
	}
		
	/*
		stack size - pointer
		state_list
	*/
	fout << (int)state_list.size() << ' ' << state_pointer << ' ' << moveLimit << '\n';
	for (std::string& st : state_list) fout << st << '\n';

	fout << score[0] << ' ' << score[1] << '\n';
	fout.close();

	return FileStatus::Success;
}

int Board::loadGame() {
	const std::string fileName = std::string(PROJECT_DIR) + "assets/game.cache";
	std::ifstream fin(fileName);

	/*
		Loading games
	*/

	if (!fin.is_open() || fin.eof()) {
		return FileStatus::FileNotFound;
	}
	
	int stackSize; 
	int stack_pointer;
	int stackLimit;
	std::vector <std::string> stack_list;

	std::vector <std::array <int, 2>> stackCapture;
	std::array <int, 2> tempScore;

	try {
		if (!(fin >> stackSize >> stack_pointer >> stackLimit)) {
			throw std::runtime_error("Failed to read stack parameters (I/O failure/EOF)");
		}

		if (stackSize <= 0 || stackSize <= stack_pointer) throw std::invalid_argument("Zero-Negative stack size/Invalid stack pointer index.");
		
		if (stackLimit != moveLimit) throw std::invalid_argument("Different move limits");

		stack_list.assign(stackSize, "");

		for (int i = 0; i < stackSize; ++i) {
			if (!(fin >> stack_list[i]) || (int)stack_list[i].length() != row * column + 1) {
				throw std::runtime_error("Missing or incomplete board state in stack list.");
			}

			for (char x : stack_list[i]) {
				if (x != '.' && x != '0' && x != '1') throw std::invalid_argument("Invalid cell state. Found " + std::to_string(x));
			}
		}

		if (!(fin >> tempScore[0] >> tempScore[1])) {
			throw std::runtime_error("Missing/Invalid final score format");
		}
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Format Error: " << e.what() << "\n";
		return FileStatus::WrongFormat;
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Runtime error: " << e.what() << "\n";
		return FileStatus::CorruptedFile;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << '\n';
		return FileStatus::CorruptedFile;
	}

	
	//Avoid loading the game into the bot multiple times
	bool different = state_list != stack_list;

	state_pointer = stack_pointer;
	state_list = stack_list;
	moveLimit = stackLimit;

	score = tempScore;

	current_turn = (state_pointer & 1);
	playing = score[0] == -1 && score[1] == -1;

	if (different && playing) {
		gameTrace.sendCommand("clear_board");
		while (gameTrace.waitForReply(1000).empty());

		for (int i = 0; i + 1 < (int)state_list.size(); ++i) {
			for (int p = 0; p < row * column; ++p) {
				if (state_list[i][p] == '.' && state_list[i + 1][p] != '.') {
					gameTrace.sendCommand(std::string("play ") + (i & 1 ? "W " : "B ") + cellPosConversion(p / column, p % column, row, column));
					while (gameTrace.waitForReply(1000).empty());

					break;
				}
			}
		}
	}

	return FileStatus::Success;
}

void Board::clearGame() {
	state_list.clear();

	current_turn = 0;
	state_pointer = 0;
	playing = true;

	score = { -1, -1 };

	state_list.assign(1, std::string(row * column, '.'));
	state_list.back().push_back('0');

	if (row != -1) {
		gameTrace.sendCommand("clear_board");
		while (gameTrace.waitForReply(1000).empty());
	}
}

void Board::setWinByTime(int turn) {
	playing = false;

	score[turn] = 0;
	score[turn ^ 1] = -0x3f3f3f3f;
}