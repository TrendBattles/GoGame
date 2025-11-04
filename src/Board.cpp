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
	single_cell_capture = std::make_pair(-1, -1);
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

	state = std::vector <std::string>(row, std::string(column, '.'));
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

void Board::placeAt(sf::Vector2f mouse_pos) {
	const float RADIUS = 25;

	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < column; ++c) {
			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) > RADIUS) continue;

			if (!possibleToPlace(r, c, current_turn)) continue;

			std::cout << r << " " << c << "\n";
			setState(r, c, current_turn);

			current_turn = 1 ^ current_turn;
		}
	}
}

void Board::drawShadow(sf::Vector2f mouse_pos) {
	const float RADIUS = 25;

	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < column; ++c) {
			sf::Vector2f cur_pos = offset + gapX * float(c) + gapY * float(r);
			if (dist(cur_pos, mouse_pos) > RADIUS) continue;

			if (!possibleToPlace(r, c, current_turn)) continue;

			pieces[current_turn].draw(cur_pos, 0.6);
		}
	}
}

void Board::setState(int x, int y, int c) {
	if (x < 0 || x >= row || y < 0 || y >= column) {
		std::cerr << "Setting a null cell (" << x << "," << y << ")\n";
		return;
	}
	
	state[x][y] = c == -1 ? '.' : c + '0';
}


//Only checks valid cells on the board
int Board::getState(int x, int y) {
	if (outsideBoard(x, y)) return -1;

	return state[x][y] == '.' ? -1 : state[x][y] - '0';
}

bool Board::emptyCell(int x, int y) {
	if (outsideBoard(x, y)) return false;

	return state[x][y] == '.';
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

bool Board::KORule(int x, int y, bool turn) {
	/*
		To check if we place the piece for the target turn in (x, y), it violates KO rule.
		Note that (x, y) has to be empty.
	*/
	if (single_cell_capture == std::make_pair(-1, -1)) return false;

	if (!emptyCell(x, y)) {
		std::cerr << "Error: (" << x << "," << y << ") is not empty to check KO rule\n";
		return false;
	}

	const int delta_x[4] = { +1, -1, 0, 0 };
	const int delta_y[4] = { 0, 0, +1, -1 };

	for (int iter = 0; iter < 4; ++iter) {
		int adj_x = single_cell_capture.first + delta_x[iter];
		int adj_y = single_cell_capture.second + delta_y[iter];


		if (emptyCell(adj_x, adj_y) && x != adj_x && y != adj_y) return true;
	}

	return false;
}

bool Board::existCapture(int x, int y, bool turn) {
	/*
		If we place (x, y) in the target turn, will there be captures ?
	*/
	if (!emptyCell(x, y)) {
		std::cerr << "Error: (" << x << "," << y << ") is not empty to check existing captures \n";
		return false;
	}
	
	const int delta_x[4] = { +1, -1, 0, 0 };
	const int delta_y[4] = { 0, 0, +1, -1 };

	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < column; ++j) {
			if (getState(i, j) != (1 ^ turn)) continue;
				
			/*
				Liberty -> No capture
			*/

			int liberty = false;

			for (int iter = 0; iter < 4; ++iter) {
				int new_i = i + delta_x[iter];
				int new_j = j + delta_y[iter];

				if (outsideBoard(new_i, new_j)) continue;

				if (!emptyCell(new_i, new_j)) continue;
				if (new_i == x && new_j == y) continue;

				liberty = true;
				break;
			}

			if (!liberty) return true;
		}
	}

	return false;
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


	std::vector <std::pair <int, int>> _component = std::move(findComponent(x, y, turn));
	const int delta_x[4] = { +1, -1, 0, 0 };
	const int delta_y[4] = { 0, 0, +1, -1 };

	int any_liberty = false;
	for (std::pair <int, int> point : _component) {
		for (int iter = 0; iter < 4; ++iter) {
			any_liberty |= (int) emptyCell(point.first + delta_x[iter], point.second + delta_y[iter]);
		}

		if (any_liberty) break;
	}

	if (!any_liberty) {
		return !KORule(x, y, turn) && existCapture(x, y, turn);
	}


	return !existCapture(x, y, turn) || !KORule(x, y, turn);
}