
#include <iostream>
#include "Soul.hpp"

Soul::Soul(b2Vec2 initPos, float radius, b2World &world_ref) {
	collected = false;
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

void Soul::update(float deltaT){ 
	if(collected) {
		std::cout << "Disabling collision for Soul!" << std::endl;
		b2Fixture *fd = rigidBody->GetFixtureList();
		b2Filter filterData = fd->GetFilterData();
		filterData.maskBits = 0x0000;
		fd->SetFilterData(filterData);
		std::cout << fd->GetFilterData().categoryBits << " | " << fd->GetFilterData().maskBits << " | " << fd->GetFilterData().groupIndex << std::endl;
	}
}

Soul::~Soul() {
	int *p;
	p = NULL;
	*p;
	std::cout << "Don't call ~Soul!!" << std::endl;
}

float Soul::getX(){
	return rigidBody->GetPosition().x;
}

float Soul::getY() {
	return rigidBody->GetPosition().y;
}
