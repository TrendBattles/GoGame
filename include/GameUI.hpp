#ifndef GAMEUI_HPP
#define GAMEUI_HPP

#include <SFML/Graphics.hpp>
#include <Board.hpp>
#include <Texture.hpp>

struct GameUI {
public:
	void init();
	void draw_back_button(sf::RenderWindow& appwindow);
	void draw_UI(sf::RenderWindow& appwindow);

	int tryClickingAt(sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });
	void drawShadow(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });

private:
	sf::Font chinese_font, english_font;
	sf::Color ui_color;
	sf::Texture chessboard;
	std::vector<sf::Texture> chess_piece;

	void setCenter(sf::Text& text);

	Board board;


	sf::Vector2f PIECE_SCALE;
	sf::Vector2f offset, board_offset;
	sf::Vector2f gapX, gapY;
};

#endif