#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <SFML/Graphics.hpp>
#include <filesystem>

class Texture {
public:
	Texture() {}
	Texture(std::string _str_source, sf::IntRect range = sf::IntRect({ 0, 0 }, { 0, 0 }));
	bool load(std::string _str_source = "", sf::IntRect range = sf::IntRect({ 0, 0 }, { 0, 0 }));

	sf::Vector2i getSize();
	sf::Texture getTexture();
	sf::Vector2f getScale();

	void setSize(int _i_width, int _i_length);
	void draw(sf::Vector2f pos = sf::Vector2f(0, 0));
	void setScale(sf::Vector2f scale);

	bool getCentered();
	void setCentered(bool centered);

private:
	sf::Texture _tex;
	sf::Vector2i _ir_size;
	sf::Vector2f scale = sf::Vector2f(1, 1);
	bool is_centered = false;
};

#endif