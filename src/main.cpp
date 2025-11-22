#include <SFML/Graphics.hpp>
#include <iostream>
#include <Helper.hpp>
#include <GameLogic.hpp>

sf::RenderWindow appWindow;



int main(int argv, char* args[]) {
	appWindow = sf::RenderWindow(sf::VideoMode(windowSize), "Go",
		sf::Style::Titlebar | sf::Style::Close);

	if (appWindow.isOpen() == false) {
		std::cerr << "Error creating window" << std::endl;
		return false;
	}

	appStart();
	while (appWindow.isOpen()) {
		appLoop();
	}

	return EXIT_SUCCESS;
}