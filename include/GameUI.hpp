#ifndef GAMEUI_HPP
#define GAMEUI_HPP

#include <SFML/Graphics.hpp>
#include <Board.hpp>

struct GameUI {
public:
	void init();
	void resetGame();

	void draw_back_button(sf::RenderWindow& appwindow);
	void draw_UI(sf::RenderWindow& appwindow);
	void draw_game_buttons(sf::RenderWindow& appWindow);

	int tryClickingAt(sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });
	void drawShadow(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });

	bool annouceEndGame();
private:
	sf::Font chinese_font, english_font;
	sf::Color ui_color;
	sf::Texture goboard;
	std::array <sf::Texture, 2> go_piece;
	std::vector <sf::Texture> functional_buttons;

	void setCenter(sf::Text& text);

	Board board;

	sf::Vector2f PIECE_SCALE;
	sf::Vector2f offset, board_offset;
	sf::Vector2f gapX, gapY;
};

#endif