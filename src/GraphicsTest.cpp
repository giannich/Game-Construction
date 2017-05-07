#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include "Box2d/Box2D.h"
#include "Boat.hpp"
#include <boost/signals2/signal.hpp>
#include "GameState.hpp"
#include "ContactListener.hpp"

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>

// Just testing with 1 boat for now
osg::PositionAttitudeTransform *transform[1];

// This stub will be swapped out to whatever our OSG implementation becomes
struct Graphics
{
	// Init the viewer and other shit
	osgViewer::Viewer startupScene()
	{
		// Create the boat shape and give it some color
		// Feel free to modify the shape of the capsule:
		// Capsule (const osg::Vec3 &center, float radius, float height)
		osg::ref_ptr<osg::ShapeDrawable> boatShape = new osg::ShapeDrawable;
		boatShape->setShape( new osg::Capsule(osg::Vec3(0.0f, 0.0f, 0.0f), 0.05f, 0.10f) );
		boatShape->setColor( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );

		// Create the Geode and add the boat shape to it
		osg::ref_ptr<osg::Geode> rootGeode = new osg::Geode;
		rootGeode->addDrawable( boatShape.get() );

		// Idk whats going on
		osg::Group *rootGroup = new osg::Group();

		transform[0] = new osg::PositionAttitudeTransform;
		transform[0]->setPosition(osg::Vec3(0,0,0));
		transform[0]->addChild(rootGeode);

		rootGroup->addChild(transform[0]);

		// Finally set up the viewer and we pass it out
		osgViewer::Viewer viewer;
		viewer.setSceneData(rootGroup);
		viewer.setCameraManipulator(new osgGA::TrackballManipulator());
		viewer.realize();

		return viewer;
	}

	//Draw 1 frame of the world here
	void renderWorld(GameState* world) 
	{
		for( auto it = world->boats->begin(); it != world->boats->end(); ++it) 
		{
			b2Vec2 boatPos = it->rigidBody->GetPosition();
			float boatRot = it->rigidBody->GetAngle();
			std::cout << "(x,y): " << boatPos.x << ", " << boatPos.y << " | " << boatRot << std::endl;
			// Actual position change

			//Gianni: The changes are super fucking small, so we multiply them by some big factors
			transform[0]->setPosition(osg::Vec3(boatPos.x*10, 0.0f, boatPos.y*100));
		}
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

	//SimpleAI *ai2 = new SimpleAI(m_track,1,.7,.5,.99);
	//Boat *p2_boat = new Boat(b2Vec2(0.0f, -25.0f), *m_world, ai2);

	//Add players to world
	gState->addPlayer(*m_boat);
	//gState->addPlayer(*p2_boat);

	//Gianni: Init the scene and shit
	osgViewer::Viewer viewer = g.startupScene();

	//Main game loop
	float timestep = 1/60.0f;
	while(true) 
	{
		//Step the physics engine forward 1 frame
		m_world->Step(timestep,10,10);

		//Broadcast update to all game entities
		gState->update(timestep);

		//This passes the gamestate to anything that has registered to sig
		//For example, our graphics would now draw the updated gamestate
		sig(gState);
		//Networking code should register to sig?

		//Gianni: Render the frame
		viewer.frame();
	}
}
