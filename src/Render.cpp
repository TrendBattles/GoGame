#include <Render.hpp>
#include <iostream>
sf::RenderWindow appWindow;

bool Window::init(int width, int height) {
	appWindow = sf::RenderWindow(sf::VideoMode(sf::Vector2u(width, height)), "Go",
		sf::Style::Titlebar | sf::Style::Close);

	if (appWindow.isOpen() == false) {
		std::cerr << "Error creating window" << std::endl;
		return false;
	}

	return true;
}

void Window::addObject(Texture _target, sf::Vector2f position, double opacity) {
	sf::Texture _tex_get = _target.getTexture();
	sf::Vector2f sprite_scale = _target.getScale();
	bool centered = _target.getCentered();

	sf::Sprite obj(_tex_get);
	obj.setPosition(position);
	obj.setScale(sprite_scale);
	
	sf::Color color = obj.getColor();
	color.a = round(color.a * opacity);
	obj.setColor(color);
	if (centered) {
		sf::Vector2i size = _target.getSize();
		obj.setOrigin(sf::Vector2f(0.5 * size.x, 0.5 * size.y));
	}

	appWindow.draw(obj);
}