#include <RulesMenu.hpp>
#include <iostream>
#include <vector>
#include <Helper.hpp>

void RulesMenu::init() {
	const std::filesystem::path CHINESE_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	chinese_font.openFromFile(CHINESE_FONT_PATH.c_str());
	const std::filesystem::path ENGLISH_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/english.otf");
	english_font.openFromFile(ENGLISH_FONT_PATH.c_str());

	horizontal_offset = sf::Vector2f(virtualWindowSize.x * 0.5f, 0);
	vertical_offset = sf::Vector2f(0, 100);
	gap = sf::Vector2f(0, 35);
}

void RulesMenu::setCenter(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.size * 0.5f);
}

void RulesMenu::draw_UI(sf::RenderWindow& appwindow) {
	sf::Text menu_title(chinese_font);
	menu_title.setString("RULES");
	menu_title.setCharacterSize(50);
	menu_title.setFillColor(ui_color);
	menu_title.setPosition(horizontal_offset + vertical_offset * 0.5f);
	setCenter(menu_title);
	appwindow.draw(menu_title);

	std::vector<std::string> lines;
	lines.push_back("Go is a strategy game on a grid.");
	lines.push_back("Players place Black (go first) and White stones to surround territory.");
	lines.push_back("Stones must have at least an adjacent empty cell (liberty).");
	lines.push_back("A stone/group with no liberties is captured (removed).");
	lines.push_back("Repeating a board position is illegal (Ko rule).");
	lines.push_back("The player with the most points on the board wins.");
	lines.push_back("White will be given an 8-point lead to compensate.");
	lines.push_back("==========");
	lines.push_back("Points = (Number of your stones on board) + (Number of prisoners) + (Territory area).");
	lines.push_back("Note that you can't see your current points during the game.");
	lines.push_back("==========");
	lines.push_back("You can change board size. Currently, 9x9, 13x13 and 19x19 are supported.");
	lines.push_back("==========");
	lines.push_back("You can put move limit. When exceeding the limit, the game ends.");
	lines.push_back("The winner will be decided by the above rule.");
	lines.push_back("==========");
	lines.push_back("You can put time limit. Whoever runs out of time first loses. Passing doesn't add time.");
	lines.push_back("==========");
	lines.push_back("Saving, loading, undoing and redoing moves are disabled in time limit.");

	for (int i = 0; i < (int)lines.size(); ++i) {
		sf::Text text(english_font);
		text.setString(lines[i]);
		text.setCharacterSize(25);
		text.setFillColor(ui_color);
		text.setPosition(horizontal_offset + vertical_offset + gap * float(i));
		setCenter(text);
		appwindow.draw(text);
	}



}


void RulesMenu::draw_back_button(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos) {
	// draw the text
	sf::Text back_button(chinese_font);
	back_button.setString("Menu");

	back_button.setCharacterSize(25);
	back_button.setFillColor(ui_color);
	back_button.setPosition(sf::Vector2f(20, 20));


	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 80) {
		back_button.setFillColor(accent_color);
	}

	appwindow.draw(back_button);


	sf::Text game_button(chinese_font);
	game_button.setString("Play!");

	game_button.setCharacterSize(25);
	game_button.setFillColor(ui_color);
	game_button.setPosition(sf::Vector2f(1110, 20));

	mouse_pos.x = virtualWindowSize.x - mouse_pos.x;
	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 80) {
		game_button.setFillColor(accent_color);
	}

	appwindow.draw(game_button);
}

int RulesMenu::tryClickingAt(sf::Vector2f mouse_pos) {
	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 80)
		return 1;
	mouse_pos.x = virtualWindowSize.x - mouse_pos.x;
	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 80)
		return 2;
	return -1;
}