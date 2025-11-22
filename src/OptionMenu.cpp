#include <OptionMenu.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <Helper.hpp>


const std::filesystem::path CACHE_FILE =
std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/options.cache");

void OptionMenu::init() {
	const std::filesystem::path CHINESE_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	chinese_font.openFromFile(CHINESE_FONT_PATH.c_str());


	const std::filesystem::path ENGLISH_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/english.otf");
	english_font.openFromFile(ENGLISH_FONT_PATH.c_str());

	horizontal_offset = sf::Vector2f(virtualWindowSize.x * 0.5f, 0);
	vertical_offset = sf::Vector2f(0, 200);
	gap = sf::Vector2f(0, 80);


	text_offset = sf::Vector2f(-300, 0);
	volume_btn_size = sf::Vector2f(25, 25);
	button_gap = sf::Vector2f(2, 0);
	content_offset = sf::Vector2f(40, 0);

	selection_section.clear();
	selection_section.push_back("BOARD SIZE");
	selection_section.push_back("MUSIC THEME");
	selection_section.push_back("MOVE LIMIT");
	selection_section.push_back("TIME LIMIT");

	option_chosen.resize(selection_section.size());
	selection_option.resize(selection_section.size());

	//Board size
	selection_option[0].push_back("9 x 9");
	selection_option[0].push_back("13 x 13");
	selection_option[0].push_back("19 x 19");

	//Theme
	selection_option[1].push_back("Chill");
	selection_option[1].push_back("Futuristic");
	selection_option[1].push_back("Ancient");

	selection_option[2].push_back("None");
	for (int i = 1; i <= 5; ++i) {
		selection_option[2].push_back(std::to_string(50 * i));
	}

	selection_option[3].push_back("None");
	selection_option[3].push_back("1 min + 3s");
	selection_option[3].push_back("3 min + 5s");
	selection_option[3].push_back("10 min + 10s");
	selection_option[3].push_back("30 min + 20s");
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
		if (fin.eof()) {
			fin.close();
			initConfigFile();
			loadConfig();
			return;
		}

		int x = 0; fin >> x;
		sta.push_back(x);
	}

	int volume1 = sta[0], volume2 = sta[1], auto_save = sta[2];
	if (std::min(volume1, volume2) < 0 || std::max(volume1, volume2) > 100 || auto_save < 0 || auto_save > 1) {
		fin.close();
		initConfigFile();
		loadConfig();
		return;
	}

	music_volume = volume1;
	sound_volume = volume2;
	autoSaveToggle = auto_save;

	for (int i = 0; i < (int)selection_section.size(); ++i) {
		if (fin.eof()) {
			fin.close();
			initConfigFile();
			loadConfig();
			return;
		}

		int x; fin >> x;
		if (x < 0 || x >= selection_option[i].size()) {
			fin.close();
			initConfigFile();
			loadConfig();
			return;
		}

		option_chosen[i] = x;
	}

	fin.close();

	int timeLimitPos = std::find(selection_section.begin(), selection_section.end(), "TIME LIMIT") - selection_section.begin();

	//Time limit turned on -> No autosave, no move limits
	if (selection_option[timeLimitPos][option_chosen[timeLimitPos]] != "None") {
		autoSaveToggle = 0;

		int moveLimitPos = std::find(selection_section.begin(), selection_section.end(), "MOVE LIMIT") - selection_section.begin();
		option_chosen[moveLimitPos] = 0;

		saveConfig();
	}
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
	fout << 40 << " " << 100 << " " << 0 << "\n";
	for (int i = 0; i < (int)option_chosen.size(); ++i) {
		fout << 0 << " ";
	}
	fout << "\n";
	fout.close();
}

void OptionMenu::setCenter(sf::Text& text, int mask) {
	sf::FloatRect bounds = text.getLocalBounds();
	if ((mask & 1) == 0) bounds.size.x = 0;
	if (mask <= 1) bounds.size.y = 0;
	text.setOrigin(bounds.size * 0.5f);;
}

