#include <Board.hpp>
#include <Helper.hpp>
#include <iostream>
#include <vector>
#include <fstream>


Board::Board() {
	current_turn = 0;
	state_pointer = 0;
	playing = true;

	score = { -1, -1 };
}



void Board::setSize(int _i_row, int _i_column) {
	row = _i_row;
	column = _i_column;

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

	std::cerr << "Player " << (getTurn() + 1) << "'s turn\n";
	std::cerr << (getPassState() ? "Already passed\n" : "No pass\n");
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
	state_list[++state_pointer][x * column + y] = c == -1 ? '.' : c + '0';

	for (std::pair <int, int> point : captured) {
		state_list[state_pointer][point.first * column + point.second] = '.';
	}

	state_list[state_pointer].back() = '0';

	saveGame();
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

std::vector <std::pair <int, int>> Board::findComponent(int x, int y, int cell_id) {
	/*
		This will return a connected component containing (x, y) that is marked <turn>
		Note that (x, y) is included regardless its state.
	*/

	std::vector <std::pair <int, int>> _cell_queue;
	std::vector <std::vector <int>> visited(row, std::vector <int>(column));

	visited[x][y] = true;
	_cell_queue.emplace_back(x, y);

	const int delta_x[4] = { +1, -1, 0, 0 };
	const int delta_y[4] = { 0, 0, +1, -1 };

	for (int order = 0; order < (int)_cell_queue.size(); ++order) {
		int i, j; std::tie(i, j) = _cell_queue[order];

		for (int iter = 0; iter < 4; ++iter) {
			if (getState(i + delta_x[iter], j + delta_y[iter]) != cell_id) continue;

			int new_i = i + delta_x[iter];
			int new_j = j + delta_y[iter];

			if (visited[new_i][new_j]) continue;

			visited[new_i][new_j] = true;
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

		saveGame();

		std::cerr << "Player " << (getTurn() + 1) << "'s turn\n";
		std::cerr << (getPassState() ? "Already passed\n" : "No pass\n");
		return true;
	}
	return false;
}
bool Board::redo() {
	if (state_pointer + 1 < (int)state_list.size()) {
		current_turn = 1 ^ current_turn;
		++state_pointer;

		saveGame();

		std::cerr << "Player " << (getTurn() + 1) << "'s turn\n";
		std::cerr << (getPassState() ? "Already passed\n" : "No pass\n");
		return true;
	}
	return false;
}

bool Board::pass() {
	//End-scoring?
	if (getPassState()) {
		std::cerr << "Ending the game\n";
		playing = false;

		getScore();
		saveGame();

		return true;
	}

	state_list.erase(state_list.begin() + state_pointer + 1, state_list.end());
	state_list.push_back(state_list.back());
	state_list.back().back() = '1';
	state_pointer = (int) state_list.size() - 1;

	saveGame();

	current_turn = 1 ^ current_turn;

	std::cerr << "Player " << (getTurn() + 1) << "'s turn\n";
	std::cerr << (getPassState() ? "Already passed\n" : "No pass\n");
	return false;
}

void Board::resign() { 
	std::cerr << "Player " << (current_turn ^ 1) + 1 << " won by resignation\n";
	playing = false; 
	
	score[current_turn ^ 1] = 0x3f3f3f3f;
	score[current_turn] = 0;

	saveGame();
}

std::array <int, 2> Board::getScore() {
	if (score[0] != -1 && score[1] != -1) return score;

	/*
		Final score by both sides
		If +inf -> won by resignation
	*/
	score = { 0, 0 };
	std::string finalState = getState();
	
	for (int i = 0; i < row * column; ++i) {
		if (finalState[i] != '.') score[finalState[i] - '0'] += 1;
	}
	

	/*std::vector <std::vector <int>> visited(row, std::vector <int>(column));*/
	
	/*
		Score = (number of pieces on board) + (empty_cells covered)
		(later I guess)
	*/

	/*
		- Remove all dead pieces
		- Then, remove the all pieces that are surrounded by opponent pieces and adjacent to less than two eyes.
	*/

	/*auto checkDeadState = [&] (std::pair <int, int> source) -> void {
		int cell_id = finalState[source.first * column + source.second] - '0';

		std::vector <std::pair <int, int>> component;
		visited[source.first][source.second] = true;

		component.emplace_back(source);

		int delta_x[4] = { -1, +1, 0, 0 };
		int delta_y[4] = { 0, 0, -1, +1 };

		for (int p = 0; p < (int)component.size(); ++p) {
			int x, y; std::tie(x, y) = component[p];

			for (int dir = 0; dir < 4; ++dir) {
				int new_x = x + delta_x[dir], new_y = y + delta_y[dir];

				if (outsideBoard(new_x, new_y) || visited[new_x][new_y]) continue;

				if (finalState[new_x * column + new_y] - '0' == cell_id) {
					visited[new_x][new_y] = true;
					component.emplace_back(new_x, new_y);
				}
			}
		}

		int any_liberty = false;
		for (std::pair <int, int> P : component) {
			for (int dir = 0; dir < 4; ++dir) {
				int adj_x = P.first + delta_x[dir], adj_y = P.second + delta_y[dir];

				if (outsideBoard(adj_x, adj_y)) continue;

				any_liberty |= finalState[adj_x * column + adj_y] == '.';
			}
		}

		if (any_liberty) return;
		for (std::pair <int, int> P : component) {
			finalState[P.first * column + P.second] = '.';
		}
	};

	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < column; ++j) {
			if (finalState[i * column + j] == '.') continue;

			if (visited[i][j]) continue;

			checkDeadState(std::make_pair(i, j));
		}
	}*/



	return score;
}

void Board::saveGame() {
	const std::string fileName = std::string(PROJECT_DIR) + "assets/gopgn.txt";
	std::ofstream fout(fileName);

	/*
		Saving games for future uses
	*/

	if (!fout.is_open()) {
		std::cerr << "Error: Can't save game\n";
		return;
	}
		
	/*
		stack size - pointer
		state_list
	*/
	fout << (int)state_list.size() << ' ' << state_pointer << '\n';
	for (std::string st : state_list) fout << st << '\n';

	fout << score[0] << ' ' << score[1] << '\n';

	fout.close();
	//std::cerr << "Saved successfully\n";
}

void Board::loadGame() {
	const std::string fileName = std::string(PROJECT_DIR) + "assets/gopgn.txt";
	std::ifstream fin(fileName);

	/*
		Loading games
	*/

	system("cls");

	if (!fin.is_open() || fin.eof()) {
		std::cerr << "Error: Can't load game\n";
		return;
	}

	std::cerr << "Loaded successfully\n";
	
	int stackSize; fin >> stackSize >> state_pointer;
	state_list.assign(stackSize, "");

	for (int i = 0; i < stackSize; ++i) {
		fin >> state_list[i];
	}
	fin >> score[0] >> score[1];

	current_turn = stackSize % 2;

	if (score[0] == -1 || score[1] == -1) {
		std::cerr << "Player " << (getTurn() + 1) << "'s turn\n";
		std::cerr << (getPassState() ? "Already passed\n" : "No pass\n");

		return;
	}

	if (score[0] == 0x3f3f3f3f) {
		std::cout << "Player 1 won by resignation\n";
	}
	else if (score[1] == 0x3f3f3f3f) {
		std::cout << "Player 2 won by resignation\n";
	}
	else {
		std::cout << "Player 1: " << score[0] << '\n';
		std::cout << "Player 2: " << score[1] << '\n';

		if (score[0] == score[1]) {
			std::cout << "Draw\n";
		}
		else {
			std::cout << "Player " << (score[0] < score[1]) + 1 << " wins\n";
		}
	}
}