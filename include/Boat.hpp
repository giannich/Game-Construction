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

#include "Box2D/Box2D.h"

class GameState;
class InputStream;

#define BOAT_MAX_SOULS 5

class Boat
{
	float soulCollectionRadius;
	float forwardForce;
	float reverseForce;
	//float turningImpulse;
	float turnRate;
	//float firingForce;
public:
	int currentSouls;
	unsigned int playerNum;
	float segPosition;
	InputStream* inputStream;
	b2Body *rigidBody;
	Boat(b2Vec2 initPos, b2World& m_world, SimpleAI *ai1, unsigned int pNum);
	//float getMaxSpeed();
	void update(float deltaT, GameState &gs);
	float getX();
	float getY();
	float getRot();
};

class LocalBoat: public Boat {
public:
	LocalBoat(b2Vec2 initPos, b2World& m_world, SimpleAI *ai1, unsigned int pNum, std::vector <std::pair<std::string, int>> *broadcastPointer) : 
	Boat(initPos, m_world, ai1, pNum)
	{
		inputStream = new LocalPlayerInputStream(playerNum, broadcastPointer);
	}
};

class NetworkBoat: public Boat {
public:
	NetworkBoat(b2Vec2 initPos, b2World& m_world, SimpleAI *ai1, unsigned int pNum, std::vector <std::pair<std::string, int>> *broadcastPointer, bool isBroadcasting) : 
	Boat(initPos, m_world, ai1, pNum)
	{
		inputStream = new NetworkPlayerInputStream(playerNum, broadcastPointer, isBroadcasting);
	}
};

class AIBoat: public Boat {
public:
	AIBoat(b2Vec2 initPos, b2World& m_world, SimpleAI *ai1, unsigned int pNum, std::vector <std::pair<std::string, int>> *broadcastPointer) : 
	Boat(initPos, m_world, ai1, pNum)
	{
		inputStream = new AIInputStream(playerNum, ai1, broadcastPointer);
	}
};

#endif /* Boat_hpp */
