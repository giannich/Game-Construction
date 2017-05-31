
#ifndef FinishLine_hpp
#define FinishLine_hpp
#include "Box2D/Box2D.h"
#include "CollisionHandler.hpp"

class FinishLine {
	b2Body *rigidBody;
	float radius;
	FinishLineCollisionHandler *collisionHandler;
public:
	FinishLine(b2Vec2, float, b2World&);
};

#endif /* FinishLine_hpp */
