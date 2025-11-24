#include <math.h>
#include <SFML/Graphics.hpp>
#include <filesystem>

double sqr(double x) {
	return x * x;
}

//Converting time to print on the screen
std::string convertTime(sf::Time askTime) {
	if (askTime <= sf::seconds(0.0f))
		return "0.0";
	if (askTime >= sf::seconds(60.0f)) {
		int seconds = std::ceil(askTime.asSeconds());
		int minutes = seconds / 60;
		seconds %= 60;

		return std::to_string(minutes) + ":" + std::to_string(seconds / 10) + std::to_string(seconds % 10);
	}

	int milliseconds = askTime.asMilliseconds();
	milliseconds = (milliseconds + 50) / 100;

	int seconds = milliseconds / 10;
	milliseconds %= 10;
	
	if (seconds >= 10) return std::to_string(seconds / 10) + std::to_string(seconds % 10) + "." + std::to_string(milliseconds);
	return std::to_string(seconds) + "." + std::to_string(milliseconds);
}

sf::Vector2f convertToFloat(sf::Vector2i cur) {
	return sf::Vector2f((float)cur.x, (float)cur.y);
}
sf::Vector2f convertToFloat(sf::Vector2u cur) {
	return sf::Vector2f((float)cur.x, (float)cur.y);
}

sf::Vector2u convertToUInt(sf::Vector2f cur) {
	return sf::Vector2u(cur.x, cur.y);
}
sf::Vector2i convertToInt(sf::Vector2f cur) {
	return sf::Vector2i(cur.x, cur.y);
}

double squareDist(sf::Vector2f a) {
	return sqr(a.x) + sqr(a.y);
}
double dist(sf::Vector2f a) {
	return sqrt(squareDist(a));
}
double inverseDist(sf::Vector2f a) {
	return 1 / dist(a);
}

double squareDist(sf::Vector2f a, sf::Vector2f b) {
	return squareDist(a - b);
}

double dist(sf::Vector2f a, sf::Vector2f b) {
	return dist(a - b);
}

double inverseDist(sf::Vector2f a, sf::Vector2f b) {
	return inverseDist(a - b);
}