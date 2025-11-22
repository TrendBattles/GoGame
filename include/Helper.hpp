#ifndef HELPER_HPP
#define HELPER_HPP

#include <SFML/Graphics.hpp>

static sf::Vector2u windowSize = sf::Vector2u(1200, 800);
static const sf::Vector2u virtualWindowSize = sf::Vector2u(1200, 800);
static const sf::Color ui_color = sf::Color::White, accent_color = sf::Color::Black;

std::string convertTime(sf::Time askTime);
sf::Vector2f convertToFloat(sf::Vector2i cur);
sf::Vector2f convertToFloat(sf::Vector2u cur);

double sqr(double x);
double squareDist(sf::Vector2f a);
double dist(sf::Vector2f a);
double inverseDist(sf::Vector2f a);
double squareDist(sf::Vector2f a, sf::Vector2f b);
double dist(sf::Vector2f a, sf::Vector2f b);
double inverseDist(sf::Vector2f a, sf::Vector2f b);

#endif