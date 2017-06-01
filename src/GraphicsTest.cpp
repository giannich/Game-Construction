#include "ContactListener.hpp"
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Node>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Quat>
#include <osg/Material>
#include <osg/Texture>

#include <time.h>

#include <osgDB/ReadFile>
#include <deque>

#include <osgUtil/SmoothingVisitor>

#include <osgText/Font>
#include <osgText/Text>

#include <osgGA/NodeTrackerManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/TerrainManipulator>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/CompositeViewer>

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
#include "AI_1_4.hpp"
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

osg::ref_ptr<osg::Camera> normCamera = new osg::Camera; 

PositionAttitudeTransform *transform[maxNumBoats];
PositionAttitudeTransform *transformSouls[maxNumSouls];

time_t startTime;
osg::ref_ptr<osgText::Text> timeText = new osgText::Text;
osg::ref_ptr<osg::Geode> timeGeode = new osg::Geode;

osg::ref_ptr<osg::Geode>posGeode = new osg::Geode;
osg::ref_ptr<osgText::Text> posText = new osgText::Text;

osg::ref_ptr<osg::Geode> soulsTextGeode = new osg::Geode;
osg::ref_ptr<osgText::Text> soulsText = new osgText::Text;

PositionAttitudeTransform *mySpeedBar;
PositionAttitudeTransform *myMaxSpeedBar;
osg::ref_ptr<osg::Geode> speedBarGeode = new osg::Geode;

osg::Box *maxSpeedBox = new osg::Box;
double speedBarHeight = 600;
double speedBarWidth = 8; 

PositionAttitudeTransform *posBarBoats[maxNumBoats];

double maxSpeed = 75;
double soulSpeed = 4.7;
double baseSpeed = 50;
int myNumSouls = -1;

double countDownTime = -5;
osg::ref_ptr<osg::Geode> countDownGeode = new osg::Geode;
osg::ref_ptr<osgText::Text> countDownText = new osgText::Text;



// This stub will be swapped out to whatever our OSG implementation becomes
struct Graphics
{
	struct MapDim{
	float lowX, lowY, highX, highY;
	float length, width;
	MapDim(float lowX, float lowY, float highX, float highY){
		this->lowX = lowX;
		this->lowY = lowY;
		this->highX = highX;
		this->highY = highY;
		this->width = highX - lowX;
		this->length = highY - lowY;
	}
};// Init the viewer and other shit
	osgViewer::Viewer startupScene(GameState *world)
	{
		osgViewer::Viewer viewer;
		
		//Create startup scene with boats loaded
		Group *scene = new Group();
		Node * n = loadBoats(scene, world);

		//Create Track  and load in scene
		osg::Geometry* polyGeom = new osg::Geometry();
		MapDim *m = createTrack(polyGeom, world);
		printf("HERE minM is %f\n", m->lowX);
		scene->addChild(polyGeom);

		//Create souls and load in scene
		loadSouls(scene, world);

		//Load heightmap
		osg::ref_ptr<osg::Node> heightNode = new osg::Node();
		heightNode = createHeightField("heightmap.ppm", m);
		scene->addChild(heightNode);

		//Library debug
		std::deque<std::string> libs = osgDB::Registry::instance()->getLibraryFilePathList();
		for(auto it = libs.begin(); it != libs.end(); ++it)
			std::cout << "Lib pah: " << *it << std::endl;

		std::cout << "Load Status: " << osgDB::Registry::instance()->loadLibrary("osgdb_osg");
		osg::ref_ptr<Node> airboat = osgDB::readNodeFile("models/airboat.obj");
		std::cout << "Node PTR: "<< airboat << std::endl;

		//Create timer, position, and soul count text
		timeText = createText(osg::Vec3(650.0f, 750.0f, 0.0f),
								 "00:00",
								 40.0f);
		posText = createText(osg::Vec3(710.0f, 700.0f, 0.0f),
							 "1st",
							 40.0f);
		soulsText = createText(osg::Vec3(10.0f, 750.0f, 0.0f),
							   "0 souls",
							   20.0f);
		countDownText = createText(osg::Vec3(250.0f, 650.0f, 0.0f),
								   "get ready ...",
								   30.0f);
		timeGeode->addDrawable(timeText);
		posGeode->addDrawable(posText);
		soulsTextGeode->addDrawable(soulsText);
		countDownGeode->addDrawable(countDownText);

		//create speed
		createSpeedBar(0);
		osg::Geode *myBarGeode = new osg::Geode;
		int numBoatsHere = createPositionBar(1000, world, scene, myBarGeode);
		
		//Set up hud camera and ui
		osg::Camera* hudCamera = createHUDCamera(0, 800, 0, 800);
		hudCamera->addChild(timeGeode.get());
		hudCamera->addChild(posGeode.get());
		hudCamera->addChild(soulsTextGeode.get());
		hudCamera->addChild(countDownGeode.get());

		hudCamera->addChild(myBarGeode);
		hudCamera->addChild(speedBarGeode);
		hudCamera->addChild(myMaxSpeedBar);
		hudCamera->addChild(mySpeedBar);

		printf("posBarBoats is %u\n", numBoatsHere);
		for (int i = 0; i < numBoatsHere; i++){
			hudCamera->addChild(posBarBoats[i]);
		}

		hudCamera->getOrCreateStateSet()->setMode(GL_LIGHTING, 
											   osg::StateAttribute::OFF);
		hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
		scene->addChild(hudCamera);

		//Customize viewer
		osg::ref_ptr<osgViewer::WindowSizeHandler> handler = new osgViewer::WindowSizeHandler();

		viewer.getCamera()->setClearColor(osg::Vec4(0.8f,0.8f,0.8f,0.95f));
	//	viewer.setCameraManipulator(new osgGA::TerrainManipulator);
		
		///Add everything to viewer
		viewer.setSceneData(scene);

		return viewer;
	}

