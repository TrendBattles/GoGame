#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>
#include <Texture.hpp>

extern sf::RenderWindow appWindow;

namespace Window {
	bool init();

	void addObject(Texture _target);
}

#endif