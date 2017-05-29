#include "Box2D/Box2D.h"

class ContactListener : public b2ContactListener
{
	void BeginContact(b2Contact*);
	//void EndContact(b2Contact*);
};

