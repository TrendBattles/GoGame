#include <SFML/Graphics.hpp>
#include <Render.hpp>
#include <iostream>

void appStart();
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