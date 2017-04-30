#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include "Box2d/Box2d.h"

int main( int, char**)
{
	b2World *m_world = new b2World(b2Vec2(0.0f,0.0f));

	osgViewer::Viewer viewer;
	viewer.setSceneData(osgDB::readNodeFile("cow.osg" ));
	return viewer.run();
}
