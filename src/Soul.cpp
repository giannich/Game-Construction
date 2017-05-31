
#include <iostream>
#include "Soul.hpp"

Soul::Soul(b2Vec2 initPos, float radius, b2World &world_ref) {
	collisionHandler = new SoulCollisionHandler(this);
    //Create rigidbody
    {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(initPos.x, initPos.y);
        rigidBody = world_ref.CreateBody(&bd);
	rigidBody->SetUserData(collisionHandler);
        
        b2CircleShape shape;
	shape.m_radius = radius;
        
        b2FixtureDef fd;
	fd.isSensor = true;
        fd.shape = &shape;
        rigidBody->CreateFixture(&fd);
    }
}

Soul::~Soul() {
	std::cout << "Delete Soul!!" << std::endl;
	rigidBody->GetWorld()->DestroyBody(rigidBody);
}

float Soul::getX(){
	return rigidBody->GetPosition().x;
}

float Soul::getY() {
	return rigidBody->GetPosition().y;
}
