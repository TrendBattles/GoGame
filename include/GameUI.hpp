#ifndef GAMEUI_HPP
#define GAMEUI_HPP

#include <SFML/Graphics.hpp>
#include <Board.hpp>
#include <BoardTimer.hpp>
#include <PopUp.hpp>
#include <MoveController.hpp>


extern Board board;
extern BoardTimer Timer;
extern MoveController moveController;

struct GameConfig {
	GameConfig(int stateSize = 0);

	int piece_offset();
	float board_offset();
	float stoneRadius();

	int boardEdge = 576, borderLimit = 600;
	int gridSize;

	sf::Vector2f boardTopLeft;
	sf::Vector2f horizontal_offset, vertical_offset;

	void setBorderLimit(int x);
};

class GameUI {
public:
	void init();
	void initGame(int boardOption = 0);
	void resetGame();

	void drawLoadingScreen(sf::RenderWindow& appWindow);
	void draw_back_button(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos = sf::Vector2f(-1, -1));
	void draw_UI(sf::RenderWindow& appWindow);
	void draw_game_buttons(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos = sf::Vector2f(-1, -1));

	int tryClickingAt(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });
	void drawShadow(sf::RenderWindow& appWindow, sf::Vector2f mouse_pos = sf::Vector2f{ 0, 0 });

	bool loadGame();
	bool saveGame();

	void loadTurnIndicator();
	void annouceInGame(sf::RenderWindow& appWindow);

	void loadEndPopup();
	void annouceEndGame(sf::RenderWindow& appWindow);

	bool autoSave();
	bool autoLoad();

	void setAutoSaveToggle(int x = 0);
	int getAutoSaveToggle();

	bool loadTimer();
private:
	//Initial textures
	sf::Font chinese_font, english_font;
	sf::Color ui_color;
	std::array <sf::Texture, 2> go_piece;

	void setCenter(sf::Text& text);

	//In-game parameters
	GameConfig gameConfig;
	sf::Vector2f PIECE_SCALE;

	int autoSaveToggle;

	std::string fileNotification; //Save/Load notifications
	sf::Clock notificationTimer;
	sf::Time notificationDuration = sf::seconds(2.5f); //Time limit for showing

	//End-game texture
	bool savedEndGame, playedEndGameSound;
	Popup endPopup, newGamePopup, menuPopup;
	
	//In-game Annoucement
	Popup messageBox, blackSide, whiteSide;

	sf::Text createText(std::string message = "", bool centered = true, sf::Color textColor = sf::Color::White, int characterSize = 25);

	sf::FloatRect expandHitbox(sf::FloatRect bounds, float margin);
};

#endif