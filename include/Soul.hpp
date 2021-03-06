
#ifndef Soul_hpp
#define Soul_hpp
#include "CollisionHandler.hpp"
#include "Box2D/Box2D.h"

class Soul {
	float collectionRadius;
	SoulCollisionHandler *collisionHandler;
public:
	bool collected;
	bool needsDelete;
	b2Body *rigidBody;
	Soul(b2Vec2, float, b2World&);
	void update(float);
	float getX();
	float getY();
	~Soul();
};

#endif /* Soul_hpp */
