#include <Board.hpp>
#include <Helper.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <bitset>


Board::Board() {
	current_turn = 0;
	state_pointer = 0;
	playing = true;
	moveLimit = 0;

	score = { -1, -1 };
}



void Board::setSize(int _i_row, int _i_column) {
	if (_i_row != row || _i_column != column) {
		row = _i_row;
		column = _i_column;
	}
	
	current_turn = 0;
	state_pointer = 0;
	playing = true;

	score = { -1, -1 };
	state_pointer = 0;
	state_list.assign(1, std::string(row * column, '.'));
	state_list.back().push_back('0');

	moveHistory.assign(1, "---");
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

	moveHistory.erase(moveHistory.begin() + state_pointer, moveHistory.end());
	moveHistory.push_back(cellPosConversion(x, y, row, column));
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

bool Board::undo(MoveController& moveController) {
	if (state_pointer > 0) {
		current_turn = 1 ^ current_turn;
		--state_pointer;

		moveController.undo();

		return true;
	}
	return false;
}
void Board::undoAll(MoveController& moveController) {
	while (undo(moveController));

	current_turn = state_pointer & 1;
}
bool Board::redo(MoveController& moveController) {
	if (state_pointer + 1 < (int)state_list.size()) {
		current_turn = 1 ^ current_turn;
		++state_pointer;

		moveController.playTurn(current_turn ^ 1, cellPosGet(moveHistory[state_pointer], row, column));

		return true;
	}
	return false;
}
void Board::redoAll(MoveController& moveController) {
	while (redo(moveController));

	current_turn = state_pointer & 1;
}

bool Board::pass() {
	state_list.erase(state_list.begin() + state_pointer + 1, state_list.end());
	moveHistory.erase(moveHistory.begin() + state_pointer + 1, moveHistory.end());

	if (getPassState()) {
		playing = false;

		return true;
	}

	
	state_list.push_back(state_list.back());
	state_list.back().back() = '1';
	moveHistory.push_back("---");

	state_pointer = (int) state_list.size() - 1;

	current_turn = 1 ^ current_turn;
	return false;
}

void Board::resign() { 
	playing = false; 
	
	state_list.erase(state_list.begin() + state_pointer + 1, state_list.end());
	state_list.push_back(state_list.back());

	state_pointer = (int)state_list.size() - 1;

	moveHistory.erase(moveHistory.begin() + state_pointer, moveHistory.end());
	moveHistory.push_back("---");

	score[current_turn ^ 1] = 0x3f3f3f3f;
	score[current_turn] = 0;
}

std::array <int, 2> Board::getScore() {
	if (score[0] != -1 && score[1] != -1) return score;

	/*
		Final score by both sides (by inputting the final state)
		If +inf -> won by resignation
		Else if -inf -> out of time
		Else pass case
	*/

	score = { 0, 0 };

	//Removing dead stones
	auto removeDeadStones = [&]() -> void {
		state_list.push_back(state_list.back());
		moveHistory.push_back("---");
		state_pointer = (int)state_list.size() - 1;
			
		std::string& currentState = state_list.back();

		std::vector <std::vector <int>> group_mark(row, std::vector <int>(column, -1));
	
		for (int i = 0; i < row; ++i) {
			for (int j = 0; j < column; ++j) {
				if (emptyCell(i, j)) continue;

				if (group_mark[i][j] != -1) continue;

				//Finding group chain
				std::vector <std::pair <int, int>> component = findComponent(i, j, getState(i, j));
				const int delta_x[4] = { +1, -1, 0, 0 };
				const int delta_y[4] = { 0, 0, +1, -1 };

				int eyes_count = 0;

				std::bitset <32 * 12> adjacentEmptyToggle;
				for (std::pair <int, int> point : component) {
					for (int dir = 0; dir < 4; ++dir) {
						int adj_x = point.first + delta_x[dir], adj_y = point.second + delta_y[dir];

						if (!emptyCell(adj_x, adj_y) || adjacentEmptyToggle[adj_x * column + adj_y]) continue;
						

						std::vector <std::pair <int, int>> emptyComponent = findComponent(adj_x, adj_y, -1);
						eyes_count += 1;

						for (std::pair <int, int> emptyPoint : emptyComponent) {
							int emptyId = emptyPoint.first * column + emptyPoint.second;

							adjacentEmptyToggle[emptyId] = true;
						}
					}
				}
				
				//eyes_count >= 2 => Alive
				for (std::pair <int, int> point : component) {
					group_mark[point.first][point.second] = eyes_count >= 2;
				}
			}
		}

		for (int i = 0; i < row; ++i) {
			for (int j = 0; j < column; ++j) {
				if (group_mark[i][j] == 0) {
					currentState[i * column + j] = '.';
				}
			}
		}
	};

	removeDeadStones();

	std::string currentState = getState();
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
	for (std::string& placeMove : moveHistory) fout << placeMove << '\n';

	fout << score[0] << ' ' << score[1] << '\n';
	fout.close();

	return FileStatus::Success;
}

int Board::loadGame(MoveController& moveController) {
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
	std::vector <std::string> stack_list, stack_move;

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

		stack_move.assign(stackSize, "");

		for (int i = 0; i < stackSize; ++i) {
			if (!(fin >> stack_move[i]) || (int)stack_move[i].length() < 2 || (int)stack_move[i].length() > 3) {
				throw std::runtime_error("Missing or invalid move.");
			}
			
			if (stack_move[i] == "---") continue;

			std::pair <int, int> cellPos = cellPosGet(stack_move[i], row, column);
	
			if (cellPos.first < 0 || cellPos.first >= row || cellPos.second < 0 || cellPos.second >= column) {
				throw std::runtime_error("Invalid position.");
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

	state_pointer = stack_pointer;
	state_list = stack_list;
	moveLimit = stackLimit;
	moveHistory = stack_move;

	score = tempScore;

	current_turn = (state_pointer & 1);
	playing = score[0] == -1 && score[1] == -1;
	

	auto WriteFile = [&]() -> void {
		std::string pgn = "(;GM[1]FF[4]";

		pgn += "SZ[" + std::to_string(row) + "]";
		pgn += "KM[4.0]RU[Chinese]";

		for (int i = 1; i <= state_pointer; ++i) {
			pgn.push_back(';');

			if (moveHistory[i] == "---") {
				pgn += (i & 1) ? "B[]" : "W[]";
			}
			else {
				std::pair <int, int> cellPos = cellPosGet(moveHistory[i], row, column);
				
				pgn += (i & 1) ? "B[" : "W[";
				pgn.push_back(cellPos.second + 'a');
				pgn.push_back(cellPos.first + 'a');

				pgn.push_back(']');
			}
		}

		pgn.push_back(')');

		const std::string pgn_path = std::string(PROJECT_DIR) + "KataGo/data.sgf";
		std::ofstream fout(pgn_path);

		fout << pgn;
		
		fout.close();
	};

	WriteFile();

	std::thread([&]() {
		// Set Board Size (The Blocking Operation!)
		// Since we are on a background thread, blocking here is FINE.
		// It won't freeze the "Loading..." animation.
		auto minEndTime = std::chrono::steady_clock::now() + std::chrono::seconds(2);

		moveController.markAsLoading();

		moveController.setBoardSize(row);
		moveController.loadState();

		// Wait for the reminder 
		std::this_thread::sleep_until(minEndTime);

		// Signal Completion
		// You need to add a method to MoveController to trigger the "Ready" flag

		moveController.markAsReady();
	}).detach();

	return FileStatus::Success;
}

void Board::clearGame() {
	if (row == -1) return;

	state_list.clear();

	current_turn = 0;
	state_pointer = 0;
	playing = true;

	score = { -1, -1 };

	state_list.assign(1, std::string(row * column, '.'));
	state_list.back().push_back('0');
	moveHistory.assign(1, "---");
}

void Board::setWinByTime(int turn) {
	playing = false;

	score[turn] = 0;
	score[turn ^ 1] = -0x3f3f3f3f;
}