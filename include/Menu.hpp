#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <Texture.hpp>

struct Menu {
public:
	void init();
	void draw_game_text(sf::RenderWindow &appwindow);
	void draw_button(sf::RenderWindow& appwindow);

private:
	sf::Font font;
	sf::Vector2f horizontal_offset;
	sf::Color ui_color;
	sf::Vector2f text_offset;
	sf::Vector2f btn_size;
	sf::Vector2f btn_gap, btn_offset;
	std::vector<std::string> btn_list;
};

#endif