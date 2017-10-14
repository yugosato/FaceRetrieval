#pragma once


#define VGG
//#define HISTOGRAM
//#define GABOR
//#define HISTOGRAM_GABOR
//#define VISUALRANK

#define OPENUSE


#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <stdio.h>
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
#include "selection.h"
#include "ScrollBar4OF.hpp"
#include "rerank.h"
#include "VisualRank.h"
#include "evaluation.h"
#include "rocchio.h"


// openFrameworks base class.
class ofApp: public ofBaseApp
{
public:
	//-----------------------------------------
	// Startup arguments.
	std::string subjectname_;						// Subject name.
	int searchTarget_;								// Search target.
	std::string selection_method_;                  // "uncertain"/"random"/"traditional"

	//-----------------------------------------
	// History.
	int historysize_;								// The number of saved history.
	int backcount_;									// Back count.
	int forwardcount_;								// Forward count.
	std::vector<std::vector<int>> numberhistory_;	// Histories (images' ids).
	std::vector<int> candidatehistory_;				// Histories (images' ids; removed duplication).
	int nowhistory_;								// Current history position.
	std::vector<int> firstshowlist_;				// Initial showlist (images' ids).
	bool ishistory_;								// Flag: History showed.
	bool canBack_;									// Flag: User can step back to previous results.
	bool canForward_;								// Flag: User can step forward to next results.

	//-----------------------------------------
	// GUI settings' parameters.
	const int leftsize_ = 1000;												// Left region.
	const int uppermargin_ = 20;											// Upper margin.
	const int uppersize_ = uppermargin_ + 40;								// Upper region.
	const int button_height_ = 30;											// Button height.
	const int button_width_ = button_height_;								// Show active selection results button width.
	const int buttonposy_line1_ = uppermargin_ + 5;							// The Y-coordinate of buttons on the 1st line.
	const int searchbutton_posx_ = 1000;									// The X-coordinate of search button.
#ifndef OPENUSE
	const int searchbutton_width_ = 90;										// Search button width.
#else
	const int searchbutton_width_ = button_width_;
#endif
	const int buttonA_posx_ = searchbutton_posx_ + searchbutton_width_ + 5;	// The X-coordinate of button displays the active selection results.
	const int buttonB_posx_ = buttonA_posx_ + button_width_ + 5;			// The X-coordinate of button displays the camparison results.
	const int buttonC_posx_ = buttonB_posx_ + button_width_ + 5;			// The X-coordinate of button displays the reranked results.
	const int buttonD_posx_ = buttonC_posx_ + button_width_ + 5;			// The X-coordinate of button displays the visualrank results.
	const int guiScrollarea_height_ = 280;									// ScrollableCanvas's height (ofxUI).

	//-----------------------------------------
	// Database information.
	int row_;							// The number of database images.
	int col_;							// The number of features' dimensions.
	std::vector<std::string> name_;		// Image full-path.
	std::vector<int> person_ids_;		// Person ids corresponding to each image.

	//-----------------------------------------
	// Mouse & Keyboard.
	int clickx_;						// The X-coordinate of mouse click.
	int clicky_;						// The Y-coordinate of mouse click.
	int dragx_;							// The X-coordinate of mouse draging.
	int dragy_;							// The Y-coordinate of mouse draging.
	int mouseoverx_;					// The X-coordinate of mouse click.
	int mouseovery_;					// The Y-coordinate of mouse click.
	int mouseover_;						// Current mouseovered image positions.
	bool click_;						// Flag: Clicked.
	bool leftsideclick_;				// Flag: Left side clicked.

	//-----------------------------------------
	// Display Settings.
	int colShow_;						// The number of cols.
	int rowShow_;						// The number of rows.
	int d_size_;						// Size of each displayed image.
	int area_width_;					// Area A width.
	int area_height_;					// Area A height.

	//-----------------------------------------
	// Input Query.
	bool clickflag_;					// Flag: Query clicked.

	//-----------------------------------------
	// Window Information.
	const int initWidth_ = 1600;		// Initial window size: Width.
	const int initHeight_ = 920;		// Initial window size: Height.
	int windowWidth_;					// Current window width.
	int windowHeight_;					// Current window height.

	//-----------------------------------------
	// Path Settings.
	std::string binData_;					// Data (ofApp) directory.
	std::string datasetdir_;				// Dataset directory.
	std::string dataset_;					// Dataset name.
	std::string nameFile_;					// Image list file.
	std::string featuresfile_;				// Image features file (tsv).
	std::string npyFile_;					// Image features file (npy).
	std::string indexFile_;					// Index directory (for NGT).

