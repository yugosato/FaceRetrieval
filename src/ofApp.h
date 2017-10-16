#pragma once


// -------- DATABASE -------- //
//#define CFD
//#define LFW
#define CFD_LFW
// -------- FEATURES -------- //
#define VGG
//#define HISTOGRAM
//#define GABOR
//#define HISTOGRAM_GABOR
// --------- RERANK --------- //
//#define VISUALRANK
// --------- DESIGN --------- //
#define OPENUSE
// -------------------------- //


#include <iostream>
#include <vector>
#include <string>
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
#include "util.h"


// openFrameworks base class.
class ofApp: public ofBaseApp
{
public:
	//-----------------------------------------
	// Startup arguments.
	std::string subjectname_;
	int searchTarget_;
	std::string selection_method_;

	//-----------------------------------------
	// Window size.
	const int initWidth_ = 1600;
	const int initHeight_ = 920;
	int windowWidth_;
	int windowHeight_;

	// Display Settings (Result window).
	const int leftsize_ = 1000;
	const int uppermargin_ = 20;
	const int uppersize_ = uppermargin_ + 40;
	const int button_height_ = 30;
	const int button_width_ = button_height_;
	const int buttonposy_line1_ = uppermargin_ + 5;
	const int searchbutton_posx_ = 1000;
	const int fontposy_top_ = + uppermargin_ + 27;
#ifndef OPENUSE
	const int searchbutton_width_ = 90;
	const int buttonA_posx_ = searchbutton_posx_ + searchbutton_width_ + 5;
	const int buttonB_posx_ = buttonA_posx_ + button_width_ + 5;
	const int buttonC_posx_ = buttonB_posx_ + button_width_ + 5;
	const int buttonD_posx_ = buttonC_posx_ + button_width_ + 5;
#else
	const int searchbutton_width_ = button_width_;
	const int result_button_width_ = 90;
	const int selection_button_width_ = result_button_width_;
	const int result_button_posx_ = searchbutton_posx_ + searchbutton_width_ + 5;
	const int selection_button_posx_ = result_button_posx_ + selection_button_width_ + 5;
#endif
	int colShow_;
	int rowShow_;
	int d_size_;
	int area_width_;
	int area_height_;

	// Display Settings (positives & negatives).
	const float holdImg_scale_ = 0.8;
	int perHeight_;
	int overview_colShow_;
	int overviewP_rowShow_;
	int overviewN_rowShow_;
	int overview_areamargin_;
	int overview_d_size_;
	int positive_txt_posy_;
	int negative_txt_posy_;
	int propose_txt_posx_;
	int propose_txt_posy_;
	int overviewP_areaposy_;
	int overviewN_areaposy_;
	int overview_areawidth_;
	int overview_areawidth_wide_;
	int overview_areaheight_;
	int propose_imgsize_;
	int propose_img_posx_;
	int propose_img_posy_;

	//-----------------------------------------
	// Mouse moving & holding.
	int clickx_;
	int clicky_;
	int dragx_;
	int dragy_;
	int mouseoverx_;
	int mouseovery_;
	int mouseover_;
	int holdImgNum_;
	int holding_x_;
	int holding_y_;

	// Flags.
	bool click_;
	bool leftsideclick_;
	bool isHolding_areaA_;
	bool isHolding_areaP_;
	bool isHolding_areaN_;
	bool isHoldAndDrag_;
	bool isInsideWindow_;
	bool isInside_areaA_;
	bool isInside_areaP_;
	bool isInside_areaN_;
	bool isInside_propose_;

	//-----------------------------------------
	// Path Settings.
	std::string binData_;
	std::string datasetdir_;
	std::string nameFile_;
	std::string featuresfile_;
	std::string npyFile_;
	std::string indexFile_;

	// Log Settings.
	std::string logdir_;
	std::string candidatefile_active_;
	std::string candidatefile_origin_;
	std::string candidatefile_rerank_;
	std::string candidatefile_visualrank_;
	std::string init_candidatefile_;
	std::string evaluationfile_;
	std::string testsettingfile_;

	// Python Settings.
	std::string pysettingfile_;
	std::string samplefile_;
	std::string scriptfile_;
	std::string positiveIndexfile_;
	std::string negativeIndexfile_;
	std::string activeIndexfile_;
	std::string cueflikIndexfile_;
	std::string randomIndexfile_;
	std::string resultGraphfile_;
	std::string new_featuresfile_;

