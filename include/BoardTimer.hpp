#ifndef BOARDTIMER_HPP
#define BOARDTIMER_HPP

#include <SFML/Graphics.hpp>

class BoardTimer {
public:
	BoardTimer();

	void setTimeLimit(int id = 0);
	bool timeLimitToggle();

	void resetClock();
	void addTime(int turn = 0);
	sf::Time getTime(int turn = 0);
	bool subtractTime(int turn);

private:
	//Time limit parameters
	sf::Time timeRemaining[2], timeAdd, timeLimit;
	sf::Clock deltaClock;

	bool timeLimitSet;
};
#endif