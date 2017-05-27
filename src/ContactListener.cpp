#include "ContactListener.hpp"
#include "CollisionHandler.hpp"
#include <iostream>

void ContactListener::BeginContact(b2Contact* contact)
{
	CollisionHandler *dataA = (CollisionHandler*)contact->GetFixtureA()->GetBody()->GetUserData();
	CollisionHandler *dataB = (CollisionHandler*)contact->GetFixtureB()->GetBody()->GetUserData();
	if(dataA != nullptr) {
		dataA->handleCollision(dataB == nullptr ? WallType : dataB->type);
	}
	if(dataB != nullptr) {
		dataB->handleCollision(dataA == nullptr ? WallType : dataA->type);
	}
}

