#include "ContactListener.hpp"
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Node>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Quat>

#include <osgDB/ReadFile>
#include <deque>

#include <osgUtil/SmoothingVisitor>

#include <osgGA/NodeTrackerManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <boost/signals2/signal.hpp>
#include <netinet/in.h>
#include <chrono>
#include <thread>
#include <queue>
#include <algorithm>

#include "Box2D/Box2D.h"
#include "Boat.hpp"
#include "GameState.hpp"
#include "Soul.hpp"
#include "AI_1_0.hpp"
#include "Networking.hpp"
#include "FinishLine.hpp"

#include <SDL.h>

using namespace osg;

float x = 0;
float z = 0;

float capLen = 1;
float capWid =0.5f;

const int maxNumBoats = 8;
const int maxNumSouls = 10;

Vec3f up = {0,1,0};
unsigned int myBoat = 0;

PositionAttitudeTransform *transform[maxNumBoats];
PositionAttitudeTransform *transformSouls[maxNumSouls];

// This stub will be swapped out to whatever our OSG implementation becomes
struct Graphics
{
	// Init the viewer and other shit
	osgViewer::Viewer startupScene(GameState *world)
	{
		//Create startup scene with boats loaded
		Group *scene = new Group();
		Node * n = loadBoats(scene, world);

		//Create Track  and load in scene
		osg::Geometry* polyGeom = new osg::Geometry();
		createTrack(polyGeom, world);
		scene->addChild(polyGeom);

		//Create souls and load in scene
		loadSouls(scene, world);

		std::deque<std::string> libs = osgDB::Registry::instance()->getLibraryFilePathList();
		for(auto it = libs.begin(); it != libs.end(); ++it)
			std::cout << "Lib pah: " << *it << std::endl;

		std::cout << "Load Status: " << osgDB::Registry::instance()->loadLibrary("osgdb_osg");
		osg::ref_ptr<Node> airboat = osgDB::readNodeFile("models/airboat.obj");
		std::cout << "Node PTR: "<< airboat << std::endl;

		//Custimize viewer
		osg::ref_ptr<osgViewer::WindowSizeHandler> handler = new osgViewer::WindowSizeHandler();

		osgViewer::Viewer viewer;
		viewer.setUpViewInWindow(500, 50, 800, 800);

		//Set up camera
		viewer.getCamera()->setClearColor(osg::Vec4(0.8f,0.8f,0.8f,0.8f));
	
		Boat *boat = (*(world->boats))[myBoat];
		float x = boat->getX();
		float y = -boat->getY();
		double angle = boat->getRot() + M_PI/2;
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

	void loadSouls(Group *root, GameState *world)
	{
		osg::ref_ptr<osg::ShapeDrawable> myShape = new osg::ShapeDrawable;

		osg::Sphere *sphere = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 1);

		myShape->setShape(sphere);
		myShape->setColor(osg::Vec4(0.152, 0.701, 0.945, 0.5));

		osg::ref_ptr<osg::Geode> myGeode = new osg::Geode;
		myGeode->addDrawable(myShape.get());

		std::vector<Soul*> *souls = world->souls;
		for (int i = 0; i < std::min<int>(souls->size(), maxNumSouls); i++) {
			transformSouls[i] = new PositionAttitudeTransform;
			transformSouls[i]->setPosition(Vec3(souls->at(i)->getX(), 0.5f, souls->at(i)->getY()));
			transformSouls[i]->addChild(myGeode);
			root->addChild(transformSouls[i]);
		}
		
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
			transform[i]->setPosition(Vec3((*it)->getX(), 0.5f, (*it)->getY()));
			transform[i]->setAttitude(Quat((*it)->getRot(), Vec3f(0, -1, 0)));
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
			float x = (*it)->getX();
			float y = (*it)->getY();
			float rot = M_PI/2 + (*it)->getRot();

			int i = it - world->boats->begin();
		//	printf("modify boat %d\n", i);
		//	printf("set (%f, %f)\n", x, y);
			transform[i]->setPosition(Vec3(x, 0.5f, y));
			transform[i]->setAttitude(Quat(rot, Vec3f(0, -1, 0)));
		}
	}
};

