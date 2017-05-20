#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Node>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Quat>

#include <osgDB/ReadFile>

#include <osgUtil/SmoothingVisitor>

#include <osgGA/NodeTrackerManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <boost/signals2/signal.hpp>
#include <chrono>
#include <thread>

#include "Box2D/Box2D.h"
#include "Boat.hpp"
#include "GameState.hpp"

#include <SDL.h>

using namespace osg;

float x = 0;
float z = 0;

float capLen = 10;
float capWid =5;

const int maxNumBoats = 8;

Vec3f up = {0,1,0};
unsigned int myBoat = 0;

PositionAttitudeTransform *transform[maxNumBoats];

// This stub will be swapped out to whatever our OSG implementation becomes
struct Graphics
{
    
	// Init the viewer and other shit
	osgViewer::Viewer startupScene(GameState *world)
	{
		
		//Create startup scene with boats loaded
		Group *scene = new Group();
		Node * n = loadBoats(scene, world);

		//Create Track and load in scene
		osg::Geometry* polyGeom = new osg::Geometry();
		createTrack(polyGeom, world);
		scene->addChild(polyGeom);

		//Custimize viewer
		osg::ref_ptr<osgViewer::WindowSizeHandler> handler = new osgViewer::WindowSizeHandler();

		osgViewer::Viewer viewer;
		viewer.setUpViewInWindow(500, 50, 800, 800);

		//Set up keyboard event handling
		//viewer.addEventHandler(handler);
		//viewer.addEventHandler(new PickHandler());

		//Set up camera
		viewer.getCamera()->setClearColor(osg::Vec4(0.8f,0.8f,0.8f,0.8f));
	
		Boat boat = (*(world->boats))[myBoat];
		float x = boat.getX();
		float y = -boat.getY();
		double angle = boat.getRot() + M_PI/2;
		//double angle = boat.getRot();
		Vec3f newEye, newCent;

		newEye = {(float)(x - 200*cos(angle)), 50, 
				  (float)(y - 200*sin(angle))};
		newCent = {(float)(x+ 50*cos(angle)),0,
				   (float)(y + 50*sin(angle))};

		viewer.getCamera()->setViewMatrixAsLookAt(newEye, newCent, up);

		///Add everything to viewer
		viewer.setSceneData(scene);
		return viewer;
	}

	Group* loadBoats(Group *root, GameState *world)
	{
		osg::ref_ptr<osg::ShapeDrawable> myShape = new osg::ShapeDrawable;
		osg::ref_ptr<osg::ShapeDrawable> otherShape = new osg::ShapeDrawable;

		osg::Capsule *cap = new osg::Capsule(osg::Vec3(0.0f, 0.0f, 0.0f), 
											 capLen, capWid);
		myShape->setShape(cap);
		otherShape->setShape(cap);

		myShape->setColor(osg::Vec4(0.388f, 0.890f, 0.623f, 0.0f));
		otherShape->setColor(osg::Vec4(0.8f, 0.0f, 0.0f, 0.0f));
		
		osg::ref_ptr<osg::Geode> myGeode = new osg::Geode;
		myGeode->addDrawable(myShape.get());

		osg::ref_ptr<osg::Geode> anotherGeode = new osg::Geode;
		anotherGeode->addDrawable(otherShape.get());

		for(auto it = world->boats->begin(); it != world->boats->end(); ++it){
			int i = it - world->boats->begin();
			transform[i] = new PositionAttitudeTransform;
			transform[i]->setPosition(Vec3(it->getX(), 5, it->getY()));
			transform[i]->setAttitude(Quat(it->getRot(), Vec3f(0, -1, 0)));
			if (i != myBoat) {
				transform[i]->addChild(anotherGeode);
			} else {
				transform[i]->addChild(myGeode);
			}
			root->addChild(transform[i]);
		}
		return transform[myBoat];
	}

	double** createInput(vec2* left, vec2* right, int size){
		double** i = 0;
		i = new double*[size * 2];
		int lcount = 0;
		int rcount = 0;

		for(int h = 0; h < size * 2; h++) {
			i[h] = new double[2];
			if (h % 2 == 0) {
				i[h][0] = left[lcount].x;
				i[h][1] = left[lcount].y;
				lcount++;
			} else {
				i[h][0] = right[rcount].x;
				i[h][1] = right[rcount].y;
				rcount++;
			}
		}
		return i;
	}
	
