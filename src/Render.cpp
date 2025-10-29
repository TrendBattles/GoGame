#include <Render.hpp>
#include <iostream>
sf::RenderWindow appWindow;

bool Window::init() {
	appWindow = sf::RenderWindow(sf::VideoMode({ 600, 600 }), "Go");

	if (appWindow.isOpen() == false) {
		std::cerr << "Error creating window" << std::endl;
		return false;
	}

	return true;
}

void Window::addObject(Texture _target) {
	sf::Texture _tex_get = _target.getTexture();

	sf::Sprite obj(_tex_get);
	appWindow.draw(obj);
}