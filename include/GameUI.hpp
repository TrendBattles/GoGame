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

	void loadGame();
	void saveGame();

	void loadTurnIndicator();
	void annouceInGame(sf::RenderWindow& appWindow);

	void loadEndPopup();
	void annouceEndGame(sf::RenderWindow& appWindow);
private:
	//Initial textures
	sf::Font chinese_font, english_font;
	sf::Color ui_color;
	sf::Texture goboard;
	std::array <sf::Texture, 2> go_piece;

	void setCenter(sf::Text& text);

	//In-game parameters
	Board board;
	sf::Vector2f PIECE_SCALE;
	sf::Vector2f offset, board_offset;
	sf::Vector2f gapX, gapY;
	
	std::string fileNotification; //Save/Load notifications
	sf::Clock notificationTimer;
	sf::Time notificationDuration = sf::seconds(5.f); //Time limit for showing


	//Game function buttons
	std::vector <sf::Texture> functional_buttons;
	sf::Text* back_button;

	//End-game texture
	bool savedEndGame;
	Popup endPopup;
	
	//In-game Annoucement
	Popup messageBox;
};

#endif