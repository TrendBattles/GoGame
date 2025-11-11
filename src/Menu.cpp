#include <Menu.hpp>
#include <iostream>
#include <vector>

void Menu::init() {

	const std::filesystem::path FONT_PATH = 
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/chinese.otf");
	font.openFromFile(FONT_PATH.c_str());

	horizontal_offset = sf::Vector2f(500, 0);
	text_offset = horizontal_offset + sf::Vector2f(0, 100);
	ui_color = sf::Color::White;


	btn_offset = horizontal_offset + sf::Vector2f(0, 250);
	btn_size = sf::Vector2f(200, 80);
	btn_gap = sf::Vector2f(0, 100);
	
	btn_list.push_back("PLAY");
	btn_list.push_back("OPTION");
	btn_list.push_back("ABOUT");
}

void Menu::draw_game_text(sf::RenderWindow &appwindow) {
	sf::Text cur_text(font);
	cur_text.setString("GO");
	cur_text.setCharacterSize(60);
	cur_text.setFillColor(ui_color);
	cur_text.setPosition(text_offset);


	sf::FloatRect bounds = cur_text.getLocalBounds();
	cur_text.setOrigin(bounds.size * 0.5f);

	appwindow.draw(cur_text);
}


void Menu::draw_button(sf::RenderWindow& appwindow) {
	for (int i = 0; i < (int)btn_list.size(); ++i) {
		sf::RectangleShape rect(btn_size);
		rect.setPosition(btn_offset + btn_gap * float(i));
		rect.setOrigin(btn_size * 0.5f);
		rect.setFillColor(sf::Color::Transparent);

		rect.setOutlineColor(ui_color);
		rect.setOutlineThickness(5.f);

		appwindow.draw(rect);


		sf::Text cur_text(font);
		cur_text.setString(btn_list[i]);
		cur_text.setCharacterSize(40);
		cur_text.setFillColor(ui_color);
		cur_text.setPosition(btn_offset + btn_gap * float(i));

		sf::FloatRect bounds = cur_text.getLocalBounds();
		cur_text.setOrigin(bounds.size * 0.5f);

		appwindow.draw(cur_text);
	}
}

int Menu::tryClickingAt(sf::Vector2f mouse_pos) {
	for (int i = 0; i < (int)btn_list.size(); ++i) {
		sf::Vector2f cur_pos = btn_offset + btn_gap * float(i);
		if (abs(cur_pos.x - mouse_pos.x) <= btn_size.x * 0.5
			&& abs(cur_pos.y - mouse_pos.y) <= btn_size.y * 0.5)
			return i;
	}
	return -1;
}