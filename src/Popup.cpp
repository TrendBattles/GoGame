#include <Popup.hpp>
#include <Helper.hpp>

//Initialization 
Popup::Popup() {
	const std::filesystem::path CHINESE_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/chinese.otf");
	chinese_font.openFromFile(CHINESE_FONT_PATH.c_str());
	const std::filesystem::path ENGLISH_FONT_PATH =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Font/english.otf");
	english_font.openFromFile(ENGLISH_FONT_PATH.c_str());
	
	background.setFillColor(sf::Color(48, 48, 48, 200));
	/*background.setOutlineThickness(2.f);
	background.setOutlineColor(sf::Color(100, 100, 100, 160));*/
}

void Popup::setSize(sf::Vector2f targetSize) {
	background.setSize(targetSize);
}

void Popup::setPosition(sf::Vector2f targetPosition) {
	background.setPosition(targetPosition);
}

sf::Vector2f Popup::getSize() {
	return background.getSize();
}

sf::Vector2f Popup::getPosition() {
	return background.getPosition();
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
	appWindow.draw(background);

	for (sf::Text messageBox : textBoxList) {
		messageBox.setPosition(messageBox.getPosition() + background.getPosition());
		appWindow.draw(messageBox);
	}
}

//Check if a text clicked on?
bool Popup::clickedOn(std::string message, sf::Vector2f mouse_pos) {
	sf::Vector2f popupTopLeft = background.getPosition();

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