	osg::Node* createHeightField(std::string heightFile, MapDim* m)
	{
		osg::Image* heightMap = osgDB::readImageFile(heightFile);
		osg::HeightField* heightField = new osg::HeightField();
		heightField->allocate(heightMap->s(), heightMap->t());
	//	heightField->setOrigin(osg::Vec3(m.lowX + (m.width/2),
						//					m.lowY + (m.length/2), 0.0));
		heightField->setRotation(Quat(M_PI/2, osg::Vec3f(-1,0,0)));
		heightField->setXInterval(2.5f);
    heightField->setOrigin(osg::Vec3(-1000, -100, 2000));
		heightField->setYInterval(2.5f);
	   for (int r = 0; r < heightField->getNumRows(); r++) {
			for (int c = 0; c < heightField->getNumColumns(); c++) {
				heightField->setHeight(c, r, ((*heightMap->data(c, r)) / 255.0f) * 200.0f);
			}
        }

	heightMap = osgDB::readImageFile("heightMap.ppm");

    osg::HeightField* heightField2 = new osg::HeightField();
    heightField2->allocate(heightMap->s(), heightMap->t());
	heightField2->setRotation(Quat(M_PI/2, osg::Vec3f(-1,0,0)));
//							Quat(M_PI/2, osg::Vec3f(0,1,0)) *
//							Quat(M_PI/2, osg::Vec3f(0,0,1)));
    heightField2->setXInterval(2.5f);
   // heightField2->setOrigin(osg::Vec3(-heightMap->s() / 2, 500, heightMap->t() / 2));
	heightField2->setOrigin(osg::Vec3(-1000, -100, 2000));
	heightField2->setYInterval(2.5f);
	   for (int r = 0; r < heightField->getNumRows(); r++) {
			for (int c = 0; c < heightField->getNumColumns(); c++) {
				heightField->setHeight(c, r, ((*heightMap->data(c, r)) / 255.0f) * 200.0f);
			}
        }

		osg::Geode* geode = new osg::Geode();
		geode->addDrawable(new osg::ShapeDrawable(heightField));

		geode->addDrawable(new osg::ShapeDrawable(heightField2));
		osg::Texture2D* tex = new osg::Texture2D(osgDB::readImageFile("color.ppm"));
		tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR_MIPMAP_LINEAR);
		tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
		tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
		geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);

		return geode;

	}

	osgViewer::Viewer* createView(int x, int y, int w, int h, osg::Node* scene)
	{
		osg::ref_ptr<osgViewer::Viewer> view = new osgViewer::Viewer;
		view->setSceneData(scene);
		view->setUpViewInWindow(x,y,w,h);
		return view.release();
	}

	void loadSouls(Group *root, GameState *world)
	{
		
		osg::ref_ptr<Node> head = osgDB::readNodeFile("models/head.ive");
		osg::PositionAttitudeTransform *head_aligned = new PositionAttitudeTransform;
		head_aligned->setPosition(Vec3f(0.0f,1.5f,0.0f));
		head_aligned->setAttitude(Quat(M_PI/2.0f, Vec3f(0,0,-1))*
								  Quat(M_PI/2.0f, Vec3f(-1,0,0)) *
								  Quat(M_PI/2.0f, Vec3f(0,-1,0))
				);	
		osg::Material *green = new osg::Material;
		green->setDiffuse(osg::Material::FRONT, Vec4(0.0f, 0.1f, 0.0f, 0.2f));
		head_aligned->getOrCreateStateSet()->setAttribute(green, osg::StateAttribute::OVERRIDE);
		head_aligned->setScale(Vec3f(0.5f,0.5f,0.5f));
		head_aligned->addChild(head);
/*
		osg::ref_ptr<osg::ShapeDrawable> myShape = new osg::ShapeDrawable;

		osg::Sphere *sphere = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 1);

		myShape->setShape(sphere);
		myShape->setColor(osg::Vec4(0.152, 0.701, 0.945, 0.5));

		osg::ref_ptr<osg::Geode> myGeode = new osg::Geode;
		myGeode->addDrawable(myShape.get());
*/
		std::vector<Soul*> *souls = world->souls;
		for (int i = 0; i < std::min<int>(souls->size(), maxNumSouls); i++) {
			transformSouls[i] = new PositionAttitudeTransform;
			transformSouls[i]->setPosition(Vec3(souls->at(i)->getX(), 0.5f, souls->at(i)->getY()));
			transformSouls[i]->setScale(Vec3(0.01f,0.01f,0.01f));
			transformSouls[i]->addChild(head_aligned);
			root->addChild(transformSouls[i]);
		}
		
	}

	void updateSouls(GameState *world){
		for(auto it = world->souls->begin(); it != world->souls->end(); ++it)
		{
			int i = it - world->souls->begin();
			if(transformSouls[i]){
				//if not disabled check if colleced
				if((*it)->collected == true){
					transformSouls[i]->setScale(Vec3(0.0,0.0,0.0));
					transformSouls[i] = NULL;
				}
			}
		}
	}

	osgText::Text* createText(const osg::Vec3& pos,
							  const std::string& content,
						      float size)
	{
		osg::ref_ptr<osgText::Font> g_font = osgText::readFontFile("fonts/Arial.tff");
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(g_font.get());
		text->setCharacterSize(size);
		text->setAxisAlignment(osgText::TextBase::XY_PLANE);
		text->setPosition(pos);
		text->setText(content);
		return text.release();
	}

	osg::Camera* createHUDCamera(double left, double right, 
								 double bottom, double top)
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		camera->setRenderOrder(osg::Camera::POST_RENDER);
		camera->setAllowEventFocus(false);
		camera->setProjectionMatrix(osg::Matrix::ortho2D(left,right,bottom,top));
		return camera.release();
	}
	
	Group* loadBoats(Group *root, GameState *world)
	{
		osg::ref_ptr<Node> airboat = osgDB::readNodeFile("models/3d-model.3ds");

		osg::PositionAttitudeTransform *airboat_aligned = new PositionAttitudeTransform;
		airboat_aligned->setPosition(Vec3f(0.0f,-8.0f,0.0f));
	//	airboat_aligned->setAttitude(Quat(M_PI / 2.0f, Vec3f(-1, 0, 0)) * Quat(M_PI / 2.0f,Vec3f(0,-1,0))); // This might change if we import a different model
		airboat_aligned->setAttitude(Quat(M_PI / 2.0f, Vec3f(-1, 0, 0)) * Quat(M_PI,Vec3f(0,1,0)));
		airboat_aligned->setScale(Vec3f(0.35f,0.35f,0.35f));
	//	osg::Material *red = new osg::Material;
	//	osg::Material *red = new osg::Material(osgDB::readImageFile("models/model.mtl"));
	//	red->setDiffuse(osg::Material::FRONT, Vec4(0.3f, 0.0f, 0.0f, 0.2f));
	//	airboat_aligned->getOrCreateStateSet()->setAttribute(red, osg::StateAttribute::OVERRIDE);
//		airboat_aligned->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
		airboat_aligned->addChild(airboat);

		for(auto it = world->boats->begin(); it != world->boats->end(); ++it){
			int i = it - world->boats->begin();
			transform[i] = new PositionAttitudeTransform;
			transform[i]->setPosition(Vec3((*it)->getX(), 0.0f, (*it)->getY()));
			transform[i]->setAttitude(osg::Quat(0.0, osg::Vec3f(0, 0, 1)));
			transform[i]->setScale(Vec3(0.07f,0.07f,0.07f));
			transform[i]->addChild(airboat_aligned);
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
	
	int createVertices(double **input, osg::Vec3Array* vertices, int size, 
					   MapDim *m) 
	{
		int numCoords = size;
		float minX;
		float maxX;
		float minY;
		float maxY;

		for (int i = 0; i < numCoords; i++) {
			float x = input[i][0];
			float y = input[i][1];
			vertices->push_back(osg::Vec3(input[i][0], 0, input[i][1]));
			if(minX > x){
				minX = x;
				printf("minx changed to %f\n", minX);
			}else if(maxX < x){
				maxX = x;
			}
			if(minY > y){
				minY = y;
			}else if(minY < y){
				maxY = y;
			}
		}
		m = new MapDim(minX, minY, maxX, maxY); 
		printf("here minX is %f\n", m->lowX);
		return numCoords;
	}

	Vec3f getBoxCenter(double botX, double botY, double botZ,
					  double width, double height)
	{
		return Vec3f(botX + (width/2), botY + (height/2), botZ);
	}
	
	void createSpeedBar(double mySpeed)
	{
		double barHeight = speedBarHeight;
		double barWidth = speedBarWidth;

		double botX = 20;
		double botY = 30;

		Vec3f maxSpeedCenter = getBoxCenter(botX, botY, 0.0,
											barWidth, barHeight);

		Vec3f maxSpeedHalf(barWidth, barHeight/2, 0.1);
		
		osg::ref_ptr<osg::ShapeDrawable> maxSpeedShape = new osg::ShapeDrawable;
		
		maxSpeedBox->set(maxSpeedCenter, maxSpeedHalf);

		maxSpeedShape->setShape(maxSpeedBox);
		maxSpeedShape->setColor(osg::Vec4(0.788, 0.756, 0.756, 1.0f));

		speedBarGeode->addDrawable(maxSpeedShape.get());

		//make my max speed tick
		osg::Box *myMaxSpeedBox = new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f),
												10, 15, 3);
		osg::ref_ptr<osg::ShapeDrawable> myMaxSpeedShape = new osg::ShapeDrawable;
		myMaxSpeedShape->setShape(myMaxSpeedBox);
		myMaxSpeedShape->setColor(osg::Vec4(0.505, 0.505, 0.529, 1.0f));	
		
		osg::ref_ptr<osg::Geode> myMaxSpeedGeode = new osg::Geode;
		myMaxSpeedGeode->addDrawable(myMaxSpeedShape.get());

		myMaxSpeedBar = new PositionAttitudeTransform;
		myMaxSpeedBar->addChild(myMaxSpeedGeode);
			
		//make my speed tick
		osg::Box *mySpeedBox = new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f),
											15, 15, 3);
		
		osg::ref_ptr<osg::ShapeDrawable> mySpeedShape = new osg::ShapeDrawable;
		mySpeedShape->setShape(mySpeedBox);
		mySpeedShape->setColor(osg::Vec4(0.0f, 0.f, 0.0f, 1.0f));

		osg::ref_ptr<osg::Geode> mySpeedGeode = new osg::Geode;
		mySpeedGeode->addDrawable(mySpeedShape.get());

		mySpeedBar = new PositionAttitudeTransform;
		double posY = mySpeed * barHeight / maxSpeed;
		mySpeedBar->setPosition(Vec3f(30 + barWidth/2 - 2, posY, 0.1));
		mySpeedBar->addChild(mySpeedGeode);
	}

	void updateMyMaxSpeedBar(double mySpeed)
	{
		double mySpeedRatio = mySpeed / maxSpeed;
		double barHeight = speedBarHeight;
		double barWidth = speedBarWidth;

		double mySpeedHeight = mySpeed * barHeight / maxSpeed;

		double botX = 20;
		double botY = 30;

		float posY = (mySpeed / maxSpeed) * barHeight + botY;
		myMaxSpeedBar->setPosition(Vec3f(20 + barWidth/2, posY, 0.1));
	}

	void updateSpeedBar(double mySpeed)
	{

		double mySpeedRatio = mySpeed / maxSpeed;
		double barHeight = speedBarHeight;
		double barWidth = speedBarWidth;

		double mySpeedHeight = mySpeed * barHeight / maxSpeed;

		double botX = 20;
		double botY = 30;

		Vec3f mySpeedCenter = getBoxCenter(botX - 2, botY+mySpeedHeight, 0.0, 
										   barWidth + 3, 2);
		
		float posY = (mySpeed / maxSpeed) * barHeight + botY;
		mySpeedBar->setPosition(Vec3f(20 + barWidth/2, posY, 0.1));
	}

	void updatePositionBar(double maxSeg, GameState* world)
	{

		double botX = 770;
		double botY = 30;

		double barHeight = speedBarHeight;
		double barWidth = speedBarWidth;

		for(auto it=world->boats->begin(); it!= world->boats->end(); ++it)
		{
			int i = it - world->boats->begin();
			float posY = ((*it)->segPosition * barHeight / maxSeg ) + botY;
			posBarBoats[i]->setPosition(Vec3(botX + (barWidth/2), posY, 0.1));
		}
	}	

	int createPositionBar(double maxSeg, GameState* world, Group* root,
						   osg::Geode*  myBarGeode)
	{
		double allBoatSeg[maxNumBoats];
		double mySeg;
		int allBoatCounter = 0;

		double botX = 770;
		double botY = 30;

		double oRadius = 5;
		double mRadius = 6; 

		double barHeight = speedBarHeight;
		double barWidth = speedBarWidth;

		osg::ref_ptr<osg::ShapeDrawable> oShape = new osg::ShapeDrawable;
		osg::ref_ptr<osg::ShapeDrawable> mShape = new osg::ShapeDrawable;

		osg::Sphere *oS = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), oRadius);
		osg::Sphere *mS = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), mRadius);

		oShape->setShape(oS);
		mShape->setShape(mS);

		oShape->setColor(osg::Vec4(0.505, 0.505, 0.529, 1.0f));
		mShape->setColor(osg::Vec4(0.301, 0.301, 0.309, 1.0f));

		osg::ref_ptr<osg::Geode> oGeode = new osg::Geode;
		oGeode->addDrawable(oShape.get());

		osg::ref_ptr<osg::Geode> mGeode = new osg::Geode;
		mGeode->addDrawable(mShape.get());

		int i = 0;

		double yOff = 30;

		for(auto it=world->boats->begin(); it!= world->boats->end(); ++it)
		{
			i = it - world->boats->begin();
			float posY = ((*it)->segPosition * barHeight / maxSeg) + yOff;
			printf("posy: %f\n", posY);
			posBarBoats[i] = new PositionAttitudeTransform;
			posBarBoats[i]->setPosition(Vec3(botX + (barWidth/2), posY, 0.1));  
			if(i != myBoat){
				posBarBoats[i]->addChild(oGeode);
			}else{
				posBarBoats[i]->addChild(mGeode);	
			}
		}
	
		//createBar

		Vec3f barCenter = getBoxCenter(botX, botY, 0.0, barWidth, barHeight);
		Vec3f barHalf(barWidth, barHeight/2, 0.1);
		
		osg::Box *myBox = new osg::Box;
		myBox->set(barCenter, barHalf);
		
		osg::ref_ptr<osg::ShapeDrawable> myBar = new osg::ShapeDrawable;
		myBar->setShape(myBox);
		myBar->setColor(osg::Vec4(0.788, 0.756, 0.756, 1.0f));

		myBarGeode->addDrawable(myBar.get());
		return i + 1;
	}
					

	MapDim* createTrack(osg::Geometry* polyGeom, GameState* world)
	{
		MapDim *m;
		Track *m_track = world->m_track;
		double **input = createInput(m_track->l, m_track->r, m_track->N);

		osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
		shared_colors->push_back(osg::Vec4(0.850, 0.843, 0.866, 1.0f));
	//	shared_colors->push_back(osg::Vec4(0.788, 0.756, 0.756, 1.0f));

		shared_colors->push_back(osg::Vec4(0.937, 0.862, 0.862, 1.0f));

		osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
		shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));

		osg::Vec3Array* vertices = new osg::Vec3Array;
		int numCoords = createVertices(input, vertices, m_track->N * 2, m);
		printf("x is %f width is %f\n", m->lowX, m->width);

		polyGeom->setVertexArray(vertices);
		polyGeom->setColorArray(shared_colors.get(), osg::Array::BIND_OVERALL);
		polyGeom->setNormalArray(shared_normals.get(), osg::Array::BIND_OVERALL);

		polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,numCoords));
		return m;
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
			transform[i]->setAttitude(Quat(rot + M_PI / 2.0f, Vec3f(0, -1, 0)));
		}
		if (countDownTime > 0){
			time_t curTime;
			time(&curTime);
			int sec = (int) difftime(curTime, startTime);
			int min = sec / 60;
			sec = sec % 60;
			std::string minS;
			std::string secS;
			if (min < 10){
				minS = "0" + std::to_string(min);
			}else{
				minS = std::to_string(min);
			}

			if(sec < 10){
				secS = "0" + std::to_string(sec);
			}else{
				secS = std::to_string(sec);
			}
			std::string timeS = minS + ":" + secS;
			timeText = NULL;
			timeText = createText(osg::Vec3(650.0f, 750.0f, 0.0f),
								  timeS, 40.00f);
			timeGeode->removeDrawables(0);
			timeGeode->addDrawable(timeText);
		}else if (countDownTime == -1 || countDownTime == -2 || 
				  countDownTime == -3 ){
			std::string sec = std::to_string((int)(countDownTime * -1)) + "...";
			countDownText = NULL;
			countDownText = createText(osg::Vec3(350.0f, 650.0f, 0.0f),
									   sec, 60.0f);
			countDownGeode->removeDrawables(0);
			countDownGeode->addDrawable(countDownText);
		}else if (countDownTime == -0.5){
			countDownText = NULL;
			countDownText = createText(osg::Vec3(350.0f, 650.0f, 0.0f),
									   "go!!!", 40.0f);
			countDownGeode->removeDrawables(0);
			countDownGeode->addDrawable(countDownText);
		}else if(countDownTime == 0){
			countDownText = NULL;
			countDownGeode->removeDrawables(0);
		}
									   	

		int pos = getPosition(world);
		std::string posString;
		if(pos == 1){
			posString = "1st";
		}else if (pos == 2){
			posString = "2nd";
		}else if (pos == 3){
			posString = "3rd";
		}else{
			posString = std::to_string(pos) + "th";
		}
		posText = NULL;
		posText = createText(osg::Vec3(700.0f, 700.0f, 0.0f),
				  posString, 40.0f);
		posGeode->removeDrawables(0);
		posGeode->addDrawable(posText);

		Boat *myBoatObj = world->boats->at(myBoat);
		std::string soulString = std::to_string(myNumSouls) + " souls";
		soulsText = NULL;
		soulsText = createText(osg::Vec3(10.0f, 750.0f, 0.0f),
					           soulString, 40.0f);
		soulsTextGeode->removeDrawables(0);
		soulsTextGeode->addDrawable(soulsText);

		updateSpeedBar(myBoatObj->getSpeed());
		if ((int) myBoatObj->currentSouls > myNumSouls){
			myNumSouls = myBoatObj->currentSouls;
			updateMyMaxSpeedBar(baseSpeed + (soulSpeed * myNumSouls));
		}
		updatePositionBar(1000, world);
		updateSouls(world);
		
	}
	int getPosition(GameState* world)
	{
		int pos = 1;
		Boat *myBoatObj = world->boats->at(myBoat);
		float mySeg = myBoatObj->segPosition;
		for (auto it = world->boats->begin(); it != world->boats->end(); ++it){
			int i = it - world->boats->begin();
			if (i != myBoat){
			   if((*it)->segPosition > mySeg){
				   pos++;
			   }
			}
		}
		return pos;
	}
};
struct Color {
	double r;
	double g;
	double b;
	Color(double r, double g, double b){
		this->r = r;
		this->g = g;
		this->b = b;
	}
};
struct ColorKey {
	double min;
	double max;
	Color *c;
	//Elevation 1 is lowest, 4 is highest
	ColorKey(double min, double max, int elevation){
		this->min = min;
		this->max = max;
		if(elevation == 1) {
			this->c = new Color(77,77,79); //water blue
		} else if (elevation == 2) {
			this->c = new Color(129,129,135); //desert brown
		} else if (elevation == 3) {
			this->c = new Color(140,140,144); //lush green
		} else if (elevation == 4) {
			this->c = new Color(201,193,193); //less lush green
		} else if (elevation == 5) {
			this->c = new Color(217,215,221); //snow white
		}
	}
	int include(double z){
		if(z >= this->min && z <= this->max){
			return 1;
		} 
		return 0;
	}
};



