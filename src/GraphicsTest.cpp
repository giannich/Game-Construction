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


using namespace osg;

float x = 0;
float z = 0;

float capLen = 5;
float capWid =2;

const int maxNumBoats = 8;

Vec3f up = {0,1,0};
unsigned int myBoat = 0;

PositionAttitudeTransform *transform[maxNumBoats];
/*
class PickHandler : public osgGA::GUIEventHandler {
	public: 
		PickHandler() {}
		~PickHandler() {}
		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	protected:
};

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, 
						 osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
			{
				std::cout<<"Forward"<<std::endl;
				x += 10 * cos(rot);
				z += 10 * sin(rot);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
			{
				std::cout<<"Backward"<<std::endl;
				x -= 10 * cos(rot);
				z -= 10 * sin(rot);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left) {
				std::cout<<"Left"<<std::endl;
				rot -= .1;
				if(rot < 0)
					rot+=6.28;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
			{
				std::cout<<"Right"<<std::endl;
				rot += .1;
				if(rot > 6.29)
					rot-=6.28;


			}
			return false;
		}
		default:
			return false;
	}
}
*/

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
		viewer.getCamera()->resize(100, 100);	

		///Add everything to viewer
		viewer.setSceneData(scene);
		return viewer;
	}

	Group* loadBoats(Group *root, GameState *world)
	{
		osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
		osg::Capsule *cap = new osg::Capsule(osg::Vec3(0.0f, 0.0f, 0.0f), 
											 capLen, capWid);
		shape->setShape(cap);
		shape->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.0f));

		osg::ref_ptr<osg::Geode> anotherGeode = new osg::Geode;
		anotherGeode->addDrawable(shape.get());

		for(auto it = world->boats->begin(); it != world->boats->end(); ++it){
			int i = it - world->boats->begin();
			transform[i] = new PositionAttitudeTransform;
			transform[i]->setPosition(Vec3(it->getX(), 5, it->getY()));
			transform[i]->setAttitude(Quat(it->getRot(), Vec3f(0, -1, 0)));
			transform[i]->addChild(anotherGeode);

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

int main( int, char**)
{
	//Initialize Phyiscs world
	b2World *m_world = new b2World(b2Vec2(0.0f,0.0f));
	Track *m_track = new Track(1000,25.0f,50.0f,4);
	GameState *gState = new GameState(*m_track);

	
	//Initialize AIs and Players
	SimpleAI *ai = new SimpleAI(m_track,3,.7,.5,.99);
	Boat *m_boat = new Boat(b2Vec2(0.0f, 0.0f), *m_world, ai);

	SimpleAI *ai2 = new SimpleAI(m_track,1,.7,.5,.99);
	Boat *p2_boat = new Boat(b2Vec2(0.0f, -25.0f), *m_world, ai2);

	//Add players to world
	gState->addPlayer(*m_boat);
	gState->addPlayer(*p2_boat);

	//Initialize Graphics
	Graphics g;
	boost::signals2::signal<void (GameState*)> sig;
	sig.connect(boost::bind(&Graphics::update, g, _1));

	//Gianni: Init the scene and shit
	osgViewer::Viewer viewer = g.startupScene(gState);

	//Main game loop
	float timestep = 1/60.0f;
	int i = 0;
	float oldAngle = 0; 
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
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
		
		//Update graphics camera
		
		Boat boat = (*(gState->boats))[myBoat];
		float x = boat.getX();
		float y = boat.getY();
		//double angle = boat.getRot();
		double angle =  (boat.getRot() * .25) + (oldAngle * .75);
		printf("rot is %f\n", angle);

		Vec3f newEye = {(float)(x - 200*cos(angle)), 100, 
				  (float)(y - 200*sin(angle))};
		Vec3f newCent = {(float)(x+ 50*cos(angle)),45,
				   (float)(y + 50*sin(angle))};
		
		oldAngle = angle;
		viewer.getCamera()->setViewMatrixAsLookAt(newEye, newCent, up);
	
		viewer.frame();
		std::this_thread::sleep_until(now + ++i * std::chrono::duration<double>(timestep));
		
	}
}
