#include <SFML/Graphics.hpp>
#include <Render.hpp>
#include <iostream>

sf::Vector2f get_mouse_position();

void load_texture();

void appStart();
int pollEvent();
void handle_menu();
void handle_option_menu();
void handle_about_menu();
void handle_game_scene();
void appLoop();

int main(int argv, char* args[]) {
	if (Window::init(1000, 600) == false) {
		return EXIT_FAILURE;
	}

	appStart();
	while (appWindow.isOpen()) {
		appLoop();
	}

	return EXIT_SUCCESS;
}