
#include "Soul.hpp"

Soul::Soul(b2Vec2 initPos, float radius, b2World &world_ref) {
    //Create rigidbody
    {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(initPos.x, initPos.y);
        rigidBody = world_ref.CreateBody(&bd);
        
        b2CircleShape shape(5.0);
        
        b2FixtureDef fd;
	fd.isSensor = true;
        fd.shape = &shape;
        rigidBody->CreateFixture(&fd);
    }
}

Soul::~Soul() {
	rigidBody->GetWorld()->DestroyBody(rigidBody);
}
