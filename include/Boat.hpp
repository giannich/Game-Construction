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
    int currentSouls;
    int playerNum;
    float soulCollectionRadius;
    
    float forwardForce;
    float reverseForce;
    //float turningImpulse;
    float turnRate;
    //float firingForce;
public:
	float segPosition;
    InputStream* inputStream;
    b2Body *rigidBody;
    Boat(b2Vec2 , b2World&, SimpleAI *ai1, int pNum);
    //float getMaxSpeed();
    void update(float deltaT, GameState &gs);
	float getX();
    float getY();
	float getRot();
};

#endif /* Boat_hpp */