	// Log Settings.
	std::string logdir_;					// Log output directory.
	std::string candidatefile_active_;		// Candidate list (active selection).
	std::string candidatefile_origin_;		// Candidate list (comparison).
	std::string candidatefile_rerank_;		// Candidate list (rerank).
	std::string candidatefile_visualrank_;	// Candidate list (visualrank).
	std::string init_candidatefile_;		// Initial candidates list.
	std::string evaluationfile_;			// Evaluation result file.
	std::string testsettingfile_;			// Test setting file.

	// Python Settings.
	std::string pysettingfile_;				// Python settings.
	std::string samplefile_;				// Training sample list.
	std::string scriptfile_;				// Script file (Trainer).
	std::string positiveIndexfile_;			// Estimated positive index file.
	std::string negativeIndexfile_;			// Estimated negative index file.
	std::string activeIndexfile_;			// Active Selection index file.
	std::string cueflikIndexfile_;			// Active Selection index file (CueFlik).
	std::string randomIndexfile_;			// Random Selection index file.
	std::string resultGraphfile_;			// For update graph.
	std::string new_featuresfile_;			// For rerank method.

	//-----------------------------------------
	// Font.
	std::string ttf_;								// Font data.
	const int fontsize_ = 16;						// Font size.
	const int fontposy_top_ = + uppermargin_ + 27;	// The Y-coordinate of top line text.

	//-----------------------------------------
	// Overview Settings.
	bool isHolding_areaA_;					// Flag: Holding areaA.
	bool isHolding_areaP_;					// Flag: Holding areaP.
	bool isHolding_areaN_;					// Flag: Holding areaN.
	bool isHoldAndDrag_;					// Flag: Holding and draging image.
	bool isInsideWindow_;					// Flag: Mouse pointer is inside window.
	bool isInside_areaA_;					// Flag: Holding inside area A.
	bool isInside_areaP_;					// Flag: Holding inside area P.
	bool isInside_areaN_;					// Flag: Holding inside area N.
	bool isInside_propose_;					// Flag: Holding inside proposd img.
	bool isJudgeTrue_;							// Flag: Judge.
	bool isJudgeFalse_;							// Flag: False to judging.
	int overview_colShow_;					// The number of cols.
	int overviewP_rowShow_;					// The number of rows (area P).
	int overviewN_rowShow_;					// The number of rows (area N).
	const float holdImg_scale_ = 0.8;		// Holding image scale.
	int holdImgNum_;						// Holding image number;
	int holding_x_;							// The X-coordinate of held image.
	int holding_y_;							// The Y-coordinate of held image.
	int overview_areamargin_;				// Margin of Overview area.
	int overview_d_size_;					// Size of each displayed image (overview).
	int perHeight_;							// Split window into parts.
	int positive_txt_posy_;					// "Positive" text position.
	int negative_txt_posy_;					// "Negative" text position.
	int propose_txt_posx_;					// "Is this person?" text position.
	int propose_txt_posy_;					// "Is this person?" text position.
	int overviewP_areaposy_;				// The Y-coordinate of positive overview.
	int overviewN_areaposy_;				// The Y-coordinate of negative overview.
	int overview_areawidth_;				// Every overview width.
	int overview_areawidth_wide_;			// Every overview width (wide).
	int overview_areaheight_;				// Every overview height.
	int propose_imgsize_;					// Proposed image size
	int propose_img_posx_;					// Proposed image position.
	int propose_img_posy_;					// Proposed image position.
	int len_positives_;						// The number of positive samples.
	int len_negatives_;						// The number of negative samples.
	std::vector<ofImage> positive_images_;	// Positive sample images.
	std::vector<ofImage> negative_images_;	// Negative sample images.
	std::vector<int> positives_;			// Positive Samples (image id).
	std::vector<int> negatives_;			// Negative Samples (image id).

	//-----------------------------------------
	// Others.
	int epoch_;								// The number of current search iteration.
	bool isSearchedAll_;					// Flag: Finished searching.
	bool isFinishedInitSet_;				// Flag: Finished initial settings.
	bool canSearch_;						// Flag: Ready to search.
	float process_time_;					// Total processing time.
	float timer_start_;						// Timer (start).
	int len_current_showlist_;				// The number of current shown images.
	float total_search_time_;				// Total search time until search target is found.
	float total_search_time_start_;			// Timer (start).
	int selection_count_;					// Count user selections.
	bool isWroteTestResult_;				// Flag: Have wrote tese results.

	//-----------------------------------------
	// Pause.
	const float pause_time_ = 1.0f;			// Pause drawing.
	float pause_timer_start_;				// Pause timer start.

