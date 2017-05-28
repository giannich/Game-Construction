//
//  Boat.cpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#include "Boat.hpp"
#include <algorithm>

Boat::Boat(b2Vec2 initPos, b2World& m_world, SimpleAI *ai1, unsigned int pNum)
{
	collisionHandler = new BoatCollisionHandler(this);
	//Create rigidbody
	{
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(initPos.x, initPos.y);
		rigidBody = m_world.CreateBody(&bd);

		b2PolygonShape shape;
		b2Vec2 vertices[6];
		vertices[0].Set(-0.6f, 0.0f);
		vertices[1].Set(-0.4f, 0.4f);
		vertices[2].Set(0.4f, 0.4f);
		vertices[3].Set(0.6f, 0.0f);
		vertices[4].Set(0.4f, -0.4f);
		vertices[5].Set(-0.4f, -0.4f);
		shape.Set(vertices,6);

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.density = 0.01f;
		fd.restitution = 0.3f;
		rigidBody->SetLinearDamping(0.5f);
		rigidBody->SetAngularDamping(5.0f);
		rigidBody->CreateFixture(&fd);
		rigidBody->SetUserData(collisionHandler);
	}

	playerNum = pNum;
	currentSouls = 0;
	soulCollectionRadius = 5.0f;
	forwardForce = 6.5f;
	reverseForce = -2.0f;
	turnRate = 0.9f;
	segPosition = -0.5;
}

float Boat::dampingCoefficient() {
	int soulCount = std::min(currentSouls,6);
	return 1.0f - 0.1f*soulCount;
}

void Boat::addSoul() {
	currentSouls += 1;
	rigidBody->SetLinearDamping(dampingCoefficient());
}

void Boat::update(float deltaT, GameState &gs)
{
	inputStream->update(deltaT, gs);
	InputState inputState = inputStream->lastInputState;
	switch (inputState.acc)
	{
		case Accelerating:
		{
			b2Vec2 f = rigidBody->GetWorldVector(b2Vec2(forwardForce * deltaT, 0.0f));
			b2Vec2 p = rigidBody->GetWorldPoint(b2Vec2(0.0f, 0.0f));
			rigidBody->ApplyForce(f, p, true);
		}
			break;
		case Reversing:
		{
			b2Vec2 f = rigidBody->GetWorldVector(b2Vec2(reverseForce * deltaT, 0.0f));
			b2Vec2 p = rigidBody->GetWorldPoint(b2Vec2(0.0f, 0.0f));
			rigidBody->ApplyForce(f, p, true);
		}
			break;
	case Idling:
		break;
	}
	
	switch (inputState.turn) {
		case Left:
		{
			rigidBody->SetTransform(rigidBody->GetPosition(), rigidBody->GetAngle() + turnRate * deltaT);
		}
			break;
		case Right:
		{
			rigidBody->SetTransform(rigidBody->GetPosition(), rigidBody->GetAngle() - turnRate * deltaT);
		}
			break;
	case Neutral:
		break;
	}
}

float Boat::getX() {
	return rigidBody->GetPosition().x;
}

float Boat::getY() {
	return rigidBody->GetPosition().y;
}

float Boat::getRot() {
	return rigidBody->GetAngle();
}
