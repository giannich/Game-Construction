
#include "CollisionHandler.hpp"
#include <iostream>
#include "Boat.hpp"

void BoatCollisionHandler::handleCollision(EType other) {
	std::cout << "Player " << parent->playerNum << " hit ";
	switch (other) {
		case SoulType:
			std::cout << "a Soul!" << std::endl;
			break;
		case WallType:
			std::cout << "the wall!" << std::endl;
			break;
		case BoatType:
			std::cout << "another player!" << std::endl;
			break;
		case FinishLineType:
			std::cout << "the Finish Line!" << std::endl;
			parent->disabled = true;
			break;
		default:
			break;
	}
}

void SoulCollisionHandler::handleCollision(EType other) {
	std::cout << "Delete Soul!" << std::endl;
}
