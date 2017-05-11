//
//  Boat.cpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#include "Boat.hpp"

Boat::Boat(b2Vec2 initPos, b2World& m_world)
{
    currentSouls = 0;
    soulCollectionRadius = 5.0f;
    forwardForce = 500.0f;
    reverseForce = -20.0f;
    turnRate = 0.0003f;
    //turningImpulse = 1.0f;
    //firingForce = 0.0f;
    
    //Create rigidbody
    {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(initPos.x, initPos.y);
        rigidBody = m_world.CreateBody(&bd);
        
        b2PolygonShape shape;
        b2Vec2 vertices[6];
        vertices[0].Set(-6.0f, 0.0f);
        vertices[1].Set(-4.0f, 4.0f);
        vertices[2].Set(4.0f, 4.0f);
        vertices[3].Set(6.0f, 0.0f);
        vertices[4].Set(4.0f, -4.0f);
        vertices[5].Set(-4.0f, -4.0f);
        shape.Set(vertices,6);
        
        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 1.0f;
        fd.restitution = 0.7f;
        rigidBody->SetLinearDamping(1.0f);
        rigidBody->SetAngularDamping(5.0f);
        rigidBody->CreateFixture(&fd);
        rigidBody->SetUserInfo(this);
    }
    
    //Input Stream
    inputState = new InputState();
}

float Boat::dampingCoefficient() {
    int soulCount = min(currentSouls,6);
    return 1.0f - 0.1f*soulCount;
}

void Boat::addSoul() {
    currentSouls += 1;
    rigidBody->SetLinearDamping(dampingCoefficient());
}

void Boat::update(float deltaT)
{
    switch (inputState->acc)
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
    }
    
    switch (inputState->turn) {
        case Left:
        {
            rigidBody->SetTransform(rigidBody->GetPosition(), rigidBody->GetAngle() + turnRate * deltaT);
            //rigidBody->ApplyAngularImpulse(turningImpulse * deltaT, true);
        }
            break;
        case Right:
        {
            rigidBody->SetTransform(rigidBody->GetPosition(), rigidBody->GetAngle() - turnRate * deltaT);
            //rigidBody->ApplyAngularImpulse(-1.0f * turningImpulse * deltaT, true);
        }
            break;
    }
}