struct Bounds {
	double max;
	double min;
	double diff;
	Bounds (double max, double min){
		this->max = max;
		this->min = min;
		this->diff = max - min;
	}
};
/*
Struct heightMap
{
	double** heightMap;
	double** smallHeightMap;

	void createHeightMap(int numX, int numY, Map map)
	{
		heightMap = new double*[numY];
		for(int i =0; i<numX; i++){
			heightMap[i] = new double
		double heightMap[numY][numX] = {0};
		double smallHeightMap[numY/2][numX/2];

	Bounds integrateMaps()
	{
		int scale = mapChunkSize / smallChunkSize;
		double max = terrainMap[0][0];
		double min = terrainMap[0][0];
		for (int i = 0; i < mapChunkSize; i++){
			for (int j = 0; j < mapChunkSize; j++){
				int sI = std::min(i / scale, smallChunkSize - 1);
				int sJ = std::min(j / scale, smallChunkSize - 1);
				double value = (terrainMap[i][j] * 0.3) + 
							   (0.7 * smallHeightMap[sI][sJ]);
				terrainMap[i][j] = value;
				if (value > max) {
					max = value;
				} else if (value < min) {
					min = value;
				}
			}		
		}
		
		//keep values positive
		for (int i = 0; i < mapChunkSize; i++) {
			for (int j = 0; j < mapChunkSize; j++){
				terrainMap[i][j] += std::abs(min);
			}
		}
		
		return Bounds(max + std::abs(min), 0);
	}
}
*/


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

	Map map = m_track->getMap(2, 2, 30);
	//heightMap h;
