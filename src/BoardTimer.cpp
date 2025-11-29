#include <BoardTimer.hpp>

BoardTimer::BoardTimer() = default;

void BoardTimer::setTimeLimit(int id) {
	timeLimitSet = id > 0;

	switch (id) {
	case 0:
		timeLimit = sf::seconds(0.0f);
		timeAdd = sf::seconds(0.0f);
		break;
	case 1:
		timeLimit = sf::seconds(60.0f);
		timeAdd = sf::seconds(3.0f);
		break;
	case 2:
		timeLimit = sf::seconds(180.0f);
		timeAdd = sf::seconds(5.0f);
		break;
	case 3:
		timeLimit = sf::seconds(600.0f);
		timeAdd = sf::seconds(10.0f);
		break;
	case 4:
		timeLimit = sf::seconds(1800.0f);
		timeAdd = sf::seconds(20.0f);
		break;
	}
}

bool BoardTimer::timeLimitToggle() {
	return timeLimitSet;
}

//Reset the game with time limit
void BoardTimer::resetClock() {
	if (!timeLimitSet) return;

	timeRemaining[0] = timeRemaining[1] = timeLimit;
	
	deltaClock.restart();
}
//Add time
void BoardTimer::addTime(int turn) {
	if (!timeLimitSet) return;

	timeRemaining[turn] += timeAdd;
}

sf::Time BoardTimer::getTime(int turn) {
	return timeRemaining[turn];
}

//Running clock
bool BoardTimer::subtractTime(int turn) {
	sf::Time timePassed = deltaClock.restart();

	if (timeRemaining[turn] > timePassed) {
		timeRemaining[turn] -= timePassed;
		return true;
	}

	timeRemaining[turn] = sf::seconds(0.0f);
	return false;
}