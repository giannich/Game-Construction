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

#include "Track.hpp"
#include "GameState.hpp"

class Graphics
{
	osgViewer::Viewer viewer;
public:
	Graphics(Track* track);
	// Will call viewer.frame() to advance the graphics.
	void renderWorld(GameState* world);
}