void OptionMenu::drawRadioButton(sf::RenderWindow& appWindow, double y, double a, double b, double c) {

	const sf::Color beige(252, 229, 205), brown(40, 40, 40), bruh(89, 89, 89);

	sf::Vector2f zero_percent = sf::Vector2f(a, y);
	sf::Vector2f hundred_percent = sf::Vector2f(c, y);
	sf::Vector2f current_percent = sf::Vector2f(b, y);

	sf::CircleShape cyka0(volume_btn_size.x * 0.5f), cyka100(volume_btn_size.x * 0.5f);
	cyka0.setPosition(zero_percent); cyka100.setPosition(hundred_percent);
	cyka0.setOrigin(volume_btn_size * 0.5f); cyka100.setOrigin(volume_btn_size * 0.5f);
	cyka0.setFillColor(beige); cyka100.setFillColor(brown);

	sf::RectangleShape filled(sf::Vector2f(current_percent.x - zero_percent.x, volume_btn_size.y)),
		unfilled(sf::Vector2f(hundred_percent.x - current_percent.x, volume_btn_size.y));
	filled.setOrigin(sf::Vector2f(0, volume_btn_size.y * 0.5f));
	unfilled.setOrigin(sf::Vector2f(0, volume_btn_size.y * 0.5f));
	filled.setPosition(zero_percent); unfilled.setPosition(current_percent);
	filled.setFillColor(beige); unfilled.setFillColor(brown);

	sf::CircleShape chode(volume_btn_size.x * 0.8f);
	chode.setPosition(current_percent);
	chode.setOrigin(volume_btn_size * 0.8f);
	chode.setFillColor(ui_color);
	chode.setOutlineColor(bruh);
	chode.setOutlineThickness(5.0f);


	appWindow.draw(cyka0); appWindow.draw(cyka100);
	appWindow.draw(filled); appWindow.draw(unfilled);
	appWindow.draw(chode);
}

void OptionMenu::draw_feature_button(sf::RenderWindow& appwindow) {
	sf::Text menu_title(chinese_font);
	menu_title.setString("OPTIONS");
	menu_title.setCharacterSize(60);
	menu_title.setFillColor(ui_color);
	menu_title.setPosition(horizontal_offset + vertical_offset * 0.5f);
	setCenter(menu_title);
	appwindow.draw(menu_title);

	// draw the text
	sf::Text music(chinese_font), sound(chinese_font), autoSave(chinese_font);
	music.setString("MUSIC VOLUME"); sound.setString("SOUND VOLUME");
	autoSave.setString("AUTOSAVE");

	//music.setCharacterSize(30); sound.setCharacterSize(30);
	music.setFillColor(ui_color); sound.setFillColor(ui_color);
	autoSave.setFillColor(ui_color);

	music.setPosition(horizontal_offset + vertical_offset + text_offset);
	sound.setPosition(horizontal_offset + vertical_offset + gap + text_offset);
	autoSave.setPosition(horizontal_offset + vertical_offset + gap * 2.0f + text_offset);

	setCenter(music, 2); setCenter(sound, 2);
	setCenter(autoSave, 2);

	appwindow.draw(music);
	appwindow.draw(sound);
	appwindow.draw(autoSave);

	const sf::Color beige(252, 229, 205), brown(40, 40, 40), bruh(89, 89, 89);
	for (int r = 0; r <= 1; ++r) {
		int threshold;
		if (r == 0) threshold = music_volume;
		if (r == 1) threshold = sound_volume;

		sf::Vector2f zero_percent = 
		vertical_offset + horizontal_offset + gap * float(r) + content_offset;
		sf::Vector2f hundred_percent =
			vertical_offset + horizontal_offset + button_gap * float(100) + gap * float(r) + content_offset;
		sf::Vector2f current_percent =
			vertical_offset + horizontal_offset + button_gap * float(threshold) + gap * float(r) + content_offset;

		drawRadioButton(appwindow, zero_percent.y, zero_percent.x, current_percent.x, hundred_percent.x);
	}


	delete autoSaveAlert;
	autoSaveAlert = new sf::Text(chinese_font);
	
	autoSaveAlert->setString("OFF");
	autoSaveAlert->setPosition(vertical_offset + horizontal_offset + gap * 2.0f + content_offset);
	setCenter(*autoSaveAlert, 2);


	sf::Vector2f zero_percent =
		vertical_offset + horizontal_offset + gap * 2.0f + content_offset;
	sf::Vector2f hundred_percent =
		vertical_offset + horizontal_offset + gap * 2.0f + content_offset + button_gap * float(20);
	sf::Vector2f current_percent =
		vertical_offset + horizontal_offset + gap * 2.0f + content_offset + button_gap * float(20 * autoSaveToggle);

	drawRadioButton(appwindow, zero_percent.y, zero_percent.x, current_percent.x, hundred_percent.x);
}


