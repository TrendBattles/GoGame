#include <SFML/Graphics.hpp>
#include <Render.hpp>

void appLoop();

int main(int argv, char* args[]) {
	if (Window::init() == false) {
		return EXIT_FAILURE;
	}

	while (appWindow.isOpen()) {
		appLoop();
	}

	return EXIT_SUCCESS;
}