	//-----------------------------------------
	// Font.
	const int fontsize_ = 16;
	std::string ttf_;

	//-----------------------------------------
	// Others.
	int epoch_;
	bool isFinishedInitSet_;
	bool canSearch_;
	bool isWroteTestResult_;
	int len_current_showlist_;
	int selection_count_;

	//-----------------------------------------
	// Positive & Negative.
	int len_positives_;
	int len_negatives_;
	std::vector<ofImage> positive_images_;
	std::vector<ofImage> negative_images_;
	std::vector<int> positives_;
	std::vector<int> negatives_;

	//-----------------------------------------
	// Retrieval results.
	int search_window_size_;
	int show_size_;
	std::vector<int> number_active_;
	std::vector<int> number_origin_;
	std::vector<int> number_rerank_;
#ifdef VISUALRANK
	std::vector<int> number_visusalrank_;
#endif
	std::vector<int> showList_active_;
	std::vector<int> showList_origin_;
	std::vector<int> showList_rerank_;
#ifdef VISUALRANK
	std::vector<int> showList_visualrank_;
#endif

	// Flags.
	bool isSearchedAll_;
	bool isactive_;
#ifndef OPENUSE
	bool isorigin_;
	bool isrerank_;
	bool isvisualrank_;
#else
	bool isresult_;
#endif

	//-----------------------------------------
	// Search target judge.
	bool isJudgeTrue_;
	bool isJudgeFalse_;

	//-----------------------------------------
	// Timer.
	const float pause_time_ = 1.0f;
	float pause_timer_start_;
	float total_search_time_;
	float search_timer_start_;

	//-----------------------------------------
	// Scroll Bar..
	const int ScrollBarWidth_ = 30;
	int drawHeight_areaA_;
	int drawHeight_areaP_;
	int drawHeight_areaN_;
	int scroll_areaA_;
	int scroll_areaP_;
	int scroll_areaN_;
	VerticalScrollBar vscroll_areaA_;
	VerticalScrollBar vscroll_areaP_;
	VerticalScrollBar vscroll_areaN_;


public:
	ofApp(const char* arg1, const char* arg2, const char* arg3);
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
	void guiEvent(ofxUIEventArgs& e);
	void exit();


public:
	void guiSetup();
	void initparam();
	void calculate();
	void onPaint(const std::vector<int>& list);
	void sizeChanged();
	void writelog();
	void loadImageandFont();
	void updateScrollBars();
	void initializeBars();
	void calculateHoldingOriginPoint();
	void showProcessingTime();
	void autoselect_negative();
	void update_overview_info();
	void logdir_name();
	bool isReleasedArea(float x, float y, float w, float h);
	bool isInsideDragingArea(float x, float y, float w, float h);
	bool isInsideMouseoverArea(float x, float y, float w, float h);
	bool isFileexists(const std::string& filepath);


public:
	ofxUIScrollableCanvas* gui_;
	ofTrueTypeFont font_;
	ofImage searchbutton1_;
	ofImage searchbutton2_;
#ifndef OPENUSE
	ofImage buttonA1_;
	ofImage buttonA2_;
	ofImage buttonB1_;
	ofImage buttonB2_;
	ofImage buttonC1_;
	ofImage buttonC2_;
	ofImage buttonD1_;
	ofImage buttonD2_;
#else
	ofImage result_button1_;
	ofImage result_button2_;
	ofImage selection_button1_;
	ofImage selection_button2_;
#endif
	ofImage judged_img_;


public:
	DataBase* database_;
	NowLoading* loading_;
	ImageLoader* loader_;
	Search* search_;
	SampleWriter* samplewriter_;
	Logger* logger_active_;
	Logger* logger_origin_;
	Logger* logger_rerank_;
#ifdef VISUALRANK
	Logger* logger_visualrank_;
#endif
	Trainer* trainer_;
	Selection* selection_;
	ReRank* rerank_;
#ifdef VISUALRANK
	VisualRank* visualrank_;
#endif
	SingleSearchEvaluater* single_evaluater_;
	Rocchio* rocchio_init_;
	Rocchio* rocchio_new_;
	TestWriter* test_writer_;
};
