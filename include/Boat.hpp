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
#include "Box2d/Box2d.h"

class GameState;

#define BOAT_MAX_SOULS 5

class Boat
{
    int currentSouls;
    float soulCollectionRadius;
    
    float forwardForce;
    float reverseForce;
    float turningImpulse;
    //float firingForce;
public:
    InputState* inputState;
    b2Body *rigidBody;
    Boat(float, b2World&);
    //float getMaxSpeed();
    void update(float deltaT);
};

#endif /* Boat_hpp */
