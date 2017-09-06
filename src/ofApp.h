#pragma once


#define VGG
//#define HISTOGRAM
//#define GABOR
//#define HISTOGRAM_GABOR


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include "NGT/Index.h"
#include "ofMain.h"
#include "ofxUI.h"
#include "database.h"
#include "loader.h"
#include "search.h"
#include "writer.h"
#include "trainer.h"
#include "time.h"


const int initWidth_ = 960;				// Initial window size: Width.
const int initHeight_ = 720;			// Initial window size: Height.


// openFrameworks base class.
class ofApp: public ofBaseApp
{
public:
	//-----------------------------------------
	// History.
	int historysize_;					// The number of saved history.
	int backcount_;						// Back count.
	int forwardcount_;                  // Forward count.
	std::vector<std::vector<int>> numberhistory_; // Histories (images' ids).
	std::vector<int> candidatehistory_; // Histories (images' ids; removed duplication).
	int nowhistory_;					// Current history position.
	std::vector<int> firstshowlist_;	// Initial showlist (images' ids).
	bool ishistory_;                    // Flag: History showed.
	bool canBack_;                      // Flag: User can step back to previous results.
	bool canForward_;                   // Flag: User can step forward to next results.

	//-----------------------------------------
	// The number of displayed images (display range [picA - picB]).
	int picA_;							// Start.
	int picB_;							// End.
	int picnum_;						// Total size.

	//-----------------------------------------
	// GUI settings' parameters.
	const int leftsize_ = 240;			// Left region.
	const int topsize_ = 0;				// Upper region.
	const int fontsize_ = 18;			// Font size.
	const int buttonheight_ = 50;       			// Button height.
	const int historybuttonwidth_ = buttonheight_;  // History button width.
	const int removebuttonwidth_ = buttonheight_;   // Remove button width.
	const int buttonposy_line1_ = 5;    // The Y-coordinate of buttons on the 1st line.
	const int backbuttonposx_ = 5;      // The X-coordinate of step back button.
	const int forwardbuttonposx_ = 59;  // The X-coordinate of step forward button.
	const int searchbuttonposx_ = 113;	// The X-coordinate of search button.
	const int searchbuttonwidth_ = 122; // Search button width.
	const int buttonposy_line2_ = buttonposy_line1_ + buttonheight_ + 5; // The Y-coordinate of buttons on the 1st line.
	const int removebuttonposx_ = 5;				// The X-coordinate of button displays the results removed duplication.
	const int non_removebuttonposx_ = 59;			// The X-coordinate of button displays the results remain duplication.
	const int evalbuttonposx_ = 113;				// The X-coordinate of button displays the camparison results.
	const int guiScrollarea_height_ = 280;			// ScrollableCanvas's height (ofxUI).

	//-----------------------------------------
	// Database information.
	int row_;							// The number of database images.
	int col_;							// The number of features' dimensions.
	std::vector<std::string> name_; 	// Image full-path.
	std::vector<int> person_ids_;		// Person ids corresponding to each image.

	//-----------------------------------------
	// Mouse & Keyboard.
	int clickx_;						// The X-coordinate of mouse click.
	int clicky_;						// The Y-coordinate of mouse click.
	int dragx_;							// The X-coordinate of mouse drag.
	int dragy_;							// The Y-coordinate of mouse drag.
	int dragh_;							// Movement: Height.
	int dragw_;							// Movement: Width.
	int presstime_;						// Press time.
	float velocity_;					// Flick Verocity.
	const int pressthreshold_ = 20;		// Press time limitation.
	int mouseover_;						// Current mouseovered image ids.
	bool click_;						// Flag: Clicked.
	bool leftsideclick_;				// Flag: Left side clicked.

	//-----------------------------------------
	// Display Settings.
	int colShow_;						// The number of cols.
	int rowShow_;						// The number of rows.
	int d_size_;						// Size of each displayed image.
	int bottom_;						// The Y-coordinate of current bottom.
	bool rowshort_;						// Flag: Small displayed rows.

	//-----------------------------------------
	// Input Query.
	bool clickflag_;					// Flag: Query clicked.
	std::vector<bool> selectList_;		// Selected (true) or Non-selected (false).
	std::vector<int> selectedquery_;    // Position number of current selected images.
	std::vector<int> nonselectedquery_; // Position number of current non-celected images.
	int selected_num_;					// The number of selected image.