	//-----------------------------------------
	// Retrieval results.
	int active_size_;						// The number of displayed images (active selection).
	int search_window_size_;				// Search window size
	int show_size_;							// Size of showing results.
	std::vector<int> number_active_;		// Active selection results.
	std::vector<int> number_origin_;		// Retrieval results (comparison: images' ids).
	std::vector<int> number_rerank_;		// Reranked results.
	std::vector<int> number_visualrank_;	// Visualrank reranking results.
	std::vector<int> showList_active_;		// Selection image (active selection: show list).
	std::vector<int> showList_origin_;	 	// Retrieval results (compatison: show list).
	std::vector<int> showList_rerank_;	 	// Reranked results (show list).
	std::vector<int> showList_visualrank_; 	// Visualrank reranking results (show list).
	bool isactive_;							// Switch: Display active selection results.
	bool isorigin_;							// Switch: Display initial results.
	bool isrerank_;							// Switch: Display reranked results.
	bool isvisualrank_;						// Switch: Display visualrank results.

	//-----------------------------------------
	// Scroll Bar..
	VerticalScrollBar vscroll_areaA_;		// ScrollBar: All results viewer.
	VerticalScrollBar vscroll_areaP_;		// ScrollBar: Positive sample viewer.
	VerticalScrollBar vscroll_areaN_;		// ScrollBar: Negative sample viewer.
	const int ScrollBarWidth_ = 30;			// ScrollBar width.
	int drawHeight_areaA_;					// Scrollable area height.
	int drawHeight_areaP_;					// Scrollable area height.
	int drawHeight_areaN_;					// Scrollable area height.
	int scroll_areaA_;						// Current scroll position.
	int scroll_areaP_;						// Current scroll position.
	int scroll_areaN_;						// Current scroll position.


#ifdef OPENUSE
public:
	const int result_button_width_ = 90;
	const int selection_button_width_ = result_button_width_;
	const int result_button_posx_ = searchbutton_posx_ + searchbutton_width_ + 5;
	const int selection_button_posx_ = result_button_posx_ + selection_button_width_ + 5;
	ofImage result_button1_;
	ofImage result_button2_;
	ofImage selection_button1_;
	ofImage selection_button2_;
	bool isresult_;
#endif


public:
	ofApp(const char* arg1, const char* arg2, const char* arg3);


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
	void initparam();
	void calculate();
	void onPaint(const std::vector<int>& list);
	void inputHistory();
	void sizeChanged();
	bool isReleasedArea(float x, float y, float w, float h);
	bool isInsideDragingArea(float x, float y, float w, float h);
	bool isInsideMouseoverArea(float x, float y, float w, float h);
	bool isFileexists(const std::string& filepath);
	void writelog();
	void loadImageandFont();
	void updateScrollBars();
	void initializeBars();
	void calculateHoldingOriginPoint();
	void showProcessingTime();
	void autoselect_negative();
	void update_overview_info();
	void logdir_name();


public:
	bool vector_finder(const std::vector<int>& vector, const int number);
	void put_time(std::string& time_str);


public:
	ofxUIScrollableCanvas* gui_;
	void guiSetup();
	void guiEvent(ofxUIEventArgs& e);
	void exit();


public:
	ofTrueTypeFont font_;
	ofImage searchbutton1_;
	ofImage searchbutton2_;
	ofImage buttonA1_;
	ofImage buttonA2_;
	ofImage buttonB1_;
	ofImage buttonB2_;
	ofImage buttonC1_;
	ofImage buttonC2_;
	ofImage buttonD1_;
	ofImage buttonD2_;
	ofImage judged_img_;


public:
	DataBase* database_;						// Database information.
	NowLoading* loading_;						// Load Image features.
	ImageLoader* loader_; 						// Load Images
	Search* search_;							// Search image from database.
	SampleWriter* samplewriter_;				// Training sample writer
	Logger* logger_active_;						// Logger (active selection).
	Logger* logger_origin_;						// Logger (original).
	Logger* logger_rerank_;						// Logger (rerank).
	Logger* logger_visualrank_;					// Logger (visualrank).
	Trainer* trainer_;							// Online Trainer.
	Selection* selection_;						// Active Selection.
	ReRank* rerank_;							// Reranking method.
	VisualRank* visualrank_;					// VisualRank method.
	SingleSearchEvaluater* single_evaluater_;	// Evaluater.
	Rocchio* rocchio_init_;						// Rocchio algorithm (original).
	Rocchio* rocchio_new_;						// Rocchio algorithm (new feature).
	TestWriter* test_writer_;					// Writeout test log.
};


