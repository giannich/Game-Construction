
#include "FinishLine.hpp"

FinishLine::FinishLine(b2Vec2 initPos, float radius, b2World &world_ref) {
	collisionHandler = new FinishLineCollisionHandler();
	{
		b2BodyDef bd;
		bd.type = b2_staticBody;
		bd.position.Set(initPos.x, initPos.y);
		rigidBody = world_ref.CreateBody(&bd);
		
		b2CircleShape shape;
		shape.m_radius = radius;

		b2FixtureDef fd;
		fd.isSensor = true;
		fd.shape = &shape;
		rigidBody->CreateFixture(&fd);
		rigidBody->SetUserData(collisionHandler);
	}
}
