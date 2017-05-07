#include "Box2d/Box2d.h"

class ContactListener : public b2ContactListener
{
	void BeginContact(b2Contact*);
	//void EndContact(b2Contact*);
};

