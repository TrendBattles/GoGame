#include <AboutMenu.hpp>
#include <iostream>
#include <vector>
#include <Helper.hpp>

void AboutMenu::init() {
	const std::filesystem::path CHINESE_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	chinese_font.openFromFile(CHINESE_FONT_PATH.c_str());
	const std::filesystem::path ENGLISH_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/english.otf");
	english_font.openFromFile(ENGLISH_FONT_PATH.c_str());

	horizontal_offset = sf::Vector2f(virtualWindowSize.x * 0.5f, 0);
	vertical_offset = sf::Vector2f(0, 180);
	gap = sf::Vector2f(0, 40);
	ui_color = sf::Color::White;
}

void AboutMenu::setCenter(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.size * 0.5f);;
}

void AboutMenu::draw_UI(sf::RenderWindow& appwindow) {
	sf::Text menu_title(chinese_font);
	menu_title.setString("ABOUT US");
	menu_title.setCharacterSize(60);
	menu_title.setFillColor(ui_color);
	menu_title.setPosition(horizontal_offset + vertical_offset * 0.5f);
	setCenter(menu_title);

	appwindow.draw(menu_title);

	std::vector<std::string> lines;
	lines.push_back("This project was made by Le Kien Thanh and Nguyen Nguyen Gia Bao.");
	lines.push_back("Our nicknames are steveonalex and trendbattles");
	lines.push_back("This was created on 12th of November.");
	lines.push_back("It's just a random college project.");
	lines.push_back("We procrastinated this like crazy. Life is tough");
	lines.push_back("At least we managed to push this out, cool I guess.");
	lines.push_back("DEV LOGS:");
	lines.push_back("1 month before deadlines: adding chessboard");
	lines.push_back("1 week before deadlines: adding the entire menu system");
	lines.push_back("3 days before deadlines: adding the option system");
	lines.push_back("2 days before deadlines: redesigning the entire project");


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

void AboutMenu::draw_back_button(sf::RenderWindow& appwindow) {
	// draw the text
	sf::Text back_button(chinese_font);
	back_button.setString("GO BACK");

	back_button.setCharacterSize(25);
	back_button.setFillColor(ui_color);
	back_button.setPosition(sf::Vector2f(20, 20));

	appwindow.draw(back_button);
}

int AboutMenu::tryClickingAt(sf::Vector2f mouse_pos) {

	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 40)
		return 1;
	return -1;
}