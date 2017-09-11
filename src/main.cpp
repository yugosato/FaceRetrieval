#include "ofMain.h"
#include "ofApp.h"
#include "NGT/ObjectSpace.h"
#include "boost/python.hpp"
#include "boost/python/numpy.hpp"


boost::python::object NGT::ObjectSpace::neuralExtracter;
bool NGT::ObjectSpace::withTrain;

//========================================================================
int main()
{
	ofGLFWWindowSettings settings;
	settings.resizable = false;
	ofCreateWindow(settings);
	return ofRunApp(new ofApp);
}
