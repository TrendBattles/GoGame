#ifndef POPUP_HPP
#define POPUP_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <utility>

class Popup {
public:
	Popup(sf::Vector2f targetSize = sf::Vector2f(0, 0));

	void setSize(sf::Vector2f targetSize = sf::Vector2f(0, 0));
	sf::Vector2f getSize();

	void drawOn(sf::RenderWindow& appWindow);

	void addObject(std::string message = "", sf::Vector2f offsetPosition = sf::Vector2f(0, 0));
	bool clickedOn(sf::RenderWindow& appWindow, std::string message = "", sf::Vector2f mouse_pos = sf::Vector2f(0, 0));

	void clearCache();
private:
	sf::Font chinese_font, english_font;

	std::vector <sf::Text> textBoxList;
	sf::Vector2f size;
};
#endif