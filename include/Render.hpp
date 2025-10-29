#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>
#include <Texture.hpp>

extern sf::RenderWindow appWindow;

namespace Window {
	bool init(int width, int height);

	void addObject(Texture _target, sf::Vector2f position, double opacity);
}

#endif