	int createVertices(double **input, osg::Vec3Array* vertices, int size) 
	{
		int numCoords = size;
		for (int i = 0; i < numCoords; i++) {
			vertices->push_back(osg::Vec3(input[i][0], 0, input[i][1]));
		}
		return numCoords;
	}
	
	void createTrack(osg::Geometry* polyGeom, GameState* world)
	{
		Track *m_track = world->m_track;
		double **input = createInput(m_track->l, m_track->r, m_track->N);

		osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
		shared_colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
		
		osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
		shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));

		osg::Vec3Array* vertices = new osg::Vec3Array;
		int numCoords = createVertices(input, vertices, m_track->N * 2);

		polyGeom->setVertexArray(vertices);
		polyGeom->setColorArray(shared_colors.get(), osg::Array::BIND_OVERALL);
		polyGeom->setNormalArray(shared_normals.get(), osg::Array::BIND_OVERALL);

		polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,numCoords));
	}

	void update(GameState* world) 
	{
		//printf("in update\n");
		for(auto it = world->boats->begin(); it != world->boats->end(); ++it) 
		{
			float x = it->getX();
			float y = it->getY();
			float rot = M_PI/2 + it->getRot();

			int i = it - world->boats->begin();
		//	printf("modify boat %d\n", i);
		//	printf("set (%f, %f)\n", x, y);
			transform[i]->setPosition(Vec3(x, 5, y));
			transform[i]->setAttitude(Quat(rot, Vec3f(0, -1, 0)));
		}
	}
};

int main(int argc, char**argv)
{
	//Initialize Phyiscs world
	b2World *m_world = new b2World(b2Vec2(0.0f,0.0f));
	Track *m_track = new Track(1000,25.0f,110.0f,4);
	m_track->addTrackToWorld(*m_world);
	GameState *gState = new GameState(*m_track);

	//Initialize Graphics
	Graphics g;
	boost::signals2::signal<void (GameState*)> sig;
	Networking n;

	if (strncmp(argv[1], "server", 6) == 0)
	{
		sig.connect(boost::bind(&Networking::receivePlayerInfo, n, _1));
		sig.connect(boost::bind(&Graphics::renderWorld, g, _1));
		sig.connect(boost::bind(&Networking::sendGameStateInfo, n, _1));
	}
	else
	{
		sig.connect(boost::bind(&Graphics::renderWorld, g, _1));
		sig.connect(boost::bind(&Networking::sendPlayerInfo, n, _1));
		sig.connect(boost::bind(&Networking::receiveGameStateInfo, n, _1));	
	}

	//Initialize SDL for input handling
	SDL_Event e;
	SDL_Init(SDL_INIT_EVERYTHING);
	
	//Initialize AIs and Players
	//SimpleAI *ai = new SimpleAI(m_track,3,.7,.5,.99);
	Boat *m_boat = new Boat(b2Vec2(12.5f, 0.0f), *m_world, nullptr,0);

	SimpleAI *ai2 = new SimpleAI(m_track,1,.7,.5,.99);
	Boat *p2_boat = new Boat(b2Vec2(12.5f, -25.0f), *m_world, ai2,1);

	//Add players to world
	gState->addPlayer(*m_boat);
	gState->addPlayer(*p2_boat);

	osgViewer::Viewer viewer = g.startupScene(gState);

	//Main game loop
	float timestep = 1/60.0f;
	int i = 0;
	float oldAngle = 0; 
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	while(!viewer.done()) 
	{
		//Step the physics engine forward 1 frame
		m_world->Step(timestep,10,10);
		//Broadcast update to all game entities
		gState->update(timestep);

		//This passes the gamestate to anything that has registered to sig
		//For example, our graphics would now draw the updated gamestate
		sig(gState);

		//Networking code should register to sig?
		
		//Update graphics camera
		
		Boat boat = (*(gState->boats))[myBoat];
		float x = boat.getX();
		float y = boat.getY();
		//double angle = boat.getRot();
		double angle =  (boat.getRot() * .25) + (oldAngle * .75);

		Vec3f newEye = {(float)(x - 200*cos(angle)), 100, 
				  (float)(y - 200*sin(angle))};
		Vec3f newCent = {(float)(x+ 50*cos(angle)),50,
				   (float)(y + 50*sin(angle))};
		
		oldAngle = angle;
		viewer.getCamera()->setViewMatrixAsLookAt(newEye, newCent, up);
	
		viewer.frame();
		std::this_thread::sleep_until(now + ++i * std::chrono::duration<double>(timestep));
	}
}
