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
	vertical_offset = sf::Vector2f(0, 150);
	gap = sf::Vector2f(0, 40);
	ui_color = sf::Color::White;
}

void RulesMenu::setCenter(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.size * 0.5f);;
}

void RulesMenu::draw_UI(sf::RenderWindow& appwindow) {
	std::vector<std::string> lines;
	lines.push_back("Go is a strategy game on a grid.");
	lines.push_back("Players place Black (go first) and White stones to surround territory.");
	lines.push_back("Stones must have an adjacent empty point (liberty).");
	lines.push_back("A stone/group with no liberties is captured (removed).");
	lines.push_back("Repeating a board position is illegal (Ko rule).");
	lines.push_back("The player with the most stone on the board win.");
	lines.push_back("White will be given a 8 points lead to compensate.");
	lines.push_back("----------");
	lines.push_back("You can change board size. Currently, 9x9, 13x13 and 19x19 is supported.");
	lines.push_back("----------");
	lines.push_back("You can put move limit. When move limit is exceeded, the game ends.");
	lines.push_back("The winner will be calculate by the above rule.");
	lines.push_back("----------");
	lines.push_back("You can put time limit. Whoever runs out of time first loses.");



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


void RulesMenu::draw_back_button(sf::RenderWindow& appwindow) {
	// draw the text
	sf::Text back_button(chinese_font);
	back_button.setString("GO BACK");

	back_button.setCharacterSize(25);
	back_button.setFillColor(ui_color);
	back_button.setPosition(sf::Vector2f(20, 20));

	appwindow.draw(back_button);

}

int RulesMenu::tryClickingAt(sf::Vector2f mouse_pos) {

	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 40)
		return 1;
	return -1;
}