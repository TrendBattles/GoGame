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
	
	backgroundColor = sf::Color(70, 63, 56, 150);
	radius = 0;
}

void Popup::setSize(sf::Vector2f targetSize) {
	size = targetSize;
}

void Popup::setPosition(sf::Vector2f targetPosition) {
	position = targetPosition;
}

void Popup::setCornerRadius(float R) {
	radius = R;
}

sf::Vector2f Popup::getSize() {
	return size;
}

sf::Vector2f Popup::getPosition() {
	return position;
}

//Add text into popup
void Popup::addObject(std::string message, sf::Vector2f offsetPosition, sf::Color textColor) {
	sf::Text messageBox(english_font);
	messageBox.setString(message);
	messageBox.setPosition(offsetPosition);
	messageBox.setCharacterSize(25);

	messageBox.setFillColor(textColor);

	textBoxList.emplace_back(messageBox);
}

void Popup::addObject(sf::Text builtMessage) {
	textBoxList.emplace_back(builtMessage);
}

void Popup::addObject(sf::Text builtMessage, sf::Vector2f offsetPosition) {
	builtMessage.setPosition(offsetPosition);
	textBoxList.emplace_back(builtMessage);
}


void Popup::setBackgroundColor(sf::Color bgColor) {
	backgroundColor = bgColor;
}

//Display
void Popup::drawBackground(sf::RenderWindow& appWindow) {
	sf::RenderTexture textureBuffer;
	
	if (!textureBuffer.resize(convertToUInt(size))) {
		return;
	}

	textureBuffer.clear(sf::Color::Transparent);

	//Drawing a background with round corners (if set)
	//Save into a render texture first

	//Alpha placeholder
	int alpha = backgroundColor.a;
	backgroundColor.a = 255;

	sf::RectangleShape horizontal_rect, vertical_rect;
	horizontal_rect.setFillColor(backgroundColor);
	vertical_rect.setFillColor(backgroundColor);

	horizontal_rect.setSize(sf::Vector2f(size.x, size.y - 2 * radius));
	horizontal_rect.setPosition(sf::Vector2f(0, radius));

	vertical_rect.setSize(sf::Vector2f(size.x - 2 * radius, size.y));
	vertical_rect.setPosition(sf::Vector2f(radius, 0));

	textureBuffer.draw(horizontal_rect);
	textureBuffer.draw(vertical_rect);

	for (float x : {radius, size.x - radius}) {
		for (float y : {radius, size.y - radius}) {
			sf::CircleShape overlapCircle(radius);
			overlapCircle.setPosition(sf::Vector2f(x, y));
			overlapCircle.setOrigin(overlapCircle.getGlobalBounds().size * 0.5f);
			
			overlapCircle.setFillColor(backgroundColor);

			textureBuffer.draw(overlapCircle);
		}
	}
	textureBuffer.display();

	//Adding texture to a sprite
	sf::Sprite finalPopup(textureBuffer.getTexture());
	
	//No more holders
	backgroundColor.a = alpha;
	finalPopup.setColor(backgroundColor);
	finalPopup.setPosition(position);
	
	appWindow.draw(finalPopup);
}
void Popup::drawOn(sf::RenderWindow& appWindow) {
	drawBackground(appWindow);

	for (sf::Text messageBox : textBoxList) {
		messageBox.setPosition(messageBox.getPosition() + position);
		appWindow.draw(messageBox);
	}
}

//Check if a text clicked on?
bool Popup::clickedOn(std::string message, sf::Vector2f mouse_pos) {
	for (sf::Text messageBox : textBoxList) {
		messageBox.setPosition(messageBox.getPosition() + position);

		if (messageBox.getString() != message) continue;
		if (messageBox.getGlobalBounds().contains(mouse_pos)) {
			return true;
		}
	}

	return false;
}

//Check if this popup is clicked on (including the background)
bool Popup::clickedOn(sf::Vector2f mouse_pos) {
	return mouse_pos.x >= position.x && mouse_pos.x <= position.x + size.x && mouse_pos.y >= position.y && mouse_pos.y <= position.y + size.y;
}


//clear popup content
void Popup::clearCache() {
	textBoxList.clear();
	textBoxList.shrink_to_fit();
}