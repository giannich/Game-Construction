#include <osgViewer/Viewer>
#include <osgDB/ReadFile>

int main( int, char**)
{
	osgViewer::Viewer viewer;
	viewer.setSceneData(osgDB::readNodeFile("cow.osg" ));
	return viewer.run();
}
