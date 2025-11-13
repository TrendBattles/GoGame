#include <SFML/Graphics.hpp>
#include <iostream>

sf::Vector2f get_mouse_position();
sf::RenderWindow appWindow;

void appStart();
int pollEvent();
void handle_menu();
void handle_option_menu();
void handle_about_menu();
void handle_game_scene();
void appLoop();

int main(int argv, char* args[]) {
	int width = 1000, height = 600;

	appWindow = sf::RenderWindow(sf::VideoMode(sf::Vector2u(width, height)), "Go",
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