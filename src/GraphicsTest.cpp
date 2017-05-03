#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include "Box2d/Box2d.h"
#include "Boat.hpp"

int main( int, char**)
{
	b2World *m_world = new b2World(b2Vec2(0.0f,0.0f));
	Boat *m_boat;
	Track *m_track;

	//Initialization
	{
                m_track = new Track(1000,25.0f,50.0f,4);
		SimpleAI *ai = new SimpleAI(m_track,3,.7,.5,.99);
		m_boat = new Boat(b2Vec2(0.0f, 0.0f), *m_world, ai);
	}

	while(true) {
		m_world->Step(1/60.0f,10,10);
		m_boat->update(1/60.0f);
		m_boat->segPosition = m_track->getNewSegPosition(m_boat->segPosition, vec2(m_boat->rigidBody->GetPosition().x, m_boat->rigidBody->GetPosition().y));
		std::cout << m_boat->segPosition << std::endl;
	}
}
