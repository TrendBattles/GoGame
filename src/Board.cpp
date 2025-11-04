#include <Board.hpp>
#include <Helper.hpp>
#include <iostream>
#include <vector>


Board::Board() {
	PIECE_SCALE = sf::Vector2f(0.4, 0.4);
	offset = sf::Vector2f(40, 40);
	gapX = sf::Vector2f(65, 0);
	gapY = sf::Vector2f(0, 65);

	current_turn = 0;
	state_pointer = 0;
}

void Board::loadBoard(const std::string _str_source) {
	surface.load(_str_source);
}
void Board::loadPieces() {
	const std::filesystem::path CHESS_PIECE = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/go_pieces.png");
	
	for (int i = 0; i < 2; ++i) {
		pieces[i] = Texture(CHESS_PIECE.string(), sf::IntRect({ 100 * i, 0 }, { 100, 100 }));
		pieces[i].setScale(PIECE_SCALE);
		pieces[i].setCentered(true);
	}
}

void Board::setSize(int _i_row, int _i_column) {
	row = _i_row;
	column = _i_column;

	state_pointer = 0;
	state_list.assign(1, std::string(row * column, '.'));
}

std::pair <int, int> Board::getSize() {
	return std::make_pair(row, column);
}

void Board::drawBoard() {
	surface.draw();
}

void Board::drawState() {
	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < column; ++c) {
			if (emptyCell(r, c)) continue;

			int piece_type = getState(r, c);

			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			pieces[piece_type].draw(cur_pos);
		}
	}
}

void Board::tryPlacingAt(sf::Vector2f mouse_pos) {
	const float RADIUS = 25;

	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < column; ++c) {
			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) > RADIUS) continue;

			if (!possibleToPlace(r, c, current_turn)) continue;

			std::cout << r << " " << c << "\n";
			placePieceAt(r, c);

			pieces[current_turn].draw(cur_pos);
		}
	}
}

void Board::placePieceAt(int x, int y) {
	setState(x, y, current_turn);
	current_turn = 1 ^ current_turn;
}

void Board::drawShadow(sf::Vector2f mouse_pos) {
	const float RADIUS = 25;

	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < column; ++c) {
			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) > RADIUS) continue;

			if (!possibleToPlace(r, c, current_turn)) continue;

			pieces[current_turn].draw(cur_pos, 0.6);
			return;
		}
	}
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

void Board::setTurn(bool who) {
	current_turn = who;
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

bool Board::possibleToPlace(int x, int y, bool turn) {
	if (!emptyCell(x, y)) return false;
	

	/*
		When is a position possible to place a specific color ?
		
		1/If the connected component with the same color has any liberty:
			No captures or captures with NO KO rule violation.
		2/If the component has no liberties:
			Check if this move is a capture that doesn't violate KO rule or not.
	*/

	std::vector <std::pair <int, int>> captured = capturedPositions(x, y, turn);

	if (captured.empty()) {
		const int delta_x[4] = { -1, +1, 0, 0 };
		const int delta_y[4] = { 0, 0, -1, +1 };

		std::vector <std::pair <int, int>> component = findComponent(x, y, turn);
		int any_liberty = false;

		for (std::pair <int, int> point : component) {
			for (int iter = 0; iter < 4; ++iter) {
				any_liberty |= (int)emptyCell(point.first + delta_x[iter], point.second + delta_y[iter]);
			}
		}
		
		return any_liberty;
	}

	std::string current_state = state_list[state_pointer];
	current_state[x * column + y] = turn + '0';

	for (std::pair <int, int> point : captured) {
		current_state[point.first * column + point.second] = '.';
	}

	return state_pointer == 0 || current_state != state_list[state_pointer - 1];
}