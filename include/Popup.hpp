#ifndef POPUP_HPP
#define POPUP_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <utility>

class Popup {
public:
	Popup();

	void setSize(sf::Vector2f targetSize = sf::Vector2f(0, 0));
	void setPosition(sf::Vector2f targetPosition = sf::Vector2f(0, 0));
	void setCornerRadius(float R = 0);

	sf::Vector2f getSize();
	sf::Vector2f getPosition();

	void drawBackground(sf::RenderWindow& appWindow);
	void drawOn(sf::RenderWindow& appWindow);

	void addObject(std::string message = "", sf::Vector2f offsetPosition = sf::Vector2f(0, 0), sf::Color textColor = sf::Color::White);
	void addObject(sf::Text builtMessage);
	void addObject(sf::Text builtMessage, sf::Vector2f offsetPosition);

	void setBackgroundColor(sf::Color bgColor = sf::Color::Transparent);
		
	bool clickedOn(std::string message = "", sf::Vector2f mouse_pos = sf::Vector2f(0, 0));
	bool clickedOn(sf::Vector2f mouse_pos = sf::Vector2f(0, 0));

	void clearCache();
private:
	sf::Font chinese_font, english_font;

	std::vector <sf::Text> textBoxList;
	
	float radius;
	sf::Vector2f size, position;
	sf::Color backgroundColor;
};
#endif