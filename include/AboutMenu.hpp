#ifndef ABOUTMENU_HPP
#define ABOUTMENU_HPP

#include <SFML/Graphics.hpp>

class AboutMenu {
public:
	void init();
	void draw_UI(sf::RenderWindow& appwindow);
	void draw_back_button(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos = sf::Vector2f(-1, -1));
	int tryClickingAt(sf::Vector2f mousepos);

private:
	sf::Font chinese_font, english_font;
	sf::Vector2f horizontal_offset, vertical_offset, gap;

	void setCenter(sf::Text& text);
};

#endif