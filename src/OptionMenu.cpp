#include <OptionMenu.hpp>
#include <iostream>
#include <vector>

void OptionMenu::init() {

	const std::filesystem::path FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	font.openFromFile(FONT_PATH.c_str());

	horizontal_offset = sf::Vector2f(500, 0);
	vertical_offset = sf::Vector2f(0, 100);
	gap = sf::Vector2f(0, 80);
	ui_color = sf::Color::White;


	text_offset = sf::Vector2f(-150, 0);
	content_offset = sf::Vector2f(40, 0);
	volume_btn_size = sf::Vector2f(30, 20);
	button_gap = sf::Vector2f(35, 0);

	music_volume = 2;
	sound_volume = 5;
	autoSaveToggle = 0;
}

void OptionMenu::setCenter(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.size * 0.5f);;
}

void OptionMenu::draw_feature_button(sf::RenderWindow& appwindow) {
	// draw the text
	sf::Text music(font), sound(font), autoSave(font);
	music.setString("MUSIC"); sound.setString("SOUND");
	autoSave.setString("AUTOSAVE");

	//music.setCharacterSize(30); sound.setCharacterSize(30);
	music.setFillColor(ui_color); sound.setFillColor(ui_color);
	autoSave.setFillColor(ui_color);

	music.setPosition(horizontal_offset + vertical_offset + text_offset);
	sound.setPosition(horizontal_offset + vertical_offset + gap + text_offset);
	autoSave.setPosition(horizontal_offset + vertical_offset + gap * 2.0f + text_offset);

	setCenter(music); setCenter(sound);
	setCenter(autoSave);

	appwindow.draw(music);
	appwindow.draw(sound);
	appwindow.draw(autoSave);


	for (int r = 0; r <= 1; ++r) {
		int threshold;
		if (r == 0) threshold = music_volume;
		if (r == 1) threshold = sound_volume;
		for (int c = 1; c <= 5; ++c) {
			sf::RectangleShape rect(volume_btn_size);
			rect.setOrigin(volume_btn_size * 0.5f);

			if (c <= threshold) {
				rect.setFillColor(ui_color);
				rect.setOutlineColor(ui_color);
			}
			else {
				rect.setFillColor(sf::Color::Black);
				rect.setOutlineColor(sf::Color::Black);
			}

			sf::Vector2f cur_pos =
				vertical_offset + horizontal_offset + button_gap * float(c) + gap * float(r);
			rect.setPosition(cur_pos);

			appwindow.draw(rect);
		}
	}

	delete autoSaveAlert;
	autoSaveAlert = new sf::Text(font);
	
	autoSaveAlert->setString(autoSaveToggle ? "ON" : "OFF");
	autoSaveAlert->setPosition(vertical_offset + horizontal_offset + gap * 2.0f + button_gap * 2.5f);
	setCenter(*autoSaveAlert);

	appwindow.draw(*autoSaveAlert);
}


void OptionMenu::draw_back_button(sf::RenderWindow& appwindow) {
	// draw the text
	sf::Text back_button(font);
	back_button.setString("GO BACK");

	back_button.setCharacterSize(25);
	back_button.setFillColor(ui_color);
	back_button.setPosition(sf::Vector2f(20, 20));

	appwindow.draw(back_button);
}

int OptionMenu::tryClickingAt(sf::Vector2f mouse_pos) {
	for (int r = 0; r <= 1; ++r) {
		for (int c = 0; c <= 5; ++c) {
			sf::Vector2f cur_pos =
				vertical_offset + horizontal_offset + button_gap * float(c) + gap * float(r);
			sf::Vector2f diff = cur_pos - mouse_pos;

			if (abs(diff.x) <= volume_btn_size.x * 0.5f && 
				abs(diff.y) <= volume_btn_size.y * 0.5f) {
				switch (r) {
					case 0:
						music_volume = c;
						break;
					case 1:
						sound_volume = c;
						break;
				}
				return 0;
			}
		}
	}

	if (autoSaveAlert->getGlobalBounds().contains(mouse_pos)) {
		autoSaveToggle ^= 1;
		return -1;
	}

	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 40) 
		return 1;
	return -1;
}

int OptionMenu::getMusicVolume() { return music_volume; }
int OptionMenu::getSoundVolume() { return sound_volume; }
int OptionMenu::getSaveToggle() { return autoSaveToggle;  }