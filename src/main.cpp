#include "ofMain.h"
#include "ofApp.h"


// argv[1] subject name
// argv[2] search target
// argv[3] selection method

//========================================================================
int main(int argc, char* argv[])
{
	ofGLFWWindowSettings settings;
	settings.resizable = false;
	ofCreateWindow(settings);
	return ofRunApp(new ofApp(argv[1], argv[2], argv[3]));
}
