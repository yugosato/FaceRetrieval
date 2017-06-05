#include "ofMain.h"
#include "ofApp.h"
#include "NGT/ObjectSpace.h"
#include "boost/python.hpp"
#include "boost/python/numpy.hpp"


boost::python::object NGT::ObjectSpace::neuralWeight;
boost::python::object NGT::ObjectSpace::neuralBias;

//========================================================================
int main()
{
	ofSetupOpenGL(1024, 768, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}
