#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>

using namespace osg;


// This stub will be swapped out to whatever our OSG implementation becomes
struct Graphics
{
	// Just testing with 1 boat for now
	PositionAttitudeTransform *transform[1];

	// Returns the viewer
	osgViewer::Viewer startupScene()
	{
		// Create the capsule
		osg::ref_ptr<osg::ShapeDrawable> capsuleShape = new osg::ShapeDrawable;
		capsuleShape->setShape( new osg::Sphere(osg::Vec3(3.0f, 0.0f, 0.0f), 1.0f) ); 
		capsuleShape->setColor( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );

		// Create the Geode
		osg::ref_ptr<osg::Geode> anotherGeode = new osg::Geode;
		anotherGeode->addDrawable( capsuleShape.get() );

		// Other shit
		Group *scene = new Group();

		transform[0] = new PositionAttitudeTransform;
		transform[0]->setPosition(Vec3(0,0,0));
		transform[0]->addChild(anotherGeode);

		scene->addChild(transform[0]);

		osgViewer::Viewer viewer;
		viewer.setSceneData(scene);
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
					transform[0]->setPosition(Vec3(boatPos.x, boatPos.y, 0.0f));
			}
			//world->printDebugInfo();
	}

	// This should be included in the update function
	/*
	while (!viewer.done()) 
		{		
			viewer.frame();
			renderWorld(world);
		} 
	*/
};