//	h.createHeightMap(map.x_n, map.y_n, map);
	printf("%u by %u grid\n", map.x_n, map.y_n);

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
			Boat *local_boat = new LocalBoat(startPos, *m_world, nullptr, i, &broadcastList, gState);
			gState->addPlayer(local_boat);
			myBoat = i;
		}

		// Network Player
		else if (playerTypeList.at(i) == 1)
		{
			std::cout << "Made network boat at position number " << std::to_string(i) << "\n";
			Boat *net_boat = new NetworkBoat(startPos, *m_world, nullptr, i, &broadcastList, isBroadcasting, gState);
			gState->addPlayer(net_boat);
		}

		// AI Player Type 1
		else if (playerTypeList.at(i) == 2)
		{
			playerDiscardList.push_back(i);
			std::cout << "Made ai boat of type 1 at position number " << std::to_string(i) << "\n";
			AI *ai = new AI_1_0(m_track,i,numBoats,17.0f);
			Boat *ai_boat = new AIBoat(startPos, *m_world, ai, i, &broadcastList, gState);
			gState->addPlayer(ai_boat);
		}

		// AI Player Type 2
		else if (playerTypeList.at(i) == 3)
		{
			playerDiscardList.push_back(i);
			std::cout << "Made ai boat of type 2 at position number " << std::to_string(i) << "\n";
			AI *ai = new AI_1_4(m_track,i,numBoats,1.0f,0.6f,0.4f);
			Boat *ai_boat = new AIBoat(startPos, *m_world, ai, i, &broadcastList, gState);
			gState->addPlayer(ai_boat);
		}

		// AI Player Type 3
		else if (playerTypeList.at(i) == 4)
		{
			playerDiscardList.push_back(i);
			std::cout << "Made ai boat of type 3 at position number " << std::to_string(i) << "\n";
			AI *ai = new AI_1_4(m_track,i,numBoats,1.0f,0.8f,0.5f);
			Boat *ai_boat = new AIBoat(startPos, *m_world, ai, i, &broadcastList, gState);
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

		//End of pre-race countdown
		if (stopper == 300) {
			for(auto it = gState->boats->begin(); it != gState->boats->end(); ++it) {
				(*it)->disabled = false; //Enable input for all players
				time(&startTime);
				countDownTime = 0;
			}
		}else if (stopper == 120){
			countDownTime = -3;
		}else if (stopper == 180){
			countDownTime = -2;
		}else if (stopper == 240){
			countDownTime = -1;
		}else if (stopper == 280){
			countDownTime = -0.5;
		}else if(stopper == 301){
			countDownTime = 1;
		}

		//Step the physics engine forward 1 frame
		m_world->Step(timestep,10,10);

		//Take input from gamestate patch queue, if present
		while(!gsp_queue.empty()) {
			if(gsp_queue.front()->frame == stopper) {
				std::cout << "good update" << std::endl;
				gsp_queue.front()->applyPatch(gState);
				delete gsp_queue.front();
				gsp_queue.pop();
			}
			if(gsp_queue.front()->frame < stopper) {
				std::cout << "Late update!" << std::endl;
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
		
		Boat *boat = (*(gState->boats))[1];
		float x = boat->getX();
		float y = boat->getY();
		//double angle = boat.getRot();
		double angle =  (boat->getRot() * .25) + (oldAngle * .75);

		Vec3f newEye = {(float)(x - 20*cos(angle)), 10, 
				  (float)(y - 20*sin(angle))};
		Vec3f newCent = {(float)(x+ 5*cos(angle)),5,
				   (float)(y + 5*sin(angle))};
		
		oldAngle = angle;
	//	normCamera->setViewMatrixAsLookAt(newEye, newCent, up);
	//	viewer.removeSlave(1);
	//	viewer.addSlave(normCamera);
	//	viewer.getSlave(0)._camera->setViewMatrixAsLookAt(newEye, newCent, up);
		viewer.getCamera()->setViewMatrixAsLookAt(newEye, newCent, up);
		
	
		viewer.frame();
		std::this_thread::sleep_until(now + ++i * std::chrono::duration<double>(timestep));
	}
	return 0;
}
