#include <Menu.hpp>
#include <iostream>
#include <vector>
#include <Helper.hpp>

void Menu::init() {

	const std::filesystem::path FONT_PATH = 
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	font.openFromFile(FONT_PATH.c_str());

	horizontal_offset = sf::Vector2f(virtualWindowSize.x * 0.5f, 0);
	text_offset = horizontal_offset + sf::Vector2f(0, 100);
	ui_color = sf::Color::White;
	accent_color = sf::Color::Black;


	btn_offset = horizontal_offset + sf::Vector2f(0, 300);
	btn_size = sf::Vector2f(200, 80);
	btn_gap = sf::Vector2f(0, 80);
	
	btn_list.push_back("PLAY");
	btn_list.push_back("OPTION");
	btn_list.push_back("RULES");
	btn_list.push_back("ABOUT");
	btn_list.push_back("QUIT");
}

void Menu::draw_game_text(sf::RenderWindow &appwindow) {
	sf::Text cur_text(font);
	cur_text.setString("GO GAME");
	cur_text.setCharacterSize(80);
	cur_text.setFillColor(ui_color);
	cur_text.setPosition(text_offset);


	sf::FloatRect bounds = cur_text.getLocalBounds();
	cur_text.setOrigin(bounds.size * 0.5f);

	appwindow.draw(cur_text);
}


void Menu::draw_button(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos) {
	for (int i = 0; i < (int)btn_list.size(); ++i) {
		sf::Vector2f cur_pos = btn_offset + btn_gap * float(i);
		
		sf::RectangleShape rect(btn_size);
		rect.setPosition(cur_pos);
		rect.setOrigin(btn_size * 0.5f);
		rect.setFillColor(sf::Color::Transparent);

		rect.setOutlineColor(ui_color);
		rect.setOutlineThickness(5.f);



		sf::Text cur_text(font);
		cur_text.setString(btn_list[i]);
		cur_text.setCharacterSize(40);
		cur_text.setFillColor(ui_color);
		cur_text.setPosition(cur_pos);

		sf::FloatRect bounds = cur_text.getLocalBounds();
		cur_text.setOrigin(bounds.size * 0.5f);


		if (abs(cur_pos.x - mouse_pos.x) <= btn_size.x * 0.5
			&& abs(cur_pos.y - mouse_pos.y) <= btn_size.y * 0.5) {
			cur_text.setFillColor(accent_color);
		}

		//appwindow.draw(rect);
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