//
//  Boat.cpp
//  Box2D
//
//  Created by Steven Jordan Kozmary on 4/19/17.
//
//

#include "Boat.hpp"


Boat::Boat(float collectionRadius, b2World& m_world)
{
    //Create rigidbody
    {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(0.0f, 0.0f);
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
        fd.restitution = 0.2f;
        rigidBody->SetLinearDamping(1.0f);
        rigidBody->SetAngularDamping(1.0f);
        rigidBody->CreateFixture(&fd);
    }
    
    currentSouls = 0;
    soulCollectionRadius = 5.0f;
    forwardForce = 2000.0f;
    reverseForce = -1000.0f;
    turningImpulse = 1.0f;
    //firingForce = 0.0f;
    
    //Input Stream
    inputState = new InputState();
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
            rigidBody->ApplyAngularImpulse(turningImpulse * deltaT, true);
        }
            break;
        case Right:
        {
            rigidBody->ApplyAngularImpulse(-1.0f * turningImpulse * deltaT, true);
        }
            break;
    }
}
