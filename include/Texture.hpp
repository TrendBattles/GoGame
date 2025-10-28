#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <SFML/Graphics.hpp>
#include <filesystem>

class Texture {
public:
	Texture() {}
	Texture(std::string _str_source);

	sf::Vector2i getSize();
	sf::Texture getTexture();

	bool load(std::string _str_source);

	void setSize(int _i_width, int _i_length);
	void draw();

private:
	sf::Texture _tex;
	sf::Vector2i _ir_size;
};

#endif