#include <OptionMenu.hpp>
#include <iostream>
#include <vector>
#include <fstream>


const std::filesystem::path CACHE_FILE =
std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/options.cache");

void OptionMenu::init() {
	const std::filesystem::path CHINESE_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	chinese_font.openFromFile(CHINESE_FONT_PATH.c_str());


	const std::filesystem::path ENGLISH_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/english.otf");
	english_font.openFromFile(ENGLISH_FONT_PATH.c_str());

	horizontal_offset = sf::Vector2f(500, 0);
	vertical_offset = sf::Vector2f(0, 100);
	gap = sf::Vector2f(0, 80);
	ui_color = sf::Color::White;


	text_offset = sf::Vector2f(-150, 0);
	content_offset = sf::Vector2f(40, 0);
	volume_btn_size = sf::Vector2f(30, 20);
	button_gap = sf::Vector2f(35, 0);

	selection_section.clear();
	selection_section.push_back("BOARD SIZE");
	selection_section.push_back("MUSIC THEME");

	option_chosen.resize(selection_section.size());
	selection_option.resize(selection_section.size());
	
	selection_option[0].push_back("9 x 9");
	selection_option[0].push_back("13 x 13");
	selection_option[0].push_back("19 x 19");

	selection_option[1].push_back("Chill");
	selection_option[1].push_back("Futuristic");
	selection_option[1].push_back("Ancient");;

	loadConfig();
}

void OptionMenu::loadConfig() {
	std::ifstream fin(CACHE_FILE.c_str());

	if (!fin.is_open() || fin.eof()) {
		fin.close();
		initConfigFile();
		loadConfig();
		return;
	}

	std::vector<int> sta;
	for (int _ = 0; _ < 3; ++_) {
		int x = 0; fin >> x;
		if (fin.eof()) {
			fin.close();
			initConfigFile();
			loadConfig();
			return;
		}
		sta.push_back(x);
	}

	int volume1 = sta[0], volume2 = sta[1], auto_save = sta[2];
	music_volume = volume1;
	sound_volume = volume2;
	autoSaveToggle = auto_save;

	std::cerr << "still working normally" << std::endl;

	for (int i = 0; i < (int)selection_section.size(); ++i) {
		int x; fin >> x;
		if (fin.eof() || x >= selection_option[i].size()) {
			std::cerr << "tryna reading files failed" << std::endl;
			fin.close();
			initConfigFile();
			loadConfig();
			return;
		}
		option_chosen[i] = x;
	}

	fin.close();
}

void OptionMenu::saveConfig() {
	std::ofstream fout(CACHE_FILE.c_str());
	fout << music_volume << " " << sound_volume << " " << autoSaveToggle << "\n";
	for (int i = 0; i < (int)option_chosen.size(); ++i) {
		fout << option_chosen[i] << " ";
	}
	fout << "\n";
	
	fout.close();
}

void OptionMenu::initConfigFile() {
	std::ofstream fout(CACHE_FILE.c_str());
	fout << 2 << " " << 5 << " " << 0 << "\n";
	for (int i = 0; i < (int)option_chosen.size(); ++i) {
		fout << 0 << " ";
	}
	fout << "\n";
	fout.close();
}

void OptionMenu::setCenter(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.size * 0.5f);;
}

void OptionMenu::draw_feature_button(sf::RenderWindow& appwindow) {
	// draw the text
	sf::Text music(chinese_font), sound(chinese_font), autoSave(chinese_font);
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
	autoSaveAlert = new sf::Text(chinese_font);
	
	autoSaveAlert->setString(autoSaveToggle ? "ON" : "OFF");
	autoSaveAlert->setPosition(vertical_offset + horizontal_offset + gap * 2.0f + button_gap * 2.5f);
	setCenter(*autoSaveAlert);

	appwindow.draw(*autoSaveAlert);
}


void OptionMenu::draw_selection_button(sf::RenderWindow& appwindow) {
	// draw the text

	buttons.clear();
	for (int i = 0; i < (int)selection_section.size(); ++i) {
		sf::Text vcl(chinese_font);
		vcl.setString(selection_section[i]);
		vcl.setFillColor(ui_color);
		vcl.setPosition(horizontal_offset + vertical_offset + gap * float(3 + i) + text_offset);
		setCenter(vcl);
		appwindow.draw(vcl);

		sf::Text current_option(english_font);
		current_option.setString(selection_option[i][option_chosen[i]]);
		current_option.setFillColor(ui_color);
		current_option.setPosition(horizontal_offset + vertical_offset + gap * float(3 + i) + button_gap * 2.5f);
		setCenter(current_option);

		appwindow.draw(current_option);

		buttons.emplace_back(current_option);
	}
}

void OptionMenu::draw_back_button(sf::RenderWindow& appwindow) {
	// draw the text
	sf::Text back_button(chinese_font);
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
				saveConfig();
				return 0;
			}
		}
	}

	if (autoSaveAlert->getGlobalBounds().contains(mouse_pos)) {
		autoSaveToggle ^= 1;
		saveConfig();
		return -1;
	}

	for (int i = 0; i < (int)selection_section.size(); ++i) {
		if (buttons[i].getGlobalBounds().contains(mouse_pos)) {
			int sz = selection_option[i].size();
			option_chosen[i] = (option_chosen[i] + 1) % sz;
			saveConfig();
		}
	}

	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 40) 
		return 1;
	return -1;
}


int OptionMenu::getAttribute(std::string name) {
	for (int i = 0; i < (int)selection_section.size(); ++i) {
		if (selection_section[i] == name)
			return option_chosen[i];
	}
	return -1;
}

int OptionMenu::getMusicVolume() { return music_volume; }
int OptionMenu::getSoundVolume() { return sound_volume; }
int OptionMenu::getSaveToggle() { return autoSaveToggle;  }

