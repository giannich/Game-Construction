
#include "CollisionHandler.hpp"
#include <iostream>
#include "Boat.hpp"
#include "Soul.hpp"

void BoatCollisionHandler::handleCollision(EType other) {
	std::cout << "Player " << parent->playerNum << " hit ";
	switch (other) {
		case SoulType:
			std::cout << "a Soul!" << std::endl;
			parent->addSoul();
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
	switch(other) {
		case BoatType: 
			parent->collected = true;
			std::cout << "Delete Soul!" << parent->collected << std::endl;
			break;
		default:
			std::cout << "Soul hit a " << other << std::endl;
			break;
	}
}

void FinishLineCollisionHandler::handleCollision(EType other) { }
