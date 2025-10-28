#include <Render.hpp>
#include <iostream>
#include <filesystem>

void gameLoop() {
	const std::filesystem::path board_path = std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/board/board.png");

	Texture _tex_board(board_path.string());

	appWindow.clear(sf::Color::White);

	while (const std::optional event = appWindow.pollEvent()) {
		if (event->is <sf::Event::Closed>()) {
			std::cerr << "Closing the window" << std::endl;
			appWindow.close();

			return;
		}
	}

	_tex_board.draw();

	appWindow.display();
}