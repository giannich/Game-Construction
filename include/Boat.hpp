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
#include "AI.hpp"
#include "CollisionHandler.hpp"
#include "Box2D/Box2D.h"

class GameState;
class InputStream;

#define BOAT_MAX_SOULS 5

class Boat
{
	float soulCollectionRadius;
	float reverseForce;
	float turnRate;
	BoatCollisionHandler *collisionHandler;
	float forwardForce();
public:
	bool pickingUpSoul;
	bool finishedRace;
	int currentSouls;
	bool disabled;
	int playerNum;
	float segPosition;
	InputStream* inputStream;
	b2Body *rigidBody;
	Boat(b2Vec2 , b2World&, AI *ai1, unsigned int);
	void update(float deltaT, GameState &gs);
	float getX();
	float getY();
	float getRot();
	float getSpeed();
	void addSoul();
};

class LocalBoat: public Boat {
public:
	LocalBoat(b2Vec2 initPos, b2World& m_world, AI *ai1, unsigned int pNum, std::vector <std::pair<in_addr, int>> *broadcastPointer, GameState *gstate) : 
	Boat(initPos, m_world, ai1, pNum)
	{
		inputStream = new LocalPlayerInputStream(playerNum, broadcastPointer, gstate);
	}
};

class NetworkBoat: public Boat {
public:
	NetworkBoat(b2Vec2 initPos, b2World& m_world, AI *ai1, unsigned int pNum, std::vector <std::pair<in_addr, int>> *broadcastPointer, bool isBroadcasting, GameState *gstate) : 
	Boat(initPos, m_world, ai1, pNum)
	{
		inputStream = new NetworkPlayerInputStream(playerNum, broadcastPointer, isBroadcasting, gstate);
	}
};

class AIBoat: public Boat {
public:
	AIBoat(b2Vec2 initPos, b2World& m_world, AI *ai1, unsigned int pNum, std::vector <std::pair<in_addr, int>> *broadcastPointer, GameState *gstate) : 
	Boat(initPos, m_world, ai1, pNum)
	{
		inputStream = new AIInputStream(playerNum, ai1, broadcastPointer, gstate);
	}
};

#endif /* Boat_hpp */
