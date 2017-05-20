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
#include "InputStream.hpp"
#include "SimpleAI.hpp"

#include "Box2d/Box2d.h"

class GameState;

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
	SimpleAI *ai;
	float segPosition;
    InputState* inputState;
    b2Body *rigidBody;
    Boat(b2Vec2 , b2World&, SimpleAI *ai1);
    //float getMaxSpeed();
    void update(float deltaT);
};

#endif /* Boat_hpp */
