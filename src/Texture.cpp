#include <Texture.hpp>
#include <SFML/Graphics.hpp>
#include <Render.hpp>

sf::Vector2i Texture::getSize() {
	return _ir_size;
}
sf::Texture Texture::getTexture() {
	return _tex;
}

Texture::Texture(std::string _str_source) {
	assert(load(_str_source));
}

bool Texture::load(std::string _str_source) {
	if (_tex.loadFromFile(_str_source) == false) {
		return false;
	}

	sf::Vector2u size = _tex.getSize();
	_ir_size = sf::Vector2i{ static_cast <int> (size.x), static_cast <int> (size.y) };
	return true;
}

void Texture::setSize(int _i_width, int _i_length) {
	_ir_size = sf::Vector2i{ _i_width, _i_length };
}

void Texture::draw() {
	Window::addObject(*this);
}