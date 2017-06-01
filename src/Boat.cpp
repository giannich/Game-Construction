//
//  Boat.cpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#include "Boat.hpp"
#include <algorithm>

Boat::Boat(b2Vec2 initPos, b2World& m_world, AI *ai1, unsigned int pNum)
{
	finishedRace = false;
	playerNum = pNum;
	currentSouls = 0;
	soulCollectionRadius = 5.0f;
	//forwardForce = 6.5f;
	reverseForce = -2.0f;
	turnRate = 0.9f;
	segPosition = -0.5;
	disabled = false;

	collisionHandler = new BoatCollisionHandler(this);
	//Create rigidbody
	{
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(initPos.x, initPos.y);
		bd.angle = M_PI/2.0f;
		rigidBody = m_world.CreateBody(&bd);

		/*
		b2PolygonShape shape;
		b2Vec2 vertices[6];
		vertices[0].Set(-0.6f, 0.0f);
		vertices[1].Set(-0.4f, 0.4f);
		vertices[2].Set(0.4f, 0.4f);
		vertices[3].Set(0.6f, 0.0f);
		vertices[4].Set(0.4f, -0.4f);
		vertices[5].Set(-0.4f, -0.4f);
		shape.Set(vertices,6);
		*/

		b2CircleShape front, back;
		b2PolygonShape middle;
		front.m_radius = 0.5f;
		front.m_p.Set(1.0f, 0.0f);
		back.m_radius = 0.5f;
		back.m_p.Set(-1.0f, 0.0f);
		middle.SetAsBox(1.0f,0.5f);

		b2FixtureDef fd;
		fd.density = 0.0012f;
		fd.restitution = 0.2f;

		fd.shape = &front;
		rigidBody->CreateFixture(&fd);
		fd.shape = &middle;
		rigidBody->CreateFixture(&fd);
		fd.shape = &back;
		rigidBody->CreateFixture(&fd);
		
		rigidBody->SetLinearDamping(0.5f);
		rigidBody->SetAngularDamping(5.0f);
		rigidBody->SetUserData(collisionHandler);
	}
}

float Boat::forwardForce() {
	int soulCount = std::min(currentSouls,BOAT_MAX_SOULS);
	return 6.5f + 0.6f*soulCount;
}

void Boat::addSoul() {
	currentSouls += 1;
	std::cout << "Now have " << currentSouls << " Souls" << std::endl;
}

void Boat::update(float deltaT, GameState &gs)
{
	inputStream->update(deltaT, gs);
	InputState inputState;

	if (inputStream->getCurrentFrameNumber() > FRAME_LAG)
		inputState = inputStream->readSingleState(inputStream->getCurrentFrameNumber() - FRAME_LAG - 1);
	else
		inputState = inputStream->readSingleState(inputStream->getCurrentFrameNumber() - 1);

	if (!disabled) {
		switch (inputState.acc)
		{
			case Accelerating: {
				b2Vec2 f = rigidBody->GetWorldVector(b2Vec2(this->forwardForce() * deltaT, 0.0f));
				b2Vec2 p = rigidBody->GetWorldPoint(b2Vec2(0.0f, 0.0f));
				rigidBody->ApplyForce(f, p, true);
				break;
			}
			case Reversing: {
				b2Vec2 f = rigidBody->GetWorldVector(b2Vec2(reverseForce * deltaT, 0.0f));
				b2Vec2 p = rigidBody->GetWorldPoint(b2Vec2(0.0f, 0.0f));
				rigidBody->ApplyForce(f, p, true);
				break;
			}
			case Idling:
			default:
				break;
		}

		switch (inputState.turn) {
			case Left: {
				rigidBody->SetTransform(rigidBody->GetPosition(), rigidBody->GetAngle() + turnRate * deltaT);
				break;
			}
			case Right: {
				rigidBody->SetTransform(rigidBody->GetPosition(), rigidBody->GetAngle() - turnRate * deltaT);
				break;
			}
			case Neutral:
			default:
				break;
		}
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

float Boat::getSpeed() {
	return rigidBody->GetLinearSpeed().Length();
}