	//-----------------------------------------
	// Window Information.
	int windowWidth_;					// Current window width.
	int windowHeight_;					// Current window height.
	bool onoff_;						// Switch: Full-Screen.

	//-----------------------------------------
	// Font.
	std::string ttf_;					// Font data.

	//-----------------------------------------
	// Path Settings.
	std::string binData_;				// Data (ofApp) directory.
	std::string datasetdir_;			// Dataset directory.
	std::string dataset_;				// Dataset name.
	std::string nameFile_;				// Image list file.
	std::string matrixFile_;			// Image features file (tsv).
	std::string npyFile_;				// Image features file (npy).
	std::string indexFile_;				// Index directory (for NGT).

	// Log Settings.
	std::string logdir_;				// Log output directory.
	std::string candidatefile_main_;   	// Candidate list (main).
	std::string candidatefile_removed_;	// Candidate list (removed duplication).
	std::string candidatefile_eval_;	// Candidate list (comparison).
	std::string init_candidatefile_;	// Initial candidates list.

	// Online Training Settings.
	std::string pysettingfile_;			// Python settings.
	std::string samplefile_;			// Training sample list.
	std::string pythonfile_;			// Script file.

	//-----------------------------------------
	// Others.
	bool isLoaded_;			// Flag: Finished loading image features file.
	bool isSearchedAll_;	// Flag: Finished searching.
	bool canSearch_;		// Flag: Ready to search.
	bool dontmove_;			// Switch: Scroll lock.
	bool goback_;			// Switch: Return the head of results.
	int epoch_;				// The number of current search iteration.
	bool draw_epoch_;		// Flag: draw current search epoch.

	//-----------------------------------------
	// Retrieval results.
	std::vector<int> number_main_;			// Retrieval results (main: images' ids).
	std::vector<int> number_eval_;			// Retrieval results (comparison: images' ids).
	std::vector<int> showList_;			 	// Retrieval results (main: full-paths).
	std::vector<int> showList_removed_;  	// Retrieval results (removeed duplication: full-paths).
	std::vector<int> showList_nonTrain_; 	// Retrieval results (comparison: full-paths).
	bool isremove_;	// Switch: Display results removeed duplication.
	bool iseval_;	// Switch: Display comparison results.

	//-----------------------------------------
	// Time Record.
	clock_t starttime_;			// Total processing time (start).
	clock_t starttime_trainer_; // Online Training processing time (start).
	clock_t starttime_ngt_;     // NGT searching time (start).
	clock_t endtime_;           // Total processing time (end).
	clock_t endtime_trainer_;  	// Online Training processing time (end).
	clock_t endtime_ngt_;		// NGT searching time (end).


public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);


public:
	inline void initparam();
	inline void calculate();
	inline void onPaint(const std::vector<int>& list);
	inline void inputHistory();
	inline void initRange(const int& begin_, const int& end_);
	inline void sizeChanged();
	inline void backhistory();
	inline void forwardhistory();
	inline void back();
	inline void forward();
	inline bool pressbutton(float x, float y, float w, float h);
	inline bool isFileexists(const std::string& filepath);
	inline void writelog();
	inline void loadImageandFont();


public:
	ofxUIScrollableCanvas* gui_;
	void guiSetup();
	void guiEvent(ofxUIEventArgs& e);
	void exit();


public:
	ofTrueTypeFont font_;
	ofImage backbutton0_;
	ofImage backbutton1_;
	ofImage backbutton2_;
	ofImage forwardbutton0_;
	ofImage forwardbutton1_;
	ofImage forwardbutton2_;
	ofImage searchbutton1_;
	ofImage searchbutton2_;
	ofImage non_removebutton1_;
	ofImage non_removebutton2_;
	ofImage removebutton1_;
	ofImage removebutton2_;
	ofImage evalbutton1_;
	ofImage evalbutton2_;

	DataBase* database_;			// Database information.
	NowLoading* loading_;			// Load Image features.
	ImageLoader* loader_; 			// Load Images
	Search* ngt_;					// NGT
	SampleWriter* samplewriter_;	// Training sample writer
	Logger* logger_main_;			// Logger (main).
	Logger* logger_removed_;		// Logger (removed).
	Logger* logger_eval_;			// Logger (comparison method).
	Trainer* trainer_;				// Run Python script.
};
