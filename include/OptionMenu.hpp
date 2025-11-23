#ifndef OPTIONMENU_HPP
#define OPTIONMENU_HPP

#include <SFML/Graphics.hpp>

class OptionMenu {
public:
	void init();
	void draw_feature_button(sf::RenderWindow& appwindow);
	void draw_selection_button(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos = sf::Vector2f(-1, -1));
	void draw_back_button(sf::RenderWindow& appwindow, sf::Vector2f mouse_pos = sf::Vector2f(-1, -1));
	int tryClickingAt(sf::Vector2f mouse_pos);

	void fixAudio(sf::Vector2f mouse_pos);

	int getMusicVolume();
	int getSoundVolume();

	int getSaveToggle();

	void loadConfig();
	void saveConfig();
	void initConfigFile();

	int getAttribute(std::string name);
private:
	sf::Font chinese_font, english_font;
	sf::Vector2f horizontal_offset, vertical_offset, gap, content_offset;
	sf::Vector2f text_offset, button_gap;

	sf::Vector2f volume_btn_size;

	int music_volume;
	int sound_volume;

	sf::Text* back_button;

	int autoSaveToggle;
	sf::Text* autoSaveAlert;

	void setCenter(sf::Text& text, int mask = 3);

	std::vector<std::string> selection_section;
	std::vector<int> option_chosen;
	std::vector<int> font_size;
	std::vector<std::vector<std::string>> selection_option;
	std::vector<sf::Text> buttons;

	void drawRadioButton(sf::RenderWindow& appWindow, double y, double a, double b, double c);
	sf::FloatRect expandHitbox(sf::FloatRect bounds, float margin);
};

#endif