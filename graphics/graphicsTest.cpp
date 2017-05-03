#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>

typedef struct GameState
{
	float x;
	float y;
	float z;
} GameState;

using namespace osg;
PositionAttitudeTransform *transform[1];

Group *startupScene()
{
	osg::ref_ptr<osg::ShapeDrawable> shape2 = new osg::ShapeDrawable;
    shape2->setShape( new osg::Sphere(osg::Vec3(3.0f, 0.0f, 0.0f), 1.0f) ); 
    shape2->setColor( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );

    osg::ref_ptr<osg::Geode> anotherGeode = new osg::Geode;
    anotherGeode->addDrawable( shape2.get() );

	Group *root = new Group();

	transform[0] = new PositionAttitudeTransform;
	transform[0]->setPosition(Vec3(0,0,0));
	transform[0]->addChild(anotherGeode);

	root->addChild(transform[0]);

	return root;
}

void updateHandler(GameState gameState) {

	transform[0]->setPosition(Vec3(gameState.x, gameState.y, gameState.z));
}

int main() {

	// Init stuff
	Group *scene = startupScene();

	osgViewer::Viewer viewer;
	viewer.setSceneData(scene);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	viewer.realize();

	// GameState Stuff
	GameState aGameState;
	aGameState.x = 0;
	aGameState.y = 0;
	aGameState.z = 0;

	srand (time(NULL));

	while (!viewer.done()) {		
		viewer.frame();
		
		// Update the state of the GameState to be a random walk
		// Values change between -0.5f to +0.5f
		aGameState.x += (float) (rand() % 100 - 50) / 1000;
  		aGameState.y += (float) (rand() % 100 - 50) / 1000;
  		aGameState.z += (float) (rand() % 100 - 50) / 1000;

		updateHandler(aGameState);
	} 

	return 0;
}