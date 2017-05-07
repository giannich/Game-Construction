#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include "Box2D/Box2D.h"
#include "Boat.hpp"
#include <boost/signals2/signal.hpp>
#include "GameState.hpp"
#include "ContactListener.hpp"

// This stub will be swapped out to whatever our OSG implementation becomes
struct Graphics
{
	//Draw 1 frame of the world here
	void renderWorld(GameState* world) {
		for( auto it = world->boats->begin(); it != world->boats->end(); ++it) {
			b2Vec2 boatPos = it->rigidBody->GetPosition();
			float boatRot = it->rigidBody->GetAngle();
			std::cout << "(x,y): " << boatPos.x << ", " << boatPos.y << " | " << boatRot << std::endl;
		}
		//world->printDebugInfo();
	}
};

int main( int, char**)
{
	//Initialize Phyiscs world
	b2World *m_world = new b2World(b2Vec2(0.0f,0.0f));
	Track *m_track = new Track(1000,25.0f,50.0f,4);
	GameState *gState = new GameState(*m_track);
	ContactListener contactListener;
	m_world->SetContactListener(&contactListener);

	//Initialize Graphics
	Graphics g;
	boost::signals2::signal<void (GameState*)> sig;
	sig.connect(boost::bind(&Graphics::renderWorld, g, _1));

	//Initialize AIs and Players
	SimpleAI *ai = new SimpleAI(m_track,3,.7,.5,.99);
	Boat *m_boat = new Boat(b2Vec2(0.0f, 0.0f), *m_world, ai);

	SimpleAI *ai2 = new SimpleAI(m_track,1,.7,.5,.99);
	Boat *p2_boat = new Boat(b2Vec2(0.0f, -25.0f), *m_world, ai2);

	//Add players to world
	gState->addPlayer(*m_boat);
	gState->addPlayer(*p2_boat);

	//Main game loop
	float timestep = 1/60.0f;
	while(true) {
		//Step the physics engine forward 1 frame
		m_world->Step(timestep,10,10);

		//Broadcast update to all game entities
		gState->update(timestep);

		//This passes the gamestate to anything that has registered to sig
		//For example, our graphics would now draw the updated gamestate
		sig(gState);
		//Networking code should register to sig?
	}
}
