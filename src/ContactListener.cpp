#include "ContactListener.hpp"
#include <iostream>

void ContactListener::BeginContact(b2Contact* contact)
{
	std::cout << contact->GetFixtureA() << " | " << contact->GetFixtureB() << std::endl;
}

