#include <math.h>
#include <SFML/Graphics.hpp>

double sqr(double x) {
	return x * x;
}

sf::Vector2f convertToFloat(sf::Vector2i cur) {
	return sf::Vector2f((float)cur.x, (float)cur.y);
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