void OptionMenu::draw_selection_button(sf::RenderWindow& appwindow) {
	// draw buttons

	buttons.clear();
	for (int i = 0; i < (int)selection_section.size(); ++i) {
		sf::Text vcl(chinese_font);
		vcl.setString(selection_section[i]);
		vcl.setFillColor(ui_color);
		vcl.setPosition(horizontal_offset + vertical_offset + gap * float(3 + i) + text_offset);
		setCenter(vcl, 2);
		appwindow.draw(vcl);

		sf::Text current_option(english_font);
		current_option.setString(selection_option[i][option_chosen[i]]);
		current_option.setFillColor(ui_color);
		current_option.setPosition(horizontal_offset + vertical_offset + gap * float(3 + i) + content_offset);
		setCenter(current_option, 2);

		appwindow.draw(current_option);

		buttons.emplace_back(current_option);
	}
}


void OptionMenu::draw_back_button(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos) {
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

int OptionMenu::tryClickingAt(sf::Vector2f mouse_pos) {
	for (int r = 0; r <= 1; ++r) {
		sf::Vector2f fifty_percent =
			vertical_offset + horizontal_offset + button_gap * float(50) + gap * float(r) + content_offset;
		sf::Vector2f sz(button_gap.x * float(70), volume_btn_size.x);
		if (abs(mouse_pos.x - fifty_percent.x) <= sz.x && abs(mouse_pos.y - fifty_percent.y) <= sz.y) {
			double chiu_bo = horizontal_offset.x + content_offset.x;
			chiu_bo = (mouse_pos.x - chiu_bo) / button_gap.x;
			if (chiu_bo < 0) chiu_bo = 0;
			if (chiu_bo > 100) chiu_bo = 100;

			int ans = round(chiu_bo / 10) * 10;
			switch (r) {
				case 0:
					music_volume = ans;
					break;
				case 1:
					sound_volume = ans;
					break;
			}
		}
	}


	sf::Vector2f fifty_percent =
		vertical_offset + horizontal_offset + gap * 2.0f + content_offset + button_gap * float(10);
	sf::Vector2f sz =
		sf::Vector2f(button_gap.x * float(30), volume_btn_size.x);
	if (abs(mouse_pos.x - fifty_percent.x) <= sz.x && abs(mouse_pos.y - fifty_percent.y) <= sz.y) {
		int timeLimitPos = std::find(selection_section.begin(), selection_section.end(), "TIME LIMIT") - selection_section.begin();

		//Time limit turned on -> No autosave
		if (selection_option[timeLimitPos][option_chosen[timeLimitPos]] == "None") {
			autoSaveToggle ^= 1;
			saveConfig();
		}

		return -1;
	}

	for (int i = 0; i < (int)selection_section.size(); ++i) {
		if (buttons[i].getGlobalBounds().contains(mouse_pos)) {
			int sz = selection_option[i].size();
			//Rotating choices
			option_chosen[i] = (option_chosen[i] + 1) % sz;

			//Time limit turned on -> No autosave
			if (selection_section[i] == "TIME LIMIT" && selection_option[i][option_chosen[i]] != "None") {
				autoSaveToggle = 0;
			}

			saveConfig();
		}
	}
	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 80) {
		return 1;
	}
	mouse_pos.x = virtualWindowSize.x - mouse_pos.x;
	if (mouse_pos.x >= 0 && mouse_pos.x <= 150 && mouse_pos.y >= 0 && mouse_pos.y <= 80)
		return 2;
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