int main(int argc, char** argv)
{
	// Game Setup
	std::vector <std::pair<in_addr, int>> broadcastList;
	std::vector <std::pair<in_addr, int>> gamestateBroadcastList;
	std::vector<int> playerTypeList;
	std::vector<int> playerDiscardList;
	bool isHost;
	unsigned int seed = gameSetup(argc, argv, &broadcastList, &gamestateBroadcastList, &playerTypeList, &isHost);

	//Initialize Phyiscs world
	b2World *m_world = new b2World(b2Vec2(0.0f,0.0f));
	Track *m_track = new Track(1000,2.5f,17.0f,4, seed);
	m_track->addTrackToWorld(*m_world);
	GameState *gState = new GameState(*m_track);

	//Add souls to track
	int numSouls = 6;
	vec2* soulPos = m_track->getInitialSoulPositions(numSouls);
	for(int i = 0; i < numSouls; ++i) {
		Soul *s = new Soul(b2Vec2(soulPos[i].x, soulPos[i].y), 5.0f, *m_world);
		gState->addSoul(s);
	}

	//Add finish line to track
	int finishLineSeg = 980;
	vec2 finishL = m_track->l[finishLineSeg];
	vec2 finishR = m_track->r[finishLineSeg];
	vec2 finishM = mul(add(finishL,finishR),0.5f);
	b2Vec2 finishLinePos = b2Vec2(finishM.x, finishM.y);
	FinishLine *finish = new FinishLine(finishLinePos, 17.0f, *m_world);

	//Initialize Contact Listener for physics
	ContactListener contactListener;
	m_world->SetContactListener(&contactListener);

	//Initialize Graphics
	Graphics g;
	boost::signals2::signal<void (GameState*)> sig;
	sig.connect(boost::bind(&Graphics::update, g, _1));

	//Initialize SDL for input handling
	SDL_Event e;
	SDL_Init(SDL_INIT_EVERYTHING);
	
	//Setup Networking
	bool isBroadcasting;
	if (broadcastList.size() == 1)
		isBroadcasting = false;
	else
		isBroadcasting = true;

	int numBoats = playerTypeList.size();
	std::cout << "There are a total of " << std::to_string(numBoats) << " players\n";

	// Set up the Boats
	vec2* boatStartPositions = m_track->getInitialBoatPositions(numBoats, 5.0f, 5.0f);
	for (unsigned int i = 0; i < playerTypeList.size(); i++)
	{
		b2Vec2 startPos = b2Vec2(boatStartPositions[i].x, boatStartPositions[i].y);
		// Local Player
		if (playerTypeList.at(i) == 0)
		{
			playerDiscardList.push_back(i);
			std::cout << "Made local boat at position number " << std::to_string(i) << "\n";
			Boat *local_boat = new LocalBoat(startPos, *m_world, nullptr, i, &broadcastList);
			gState->addPlayer(local_boat);
			myBoat = i;
		}

		// Network Player
		else if (playerTypeList.at(i) == 1)
		{
			std::cout << "Made network boat at position number " << std::to_string(i) << "\n";
			Boat *net_boat = new NetworkBoat(startPos, *m_world, nullptr, i, &broadcastList, isBroadcasting);
			gState->addPlayer(net_boat);
		}

		// AI Player
		else if (playerTypeList.at(i) == 2)
		{
			playerDiscardList.push_back(i);
			std::cout << "Made ai boat at position number " << std::to_string(i) << "\n";
			AI *ai = new AI_1_0(m_track,i,numBoats,17.0f);
			Boat *ai_boat = new AIBoat(startPos, *m_world, ai, i, &broadcastList);
			gState->addPlayer(ai_boat);
		}	
	}

	// If host, will only run networkreceiving thread, otherwise if client will also run gamestate receiving thread
	std::queue<GameStatePatch *> gsp_queue;
	std::thread networkReceivingThread(receiveInputStream, gState, isHost, &playerDiscardList);
	std::thread gamestateReceivingThread(receiveGameStateInfo, gState, isHost, &gsp_queue);

	// Start the osg Viewer and finish graphics init
	osgViewer::Viewer viewer = g.startupScene(gState);

	//Main game loop
	int stopper = 0;
	float timestep = 1/60.0f;
	int i = 0;
	float oldAngle = 0; 
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	while(!viewer.done()) 
	{
		stopper++;
		//if (stopper > 100)
		//	break;
		//Step the physics engine forward 1 frame
		m_world->Step(timestep,10,10);
		//std::cout << "Position: " << m_boat->rigidBody->GetPosition().x << m_boat->rigidBody->GetPosition().y << std::endl;

		while(!gsp_queue.empty()) {
			if(gsp_queue.front()->frame == stopper) {
				std::cout << "good update" << std::endl;
				gsp_queue.front()->applyPatch(gState);
				delete gsp_queue.front();
				gsp_queue.pop();
			}
			if(gsp_queue.front()->frame < stopper) {
				std::cout << "Missed update!" << std::endl;
				gsp_queue.front()->applyPatch(gState);
				std::this_thread::sleep_until(now + (stopper - gsp_queue.front()->frame) * std::chrono::duration<double>(timestep));
				delete gsp_queue.front();
				gsp_queue.pop();
			}
		}

		//Broadcast update to all game entities
		gState->update(timestep);

		// Will send the gamestate only if host
		if (isHost && ((stopper % 10) == 0))
			sendGameStateInfo(gState, gamestateBroadcastList);

		//This passes the gamestate to anything that has registered to sig
		//For example, our graphics would now draw the updated gamestate
		sig(gState);

		//Networking code should register to sig?
		
		//Update graphics camera
		
		Boat *boat = (*(gState->boats))[myBoat];
		float x = boat->getX();
		float y = boat->getY();
		//double angle = boat.getRot();
		double angle =  (boat->getRot() * .25) + (oldAngle * .75);

		Vec3f newEye = {(float)(x - 20*cos(angle)), 10, 
				  (float)(y - 20*sin(angle))};
		Vec3f newCent = {(float)(x+ 5*cos(angle)),5,
				   (float)(y + 5*sin(angle))};
		
		oldAngle = angle;
		viewer.getCamera()->setViewMatrixAsLookAt(newEye, newCent, up);
	
		viewer.frame();
		std::this_thread::sleep_until(now + ++i * std::chrono::duration<double>(timestep));
	}
	return 0;
}
