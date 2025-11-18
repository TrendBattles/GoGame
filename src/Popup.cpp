#include <Popup.hpp>
#include <Helper.hpp>


//Initialization 
Popup::Popup(sf::Vector2f targetSize) {
	const std::filesystem::path CHINESE_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	chinese_font.openFromFile(CHINESE_FONT_PATH.c_str());
	const std::filesystem::path ENGLISH_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/english.otf");
	english_font.openFromFile(ENGLISH_FONT_PATH.c_str());

	setSize(targetSize);
}

void Popup::setSize(sf::Vector2f targetSize) {
	size = targetSize;
}

sf::Vector2f Popup::getSize() {
	return size;
}

//Add text into popup
void Popup::addObject(std::string message, sf::Vector2f offsetPosition) {
	sf::Text messageBox(english_font);
	messageBox.setString(message);
	messageBox.setPosition(offsetPosition);
	//messageBox.setCharacterSize(25);

	textBoxList.emplace_back(messageBox);
}

//Display
void Popup::drawOn(sf::RenderWindow& appWindow) {
	sf::RectangleShape PopupBackground(size);
	PopupBackground.setPosition((convertToFloat(appWindow.getSize()) - size) * 0.5f);
	PopupBackground.setFillColor(sf::Color(48, 48, 48, 200));
	PopupBackground.setOutlineThickness(2.f);
	PopupBackground.setOutlineColor(sf::Color(100, 100, 100, 160));

	appWindow.draw(PopupBackground);

	for (sf::Text messageBox : textBoxList) {
		messageBox.setPosition(messageBox.getPosition() + PopupBackground.getPosition());
		appWindow.draw(messageBox);
	}
}

//Check if a text clicked on?
bool Popup::clickedOn(sf::RenderWindow& appWindow, std::string message, sf::Vector2f mouse_pos) {
	sf::Vector2f popupTopLeft = (convertToFloat(appWindow.getSize()) - size) * 0.5f;
	for (sf::Text messageBox : textBoxList) {
		messageBox.setPosition(messageBox.getPosition() + popupTopLeft);

		if (messageBox.getString() != message) continue;
		if (messageBox.getGlobalBounds().contains(mouse_pos)) {
			return true;
		}
	}

	return false;
}


//clear popup content
void Popup::clearCache() {
	textBoxList.clear();
	textBoxList.shrink_to_fit();
}