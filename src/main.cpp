#include <SFML/Graphics.hpp>
#include <Render.hpp>

void gameLoop();

int main(int argv, char* args[]) {
	if (Window::init() == false) {
		return EXIT_FAILURE;
	}

	while (appWindow.isOpen()) {
		gameLoop();
	}

	return EXIT_SUCCESS;
}