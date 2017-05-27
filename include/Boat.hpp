//
//  Boat.hpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#ifndef Boat_hpp
#define Boat_hpp

#include <stdio.h>
#include "SimpleAI.hpp"
#include "CollisionHandler.hpp"
#include "Box2D/Box2D.h"

class GameState;
class InputStream;

#define BOAT_MAX_SOULS 5

class Boat
{
	int currentSouls;
	float soulCollectionRadius;
	float forwardForce;
	float reverseForce;
	float turnRate;
	BoatCollisionHandler *collisionHandler;
	float dampingCoefficient();
public:
	bool disabled;
	int playerNum;
	float segPosition;
	InputStream* inputStream;
	b2Body *rigidBody;
	Boat(b2Vec2 , b2World&, SimpleAI *ai1, int pNum);
	void update(float deltaT, GameState &gs);
	float getX();
	float getY();
	float getRot();
	void addSoul();
};

#endif /* Boat_hpp */
