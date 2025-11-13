#ifndef ABOUTMENU_HPP
#define ABOUTMENU_HPP

#include <SFML/Graphics.hpp>

class AboutMenu {
public:
	void init();
	void draw_UI(sf::RenderWindow& appwindow);
	void draw_back_button(sf::RenderWindow& appwindow);
	int tryClickingAt(sf::Vector2f mousepos);

private:
	sf::Font chinese_font, english_font;
	sf::Vector2f horizontal_offset, vertical_offset, gap;
	sf::Color ui_color;

	void setCenter(sf::Text& text);
};

#endif