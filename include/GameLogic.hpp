#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include <SFML/Graphics.hpp>


sf::Vector2f get_mouse_position();

void appStart();
int pollEvent();
void handle_menu();
void handle_option_menu();
void handle_about_menu();
void handle_game_scene();
void appLoop();

#endif