#ifndef OPTIONMENU_HPP
#define OPTIONMENU_HPP

#include <SFML/Graphics.hpp>
#include <Texture.hpp>

struct OptionMenu {
public:
	void init();
	void draw_volume_button(sf::RenderWindow& appwindow);
	void draw_back_button(sf::RenderWindow& appwindow);
	int tryClickingAt(sf::Vector2f mousepos);

	int getMusicVolume();
	int getSoundVolume();

private:
	sf::Font font;
	sf::Vector2f horizontal_offset, vertical_offset, gap;
	sf::Vector2f text_offset, content_offset, button_gap;
	sf::Color ui_color;

	sf::Vector2f volume_btn_size;

	int music_volume;
	int sound_volume;

	void setCenter(sf::Text& text);
};

#endif