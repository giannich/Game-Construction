
#ifndef Soul_hpp
#define Soul_hpp
#include "CollisionHandler.hpp"
#include "Box2D/Box2D.h"

class Soul {
	b2Body *rigidBody;
	float collectionRadius;
	SoulCollisionHandler *collisionHandler;
public:
	Soul(b2Vec2, float, b2World&);
	~Soul();
};

#endif /* Soul_hpp */
