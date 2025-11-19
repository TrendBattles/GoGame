#ifndef GAMEUI_HPP
#define GAMEUI_HPP

#include <SFML/Graphics.hpp>
#include <Board.hpp>
#include <PopUp.hpp>

struct GameUI {
public:
	void init();
	void initGame();
	void resetGame();

	void draw_back_button(sf::RenderWindow& appWindow);
	void draw_UI(sf::RenderWindow& appWindow);
	void draw_game_buttons(sf::RenderWindow& appWindow);

	int tryClickingAt(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });
	void drawShadow(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });

	//void annouceInGame(sf::RenderWindow& appWindow);
	void loadEndPopup();
	void annouceEndGame(sf::RenderWindow& appWindow);
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

	//End game texture
	bool savedEndGame;
	Popup endPopup;
	
	////Game Annoucement
	//Popup messageBox;
};

#endif