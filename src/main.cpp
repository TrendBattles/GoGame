#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>

const std::string project_dir = PROJECT_DIR;

int main() {

	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({ 600, 600 }), "Go");

	if (window.isOpen() == false) {
		std::cerr << "Error creating window" << std::endl;
		return EXIT_FAILURE;
	}

	const std::string board_path = project_dir + "\\assets\\board\\board.png";

	sf::Texture texture(board_path);
	sf::Sprite sprite(texture, sf::IntRect({ 0, 0 }, { 600, 600 }));

	while (window.isOpen()) {
		window.clear(sf::Color::White);

		while (const std::optional event = window.pollEvent()) {
			if (event->is <sf::Event::Closed>()) {
				std::cerr << "Closing the window" << std::endl;
				window.close(); 
				
				break;
			}
		} 


		window.draw(sprite);

		window.display();
	}
	return EXIT_SUCCESS;
}