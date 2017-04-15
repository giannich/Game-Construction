#include "InputStream.hpp"
#include "Box2D"

#define BOAT_MAX_SOULS 5

class Boat {
	InputStream* iStream;
	int currentSouls;
	float soulCollectionRadius;

	b2Body rigidBody;
	float forwardForce;
	float turningTorque;
	float firingForce;
public:
	float getCurrentMaxSpeed();
	//Called every frame - updates the input state from it's InputStream, and adds forces/torque as necessary through the physics engine
	//Also responsible for firing projectiles as directed by the firingState
	void update(GameState &gs);
}

