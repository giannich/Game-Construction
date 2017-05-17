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

#include "../include/Box2d/Box2d.h"
#include "Track.cpp"
#include "Boat.cpp"
#include "GameState.cpp"

using namespace osg;

float myTimer = 0;
float rot = 1.57;
float capLen = 10;
float capWid = 5;

float x = 0;
float z = 0;

int numBoats = 10;
int myNum = 0;

osg::Matrixd md;
osg::ref_ptr<osg::Camera> camera;

PositionAttitudeTransform *transform[numBoats];

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
		


int createVertices(double **input, osg::Vec3Array* vertices, int size) 
{
	int numCoords = size;
	for (int i = 0; i < numCoords; i++) {
		vertices->push_back(osg::Vec3(input[i][0], 0, input[i][1]));
	}
	return numCoords;
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


void update(float* x1, float* z1, double* angle, Vec3f* vec, GameState g) 
{

	(*x1) = x;
	(*z1) = z;

	//Player controlled boat, set outside for loop for now for input testing
	//But once full integration is done, will be put in the for loop and be 
	//Rendered same way as other boats
	transform[0]->setPosition(Vec3(x, 5, z));
	transform[0]->setAttitude(Quat(rot, Vec3f(0,-1,0)));

	Boat boats = g.boats;

	for(int i = 1; i < numBoats; i++) {
		float x = boats[i].getX();
		float y = boats[i].getY();
		float tempRot = boats[i].getRot();

		transform[i]->setPosition(Vec3(x, 5, y));
		transform[i]->setAttitude(Quad(tempRot, Vec3f(0, -1, 0)));
	}
}

Group * startupScene(Group *root)
{
	
	osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
	osg::Capsule *cap = new osg::Capsule(osg::Vec3(0.0f, 0.0f, 0.0f), capLen, capWid);
	cap->setRotation(Quat(1.57, Vec3f(0,-1,0)));
	shape->setShape(cap);
	shape->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.0f));

	osg::ref_ptr<osg::Geode> anotherGeode = new osg::Geode;
	anotherGeode->addDrawable(shape.get());

	for (int i = 0; i < CHILDREN; i++) {
		transform[i] = new PositionAttitudeTransform;
		transform[i]->setPosition(Vec3(0,0,0));
		transform[i]->addChild(anotherGeode);

		root->addChild(transform[i]);
	}
	return transform[0];
}

GameState createGameState(Track *m_track, float initX, float initY)
{
	GameState g = new GameState(m_track);
	for (int i = 0; i < numBoats; i++) {
	   g.addPlayer(new Boat(new b2Vec2(initX, initY), new b2World(0), null));
	};
}


GameState createTrack(osg::Geometry* polyGeom)
{
	Track *m_track = new Track(1000,25.0f,100.0f,4);
	
	double **input = createInput(m_track->l, m_track->r, 1000);
	float initX = input[0][0];
	float initY = input[0][1];

	osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
	shared_colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	
	osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
	shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));

	osg::Vec3Array* vertices = new osg::Vec3Array;

	int numCoords = createVertices(input, vertices, 1000 * 2);

	osg::Geometry* polyGeom = new osg::Geometry();

	polyGeom->setVertexArray(vertices);
	polyGeom->setColorArray(shared_colors.get(), osg::Array::BIND_OVERALL);
	polyGeom->setNormalArray(shared_normals.get(), osg::Array::BIND_OVERALL);

	polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,numCoords));
	
	GameState g = createGameState(m_track, initX, initY);
	return g;
}

int main() 
{
	//Create track and gamestate
	osg::Geometry* polyGeom = new osg::Geomtry();
	GameState g = createTrackAndGameState(polyGeom);
	
	//create startup scene with boats loaded
	Group *scene = new Group();
	Node * n = startupScene(scene);

	//add map to startup scene
	scene->addChild(polyGeom);

	//customize viewer
	osg::ref_ptr<osgViewer::WindowSizeHandler> handler = new osgViewer::WindowSizeHandler();

	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(500, 50, 800, 800);
	viewer.addEventHandler(handler);

	viewer.addEventHandler(new PickHandler());
	viewer.setSceneData(scene);
	viewer.getCamera()->setClearColor(osg::Vec4(0.8f,0.8f,0.8f,0.8f));

	Vec3d eye(0.0, capWid + 200, -capLen - 400);
	Vec3d upp(0.0, 1.0, 0.0);
	Vec3d center(0.0, 0.0, 100);

	const osg::BoundingSphere& b = n->getBound();
	viewer.getCamera()->setViewMatrixAsLookAt(eye + b.center(), 
											  center + b.center(), 
											  upp);
	scene->addChild(camera);
	viewer.realize();

	//Loop through, update scene
	float scale = 200 / 10;
	float x, y;
	double angle; 
	Vec3f vec; 
	
	while(!viewer.done())
	{
		update(&x, &y, &angle, &vec); //5 ms
		//const osg::BoundingSphere& bs = n->getBound();
		Vec3f newEye, newCent, newUp;

		newEye = {(float)(x - 200*cos(angle)), 50, 
				  (float)(y - 200*sin(angle))};
		newCent = {(float)(x+ 50*cos(angle)),0,
				   (float)(y + 50*sin(angle))};
		newUp = {0,1,0};

		viewer.getCamera()->setViewMatrixAsLookAt(newEye, newCent, newUp);		
		
		viewer.frame();
	}
	
	return 0;
}

