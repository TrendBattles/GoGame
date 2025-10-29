#include <Texture.hpp>
#include <SFML/Graphics.hpp>
#include <Render.hpp>
#include <iostream>

sf::Vector2i Texture::getSize() {
	return _ir_size;
}
sf::Texture Texture::getTexture() {
	return _tex;
}

Texture::Texture(std::string _str_source, sf::IntRect range) {
	load(_str_source, range);
}

bool Texture::load(std::string _str_source, sf::IntRect range) {
	if (range.size.x == 0) {
		if (_tex.loadFromFile(_str_source) == false) {
			return false;
		}
	}
	else {
		if (_tex.loadFromFile(_str_source, false, range) == false) {
			return false;
		}
	}
	sf::Vector2u size = _tex.getSize();
	_ir_size = sf::Vector2i{ static_cast <int> (size.x), static_cast <int> (size.y) };
	return true;
}

void Texture::setSize(int _i_width, int _i_length) {
	_ir_size = sf::Vector2i{ _i_width, _i_length };
}

void Texture::setScale(sf::Vector2f _scale) {
	scale = _scale;
}

sf::Vector2f Texture::getScale() {
	return scale;
}

void Texture::draw(sf::Vector2f position) {
	Window::addObject(*this, position);
}

bool Texture::getCentered() {
	return is_centered;
}

void Texture::setCentered(bool centered) {
	is_centered = centered;
}
