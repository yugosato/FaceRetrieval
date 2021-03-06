#include "ofApp.h"


//--------------------------------------------------------------
ofApp::ofApp(const char* arg1, const char* arg2, const char* arg3)
{
	subjectname_ = ofToString(arg1);
	searchTarget_ = ofToInt(arg2);
	selection_method_ = ofToString(arg3);
}

//--------------------------------------------------------------
void ofApp::initparam()
{
	//-----------------------------------------
	// Window size.
	windowWidth_ = initWidth_;
	windowHeight_ = initHeight_;

	// Display Settings (Result window).
	colShow_ = 5;
	d_size_ = (initWidth_ - leftsize_ - 2 * ScrollBarWidth_) / colShow_;
	area_width_ = d_size_ * colShow_;
	area_height_ = d_size_ * colShow_;

	// Display Settings (positives & negatives).
	perHeight_ = (windowHeight_ - 2 * uppersize_) / 2 - 20;
	overview_colShow_ = 15;
	overview_areamargin_ = ScrollBarWidth_;
	overview_d_size_ = (leftsize_ - 3 * overview_areamargin_) / overview_colShow_;
	positive_txt_posy_ = fontposy_top_;
	negative_txt_posy_ = positive_txt_posy_ + perHeight_ + uppersize_;
	overviewP_areaposy_ = positive_txt_posy_ + 10;
	overviewN_areaposy_ = negative_txt_posy_ + 10;
	overview_areawidth_ = overview_d_size_ * overview_colShow_;
	overview_areawidth_wide_ = windowWidth_ - 2 * overview_areamargin_;
	overview_areaheight_ = perHeight_;
	propose_txt_posy_ = 2 * uppersize_ + area_height_ - 15;
	propose_imgsize_ = (overviewN_areaposy_ + overview_areaheight_) - (2 * uppersize_ + area_height_);
	propose_img_posx_ = leftsize_ + (area_width_ - propose_imgsize_) / 2;
	propose_img_posy_ = propose_txt_posy_ + 10;

	//-----------------------------------------
	// Mouse moving & holding.
	clickx_ = -1;
	clicky_ = -1;
	dragx_ = -1;
	dragy_= -1;
	mouseoverx_ = -1;
	mouseovery_ = -1;
	mouseover_ = -1;
	holdImgNum_ = -1;
	holding_x_ = -1;
	holding_y_ = -1;

	// Flags.
	click_ = false;
	leftsideclick_ = false;
	isHolding_areaA_ = false;
	isHolding_areaP_ = false;
	isHolding_areaN_ = false;
	isHoldAndDrag_ = false;
	isInsideWindow_ = false;
	isInside_areaA_ = false;
	isInside_areaP_ = false;
	isInside_areaN_ = false;
	isInside_propose_ = false;

	//-----------------------------------------
	// Path Settings.
	binData_ = "/home/yugo/workspace/Interface/bin/data";
	datasetdir_ = "/home/yugo/Desktop/dataset";
#ifdef LFW
	nameFile_ = datasetdir_ + "/lfw-cropped/images_selected.txt";
	featuresfile_ = binData_ + "/lfw/lfw-vgg.tsv";
	npyFile_ = binData_ + "/lfw/lfw-vgg.npy";
	indexFile_ = binData_ + "/lfw/lfw-vgg_index-angle";
	init_candidatefile_ = binData_ + "/lfw/initialize.txt";
#endif
#ifdef CFD
	nameFile_ = datasetdir_ + "/cfd-cropped/images_selected.txt";
	featuresfile_ = binData_ + "/cfd/cfd-vgg.tsv";
	npyFile_ = binData_ + "/cfd/cfd-vgg.npy";
	indexFile_ = binData_ + "/cfd/cfd-vgg_index-angle";
	init_candidatefile_ = binData_ + "/cfd/initialize.txt";
#endif
#ifdef MIX
	nameFile_ = datasetdir_ + "/images_selected.txt";
	featuresfile_ = binData_ + "/mix/mix-vgg.tsv";
	npyFile_ = binData_ + "/mix/mix-vgg.npy";
	indexFile_ = binData_ + "/mix/mix-vgg_index-angle";
	init_candidatefile_ = binData_ + "/mix/initialize.txt";
#endif

	// Log Settings.
	logdir_name();
	candidatefile_active_ = logdir_ + "/active_selection.txt";
	candidatefile_origin_ = logdir_ + "/original_rank.txt";
	candidatefile_rerank_ = logdir_ + "/rerank.txt";
	candidatefile_visualrank_ = logdir_ + "/visualrank.txt";
	evaluationfile_ = logdir_ + "/distance.csv";
	testsettingfile_ = logdir_ + "/test.txt";
	activelogfile_ = logdir_ + "/log.txt";

	// Python Settings.
	pysettingfile_ = binData_ + "/py_setting.txt";
	samplefile_ = logdir_ + "/feedback.txt";
	scriptfile_ = "/home/yugo/workspace/Interface/trainer/main_process.py";
	positiveIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/positive_index.txt";
	negativeIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/negative_index.txt";
	activeIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/uncertain_index.txt";
	cueflikIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/cueflik_index.txt";
	randomIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/random_index.txt";
	resultGraphfile_ = "/home/yugo/workspace/Interface/trainer/result/acc_val.png";
	new_featuresfile_ = "/home/yugo/workspace/Interface/trainer/result/features.tsv";

	//-----------------------------------------
	// Font.
	ttf_ = binData_ + "/fonts/RictyDiminished-Bold.ttf";

	//-----------------------------------------
	// Others.
	if (selection_method_ == "random" || selection_method_ == "comparison")
		isMainMethod_ = false;
	else
		isMainMethod_ = true;
	isFinishedInitSet_ = false;
	canSearch_ = false;
	isWroteTestResult_ = false;
	showTarget_ = false;
	epoch_ = 0;
	len_current_showlist_ = show_size_;
	selection_count_ = 0;

	//-----------------------------------------
	// Positive & Negative.
	len_positives_ = 0;
	len_negatives_ = 0;

	//-----------------------------------------
	// Retrieval results.
#ifdef CFD
	search_window_size_ = 50;
#endif
#ifdef LFW
	search_window_size_ = 100;
#endif
#ifdef CFD_LFW
	search_window_size_ = 100;
#endif
	show_size_ = 25;
	active_size_ = 25;

	// Flags.
	isSearchedAll_ = false;
	isactive_ = true;
#ifndef OPENUSE
	isorigin_ = false;
	isrerank_ = false;
#ifdef VISUALRANK
	isvisualrank_ = false;
#endif
#else
	isresult_ = false;
#endif

	//-----------------------------------------
	// Search target judge.
	isJudgeTrue_ = false;
	isJudgeFalse_ = false;

	//-----------------------------------------
	// Rocchio algorithm parameters.
	alpha_ = 1.0f;
	beta_ = 0.8f;
	gamma_ = 0.1f;

	//-----------------------------------------
	// Timer.
	total_search_time_ = 0.0f;

	//-----------------------------------------
	// Scroll Bar.
	change_by_button_ = d_size_ / 3;
	change_by_button_overview_ = overview_d_size_ / 3;
}

//--------------------------------------------------------------
void ofApp::loadImageandFont()
{
	font_.load(ttf_, fontsize_);

#ifndef OPENUSE
	searchbutton1_.load(binData_ + "/items/search1.png");
	searchbutton2_.load(binData_ + "/items/search2.png");

	buttonA1_.load(binData_ + "/items/A1.png");
	buttonA2_.load(binData_ + "/items/A2.png");
	buttonB1_.load(binData_ + "/items/B1.png");
	buttonB2_.load(binData_ + "/items/B2.png");
	buttonC1_.load(binData_ + "/items/C1.png");
	buttonC2_.load(binData_ + "/items/C2.png");
#ifdef VISUALRANK
	buttonD1_.load(binData_ + "/items/D1.png");
	buttonD2_.load(binData_ + "/items/D2.png");
#endif
#else
	searchbutton1_.load(binData_ + "/items/search_open1.png");
	searchbutton2_.load(binData_ + "/items/search_open2.png");

	result_button1_.load(binData_ + "/items/result1.png");
	result_button2_.load(binData_ + "/items/result2.png");

	selection_button1_.load(binData_ + "/items/selection1.png");
	selection_button2_.load(binData_ + "/items/selection2.png");
#endif
}

//--------------------------------------------------------------
bool ofApp::isFileexists(const std::string& filepath)
{
	struct stat st;
	int ret = stat(filepath.c_str(), &st);
	if (ret == 0)
		return true;
	else
		return false;
}

//--------------------------------------------------------------
void ofApp::setup()
{
	// Initialize.
	initparam();
	ofSetWindowShape(windowWidth_, windowHeight_);
	guiSetup();
	loadImageandFont();
	mkdir(logdir_.c_str(), 0777);

	// Get Database information.
	database_ = new DataBase();
	database_->setup(searchTarget_, active_size_, nameFile_, init_candidatefile_);
	database_->initialize("random");

	if (searchTarget_ > database_->row_ - 1)
	{
		std::cerr << "[Warning] Specified search target is over database range." << std::endl;
		abort();
	}

	// Initialize scroll bar.
	initializeBars();

	// Load images.
	loader_ = new ImageLoader();
	loader_->setRow(database_->row_);
	loader_->setName(database_->name_);

	calculate();
	onPaint(showList_active_);

	// Setup search method.
	search_ = new Search();
	search_->setup(indexFile_, search_window_size_);

	// Load image features (multi thread).
	loading_ = new NowLoading();
	loading_->set_featuresfile(featuresfile_, new_featuresfile_);
	loading_->setRow(database_->row_);
	loading_->startThread();

	// Setup writer.
	samplewriter_ = new SampleWriter(samplefile_);
	samplewriter_->init_write();

	// Setup rocchio algorithm.
	rocchio_init_ = new Rocchio;
	rocchio_new_ = new Rocchio;

	// Setup online trainer.
	trainer_ = new Trainer;
	trainer_->setup(scriptfile_);

	// Setup active selection.
	selection_ = new Selection;
	selection_->setup(activeIndexfile_, cueflikIndexfile_, randomIndexfile_);
	selection_->set_searchTarget(searchTarget_);
	selection_->set_active_size(active_size_);
	selection_->set_row(database_->row_);
	selection_->set_method(selection_method_);

	// Setup reranking method.
	rerank_ = new ReRank;
#ifdef VISUALRANK
	visualrank_ = new VisualRank;
#endif

	// Setup evaluater.
	single_evaluater_ = new SingleSearchEvaluater;
	single_evaluater_->setup(searchTarget_, evaluationfile_);

	// Setup test writer.
	test_writer_ = new TestWriter;
	test_writer_->setup(testsettingfile_);
	test_writer_->settings(selection_->searchTarget_, selection_->method_);

	// Setup active selection logger.
	activelog_ = new ActiveLog;
	activelog_->setup(activelogfile_);

	std::cout << "[Setting] NGT-index: \"" << indexFile_ << "\"" << std::endl;
	std::cout << "[Setting] Matrix file: \"" << featuresfile_ << "\"" << std::endl;
	std::cout << "[Setting] Npy file: \"" << npyFile_ << "\"" << std::endl;
	std::cout << "[Setting] Feedback file: \"" << samplefile_ << "\"" << std::endl;
	std::cout << "[Setting] Python settings: \"" << pysettingfile_ << "\"" << std::endl;
	std::cout << "[Setting] Database size: " << database_->row_ << std::endl;
}

//--------------------------------------------------------------
void ofApp::guiSetup()
{
	gui_ = new ofxUIScrollableCanvas(0, 0, windowWidth_, windowHeight_);
	gui_->setScrollableDirections(false, true);
	gui_->setColorBack(ofColor(0.0f, 0.0f, 0.0f, 0.0f));
	std::string title = "Subject: " + subjectname_ + "   Target: " + ofToString(searchTarget_)
			+ "   Method: " + selection_method_ + "   Log: " + logdir_;
	ofSetWindowTitle(title);
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs& e)
{

}

//--------------------------------------------------------------
void ofApp::exit()
{
	delete gui_;
	delete database_;
	delete loading_;
	delete loader_;
	delete search_;
	delete samplewriter_;
	delete logger_active_;
	delete logger_origin_;
	delete logger_rerank_;
	delete trainer_;
	delete selection_;
	delete rerank_;
	delete single_evaluater_;
	delete rocchio_init_;
	delete rocchio_new_;
	delete test_writer_;
	delete activelog_;
#ifdef VISUALRANK
	delete logger_visualrank_;
	delete visualrank_;
#endif
}

//--------------------------------------------------------------

void ofApp::update()
{
	ofBackground(ofColor(0x000000));

	if (loading_->isLoaded_init_)
	{
		loading_->stopThread();
		loading_->isLoaded_init_ = false;

		logger_active_ = new Logger;
		logger_origin_ = new Logger;
		logger_rerank_ = new Logger;
#ifdef VISUALRANK
		logger_visualrank_ = new Logger;
#endif
		logger_active_->setup(samplefile_, candidatefile_active_, pysettingfile_, npyFile_, loading_->col_);
		logger_origin_->setup(samplefile_, candidatefile_origin_, pysettingfile_, npyFile_, loading_->col_);
		logger_rerank_->setup(samplefile_, candidatefile_rerank_, pysettingfile_, npyFile_, loading_->col_);
#ifdef VISUALRANK
		logger_visualrank_->setup(samplefile_, candidatefile_visualrank_, pysettingfile_, npyFile_, loading_->col_);
#endif
		logger_active_->writePySetting();

		single_evaluater_->set_features(loading_->features_);

		number_active_ = database_->number_active_;
		number_origin_ = database_->number_origin_;
		number_rerank_ = database_->number_rerank_;
#ifdef VISUALRANK
		number_visualrank_ = database_->number_visualrank_;
#endif
		writelog();

		isFinishedInitSet_ = true;
		canSearch_ = true;
		search_timer_start_ = ofGetElapsedTimef();
	}

	if (search_->isSearched_)
	{
		search_->stopThread();
		search_->getNumber(&number_origin_);
		search_->isSearched_ = false;

		// Run Trainer.
		samplewriter_->write(positives_, negatives_, number_origin_);
		trainer_->startThread();
	}

	if (trainer_->isTrained_)
	{
		trainer_->stopThread();
		trainer_->isTrained_ = false;

		// Concatenate vector
		std::vector<int> new_index;
		std::copy(number_origin_.begin(), number_origin_.end(),std::back_inserter(new_index));
		std::copy(positives_.begin(), positives_.end(),std::back_inserter(new_index));
		std::copy(negatives_.begin(), negatives_.end(),std::back_inserter(new_index));

		loading_->isLoadNew_ = true;
		loading_->set_new_index(new_index);
		loading_->startThread();
	}

	if (loading_->isLoaded_new_)
	{
		loading_->stopThread();
		loading_->isLoaded_new_ = false;

		// New query vector (new features).
		rocchio_new_->set_features(loading_->new_features_);
		rocchio_new_->setInput_multi(positives_, negatives_);
		rocchio_new_->set_weight(alpha_, beta_, gamma_);
		rocchio_new_->run();

		// Reranking by new features query vector.
		rerank_->set_features(loading_->new_features_);
		rerank_->set_init_result(number_origin_);
		rerank_->set_queryvector(rocchio_new_->queryvector_);
		rerank_->startThread();
	}

	if (rerank_->isReranked_)
	{
		rerank_->stopThread();
		rerank_->getNumber(&number_rerank_);
		rerank_->isReranked_ = false;

#ifdef VISUALRANK
		// VisualRank Reranking.
		visualrank_->set_features(loading_->features_);
		visualrank_->set_init_result(number_origin_);
		visualrank_->startThread();
	}

	if (visualrank_->isReranked_)
	{
		visualrank_->stopThread();
		visualrank_->isReranked_ = false;
		visualrank_->getNumber(&number_visualrank_);
#endif
		isSearchedAll_ = true;
	}

	if (isSearchedAll_)
	{
		isSearchedAll_ = false;

		// Single Search Evaluation.
		single_evaluater_->set_inputpoint(rocchio_init_->queryvector_);
		if (!isMainMethod_)
			single_evaluater_->set_results(number_origin_);
		else
			single_evaluater_->set_results(number_rerank_);
		single_evaluater_->run();

		// Active selection.
		show_size_ = search_window_size_;
		if (!isMainMethod_)
		{
			active_size_ = search_window_size_;
			selection_->set_active_size(active_size_);
		}

		selection_->set_result(number_origin_, number_rerank_);
		selection_->load();
		selection_->getNumber(&number_active_);

		database_->setNumber_active(number_active_);
		database_->setNumber_origin(number_origin_);
		database_->setNumber_rerank(number_rerank_);
#ifdef VISUALRANK
		database_->setNumber_visualrank(number_visualrank_);
#endif

		isactive_ = false;
#ifndef OPENUSE
		isactive_ = true;
		isorigin_ = false;
		isrerank_ = false;
#ifdef VISUALRANK
		isvisualrank_ = false;
#endif
#else
		isresult_ = true;
#endif

		calculate();

#ifndef OPENUSE
		onPaint(showList_active_);
#else
		if (!isMainMethod_)
			onPaint(showList_origin_);
		else
			onPaint(showList_rerank_);
#endif

		canSearch_ = true;
		search_timer_start_ = ofGetElapsedTimef();
		showProcessingTime();
		std::cout << "#####################################################################################################" << std::endl;
		vscroll_areaA_.current(0);
	}

	// Pause timer.
	if (isJudgeFalse_)
	{
		float pause_timer = ofGetElapsedTimef() - pause_timer_start_;
		if (pause_time_ < pause_timer)
			isJudgeFalse_ = false;
	}

	if (!isWroteTestResult_ && isJudgeTrue_)
	{
		total_search_time_ += ofGetElapsedTimef() - search_timer_start_;
		test_writer_->target_found(epoch_, total_search_time_, selection_count_);
		isWroteTestResult_ = true;
		std::cout << "[ofApp] Search target was found!" << std::endl;
		std::cout << "[ofApp] Total search time (remove back process): " << total_search_time_ << std::endl;
	}

	// Scroll Bar.
	vscroll_areaA_.update();
	vscroll_areaP_.update();
	vscroll_areaN_.update();

	// Timer
	update_overview_info();
	updateScrollBars();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofFill();
	ofSetLineWidth(1);

	scroll_areaA_ = - vscroll_areaA_.current();
	scroll_areaP_ = - vscroll_areaP_.current();
	scroll_areaN_ = - vscroll_areaN_.current();

	ofImage img;
	const int len = loader_->row_;

	for (int i = 0; i < len; ++i)
	{
		const int j = i % colShow_;
		const int k = i / colShow_;

		int drawx = leftsize_ + d_size_ * j;
		int drawy = uppersize_ + d_size_ * k + scroll_areaA_;

		if (uppersize_ + area_height_ < drawy)
			break;
		else if (0 > d_size_ * (k + 1) + scroll_areaA_)
			continue;

		int margin = 5;
		int imgId = loader_->showList_[i];
		img = loader_->picture_[i];

		if (!img.isAllocated())
			break;
		else if (isactive_)
		{
			bool exist_positive = vector_finder(positives_, imgId);
			bool exist_negative = vector_finder(negatives_, imgId);

			if ((isHolding_areaA_ && i == holdImgNum_) || exist_positive || exist_negative)
			{
				if (exist_positive)
				{
					ofNoFill();
					ofSetLineWidth(5);
					ofSetColor(ofColor(255.0f, 0.0f, 0.0f, 255.0f));
					ofDrawRectangle(drawx + margin, drawy + margin, d_size_ - 2*margin, d_size_ - 2*margin);
				}
				else if (exist_negative)
				{
					ofNoFill();
					ofSetLineWidth(5);
					ofSetColor(ofColor(0.0f, 0.0f, 255.0f, 255.0f));
					ofDrawRectangle(drawx + margin, drawy + margin, d_size_ - 2 * margin, d_size_ - 2 * margin);
				}

				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 130.0f));
				img.draw(drawx, drawy, d_size_, d_size_);
			}
			else
			{
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
				img.draw(drawx, drawy, d_size_, d_size_);
			}
		}
		else if (!isactive_)
		{
			if (isHolding_areaA_ && i == holdImgNum_)
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 130.0f));
			else
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
			img.draw(drawx, drawy, d_size_, d_size_);

			if (isJudgeTrue_ && imgId == searchTarget_)
			{
				ofNoFill();
				ofSetLineWidth(5);
				ofSetColor(ofColor(255.0f, 0.0f, 0.0f, 255.0f));
				ofDrawRectangle(drawx + margin, drawy + margin, d_size_ - 2*margin, d_size_ - 2*margin);
			}
		}
	}

	// Positive sample viewer.
	ofFill();
	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 130.0f));
	ofDrawRectangle(overview_areamargin_, overviewP_areaposy_, overview_areawidth_, overview_areaheight_);

	const int len_positive_images = positive_images_.size();
    if (len_positive_images > 0)
	{
		for (int i = 0; i < len_positive_images; ++i)
		{
			const int j = i % overview_colShow_;
			const int k = i / overview_colShow_;

			int drawx = overview_areamargin_ + overview_d_size_ * j;
			int drawy = overviewP_areaposy_ + overview_d_size_ * k + scroll_areaP_;

			if (overviewP_areaposy_ + overview_areaheight_ < drawy)
				break;
			else if (0 > overview_d_size_ * (k + 1) + scroll_areaP_)
				continue;

			ofImage img_positive = positive_images_[i];
			if (!img_positive.isAllocated())
				break;
			else
			{
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
				img_positive.draw(drawx, drawy, overview_d_size_, overview_d_size_);
			}
		}
	}

    // Negative sample viewer.
	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 130.0f));
	ofDrawRectangle(overview_areamargin_, overviewN_areaposy_, overview_areawidth_, overview_areaheight_);

	const int len_negative_images = negative_images_.size();
	if (len_negative_images > 0)
	{
		for (int i = 0; i < len_negative_images; ++i)
		{
			const int j = i % overview_colShow_;
			const int k = i / overview_colShow_;

			int drawx = overview_areamargin_ + overview_d_size_ * j;
			int drawy = overviewN_areaposy_ + overview_d_size_ * k + scroll_areaN_;

			if (overviewN_areaposy_ + overview_areaheight_ < drawy)
				break;
			else if (0 > overview_d_size_ * (k + 1) + scroll_areaN_)
				continue;

			ofImage img_negative = negative_images_[i];
			if (!img_negative.isAllocated())
				break;
			else
			{
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
				img_negative.draw(drawx, drawy, overview_d_size_, overview_d_size_);
			}
		}
	}

	//------------------------------------------------------------------------------
    // Dummy Rectangles.
	ofSetColor(ofColor(0.0f, 0.0f, 0.0f, 255.0f));
    ofDrawRectangle(0, 0, windowWidth_, uppersize_);
    ofDrawRectangle(0, overviewP_areaposy_ + overview_areaheight_, overview_areamargin_ + overview_areawidth_, overviewN_areaposy_ - (overviewP_areaposy_ + overview_areaheight_));
    ofDrawRectangle(0, overviewN_areaposy_ + overview_areaheight_, overview_areamargin_ + overview_areawidth_, windowHeight_ - (overviewN_areaposy_ + overview_areaheight_));
    ofDrawRectangle(leftsize_, uppersize_ + area_height_, windowWidth_ - leftsize_, windowHeight_ - (uppersize_ + area_height_));

	// Scroll Bar.
   	vscroll_areaA_.draw();
	vscroll_areaP_.draw();
	vscroll_areaN_.draw();

    // Buttons.
    ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
   	if (canSearch_)
   		searchbutton1_.draw(searchbutton_posx_, buttonposy_line1_, searchbutton_width_, button_height_);
   	else
   		searchbutton2_.draw(searchbutton_posx_, buttonposy_line1_, searchbutton_width_, button_height_);

	std::string text;
	if (isactive_)
	{
#ifndef OPENUSE
		buttonA1_.draw(buttonA_posx_, buttonposy_line1_, button_width_, button_height_);
		buttonB2_.draw(buttonB_posx_, buttonposy_line1_, button_width_, button_height_);
		buttonC2_.draw(buttonC_posx_, buttonposy_line1_, button_width_, button_height_);
#ifdef VISUALRANK
		buttonD2_.draw(buttonD_posx_, buttonposy_line1_, button_width_, button_height_);
#endif
		text = "Selection";
#else
		result_button2_.draw(result_button_posx_, buttonposy_line1_, result_button_width_, button_height_);
		selection_button1_.draw(selection_button_posx_, buttonposy_line1_, selection_button_width_, button_height_);
#endif
	}
#ifndef OPENUSE
	else if (isorigin_)
	{
		buttonA2_.draw(buttonA_posx_, buttonposy_line1_, button_width_, button_height_);
		buttonB1_.draw(buttonB_posx_, buttonposy_line1_, button_width_, button_height_);
		buttonC2_.draw(buttonC_posx_, buttonposy_line1_, button_width_, button_height_);
#ifdef VISUALRANK
		buttonD2_.draw(buttonD_posx_, buttonposy_line1_, button_width_, button_height_);
#endif
		text = "Original";
	}
	else if (isrerank_)
	{
		buttonA2_.draw(buttonA_posx_, buttonposy_line1_, button_width_, button_height_);
		buttonB2_.draw(buttonB_posx_, buttonposy_line1_, button_width_, button_height_);
		buttonC1_.draw(buttonC_posx_, buttonposy_line1_, button_width_, button_height_);
#ifdef VISUALRANK
		buttonD2_.draw(buttonD_posx_, buttonposy_line1_, button_width_, button_height_);
#endif
		text = "Rerank";
	}
#ifdef VISUALRANK
	else if (isvisualrank_)
	{
		buttonA2_.draw(buttonA_posx_, buttonposy_line1_, button_width_, button_height_);
		buttonB2_.draw(buttonB_posx_, buttonposy_line1_, button_width_, button_height_);
		buttonC2_.draw(buttonC_posx_, buttonposy_line1_, button_width_, button_height_);
		buttonD1_.draw(buttonD_posx_, buttonposy_line1_, button_width_, button_height_);
		text = "VisualRank";
	}
#endif
#else
	if (isresult_)
	{
		result_button1_.draw(result_button_posx_, buttonposy_line1_, result_button_width_, button_height_);
		selection_button2_.draw(selection_button_posx_, buttonposy_line1_, selection_button_width_, button_height_);
	}
#endif

	if (isFinishedInitSet_)
	{
		if (!canSearch_)
			text = "Searching...";
	}
	else
		text = "Please wait.";

	float width = font_.stringWidth(text);
	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
	font_.drawString(text, windowWidth_ - width - ScrollBarWidth_ - 35, fontposy_top_);

	std::string positive = "Positive Sample: ";
	std::string negative = "Negative Sample: ";

	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
    font_.drawString(positive + ofToString(len_positives_), overview_areamargin_, positive_txt_posy_);
	font_.drawString(negative + ofToString(len_negatives_), overview_areamargin_, negative_txt_posy_);
	//------------------------------------------------------------------------------

	// Proposed image.
	int margin = 10;
	std::string propose;

	if (isJudgeFalse_)
	{
		ofSetColor(ofColor(79.0f, 181.0f, 221.0f, 255.0f));
		ofDrawRectangle(propose_img_posx_, propose_img_posy_, propose_imgsize_, propose_imgsize_);
		ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
		judged_img_.draw(propose_img_posx_ + margin, propose_img_posy_ + margin, propose_imgsize_ - 2 * margin, propose_imgsize_ - 2 * margin);
		ofSetColor(ofColor(79.0f, 181.0f, 221.0f, 255.0f));
		propose = "False";
	}
	else if (!isJudgeTrue_)
	{
		ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
		ofDrawRectangle(propose_img_posx_, propose_img_posy_, propose_imgsize_, propose_imgsize_);
		ofSetColor(ofColor(128.0f, 128.0f, 128.0f, 255.0f));
		ofDrawRectangle(propose_img_posx_ + margin, propose_img_posy_ + margin, propose_imgsize_ - 2 * margin, propose_imgsize_ - 2 * margin);

		if (isactive_)
		{
			ofImage show_image;
			if (!isMainMethod_)
			{
				if (database_->toprank_img_origin_.isAllocated())
					show_image = database_->toprank_img_origin_;
			}
			else
			{
				if (database_->toprank_img_rerank_.isAllocated())
					show_image = database_->toprank_img_rerank_;
			}

			if (epoch_ > 0 && show_image.isAllocated())
			{
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
				show_image.draw(propose_img_posx_ + margin, propose_img_posy_ + margin, propose_imgsize_ - 2 * margin, propose_imgsize_ - 2 * margin);
			}
		}

		ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
		propose = "Is this photograph?";
	}
	else if (isJudgeTrue_)
	{
		ofSetColor(ofColor(255.0f, 255.0f, 0.0f, 255.0f));
		ofDrawRectangle(propose_img_posx_, propose_img_posy_, propose_imgsize_, propose_imgsize_);
		ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
		database_->target_img_.draw(propose_img_posx_ + margin, propose_img_posy_ + margin, propose_imgsize_ - 2 * margin, propose_imgsize_ - 2 * margin);
		ofSetColor(ofColor(255.0f, 255.0f, 0.0f, 255.0f));
		propose = "Exactly!";
	}

	if (showTarget_)
	{
		ofSetColor(ofColor(0.0f, 0.0f, 0.0f, 180.0f));
		ofDrawRectangle(leftsize_, uppersize_, area_width_, area_height_);

		ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
		ofDrawRectangle(propose_img_posx_, propose_img_posy_, propose_imgsize_, propose_imgsize_);
		database_->target_img_.draw(propose_img_posx_ + margin, propose_img_posy_ + margin, propose_imgsize_ - 2 * margin, propose_imgsize_ - 2 * margin);
		propose = "Target photograph";
	}

	int propose_txt_width = font_.stringWidth(propose);
	propose_txt_posx_ = leftsize_ + (area_width_ - propose_txt_width) / 2;
	font_.drawString(propose, propose_txt_posx_, propose_txt_posy_);

	// Holding image.
	if (isHoldAndDrag_)
	{
		ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
		ofImage holdImg;
		if (isHolding_areaA_)
			holdImg = loader_->picture_[holdImgNum_];
		else if (isHolding_areaP_)
			holdImg = positive_images_[holdImgNum_];
		else if (isHolding_areaN_)
			holdImg = negative_images_[holdImgNum_];

		if (holdImg.isAllocated())
			holdImg.draw(holding_x_, holding_y_, holdImg_scale_ * d_size_, holdImg_scale_ * d_size_);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	switch (key)
	{
		case 115: // Ctrl+S
		{
			ofImage img;
			img.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			std::string time;
			put_time(time);
			std::string savepath = logdir_ + "/" + time + ".jpeg";
			img.save(savepath);
			std::cout << "[ofApp] Saved snapshot image." << std::endl;
			std::cout << "[ofApp] --> \"" << savepath << "\"" << std::endl;
			break;
		}
		case 126: // Shift+0
		{
			if (!showTarget_)
				showTarget_ = true;
			else
				showTarget_ = false;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
	mouseoverx_ = x;
	mouseovery_ = y;

	int x_dash, y_dash;
	if (isInsideWindow_)
	{
		if (canSearch_)
		{
			// Area A.
			if (isInsideMouseoverArea(leftsize_, uppersize_, area_width_, area_height_))
			{
				isInside_areaA_ = true;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
				isInside_propose_ = false;
				x_dash = x - leftsize_;
				y_dash = y - scroll_areaA_ - uppersize_;
				mouseover_ = x_dash / d_size_ + y_dash / d_size_ * colShow_;
			}	// Area P.
			else if (isInsideMouseoverArea(overview_areamargin_, overviewP_areaposy_, overview_areawidth_, overview_areaheight_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = true;
				isInside_areaN_ = false;
				isInside_propose_ = false;
				x_dash = x - overview_areamargin_;
				y_dash = y - scroll_areaP_ - overviewP_areaposy_;
				mouseover_ = x_dash / overview_d_size_ + y_dash / overview_d_size_ * overview_colShow_;
			}	// Area N.
			else if (isInsideMouseoverArea(overview_areamargin_, overviewN_areaposy_, overview_areawidth_, overview_areaheight_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = true;
				isInside_propose_ = false;
				x_dash = x - overview_areamargin_;
				y_dash = y - scroll_areaN_ - overviewN_areaposy_;
				mouseover_ = x_dash / overview_d_size_ + y_dash / overview_d_size_ * overview_colShow_;
			}	// Propose area,
			else if (isInsideMouseoverArea(propose_img_posx_, propose_img_posy_, propose_imgsize_, propose_imgsize_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
				isInside_propose_ = true;
				mouseover_ = -1;
			}
			else
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
				isInside_propose_ = false;
				mouseover_ = -1;
			}
		}
		else
		{
			isInside_areaA_ = false;
			isInside_areaP_ = false;
			isInside_areaN_ = false;
			isInside_propose_ = false;
			mouseover_ = -1;
		}
	}
	else
	{
		isInside_areaA_ = false;
		isInside_areaP_ = false;
		isInside_areaN_ = false;
		isInside_propose_ = false;
		mouseover_ = -1;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	dragx_ = x;
	dragy_ = y;
	click_ = false;

	if (vscroll_areaA_.mouseDragged(x, y))
		return;

	if (vscroll_areaP_.mouseDragged(x, y ))
		return;

	if (vscroll_areaN_.mouseDragged(x, y ))
		return;

	if (isInsideWindow_)
	{
		if (canSearch_ && mouseover_ >= 0)
		{
			if (isHolding_areaA_)
			{
				int imgId = showList_active_[mouseover_];
				bool exist_positive = vector_finder(positives_, imgId);
				bool exist_negative = vector_finder(negatives_, imgId);

				if (isactive_)
				{
					if (!exist_positive && !exist_negative)
					{
						holdImgNum_ = mouseover_;
						isHoldAndDrag_ = true;
						calculateHoldingOriginPoint();
					}
				}
				else
				{
					holdImgNum_ = mouseover_;
					isHoldAndDrag_ = true;
					calculateHoldingOriginPoint();
				}
			}
			else if (isHolding_areaP_)
			{
				int imgId = positives_[mouseover_];
				bool exist_negative = vector_finder(negatives_, imgId);

				if (!exist_negative)
				{
					holdImgNum_ = mouseover_;
					isHoldAndDrag_ = true;
					calculateHoldingOriginPoint();
				}
			}
			else if (isHolding_areaN_)
			{
				int imgId = negatives_[mouseover_];
				bool exist_positive = vector_finder(positives_, imgId);

				if (!exist_positive)
				{
					holdImgNum_ = mouseover_;
					isHoldAndDrag_ = true;
					calculateHoldingOriginPoint();
				}
			}

			// Area A.
			if (isInsideDragingArea(leftsize_, uppersize_, area_width_, area_height_))
			{
				isInside_areaA_ = true;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
				isInside_propose_ = false;
			}	// Area P.
			else if (isInsideDragingArea(overview_areamargin_, overviewP_areaposy_, overview_areawidth_, overview_areaheight_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = true;
				isInside_areaN_ = false;
				isInside_propose_ = false;
			}	// Area N.
			else if (isInsideDragingArea(overview_areamargin_, overviewN_areaposy_, overview_areawidth_, overview_areaheight_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = true;
				isInside_propose_ = false;
			}	// Propose area.
			else if (isInsideDragingArea(propose_img_posx_, propose_img_posy_, propose_imgsize_, propose_imgsize_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
				isInside_propose_ = true;
			}
			else
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
				isInside_propose_ = false;
			}
		}
		else
		{
			isInside_areaA_ = false;
			isInside_areaP_ = false;
			isInside_areaN_ = false;
			isInside_propose_ = false;
		}
	}
	else
	{
		isInside_areaA_ = false;
		isInside_areaP_ = false;
		isInside_areaN_ = false;
		isInside_propose_ = false;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	clickx_ = x;
	clicky_ = y;
	click_ = true;
	holdImgNum_ = -1;

	if (clickx_ < leftsize_)
		leftsideclick_ = true;
	else
		leftsideclick_ = false;

	if (vscroll_areaA_.mousePressed(clickx_, clicky_))
		return;

	if (vscroll_areaP_.mousePressed(clickx_, clicky_))
		return;

	if (vscroll_areaN_.mousePressed(clickx_, clicky_))
		return;

	if (isInsideWindow_)
	{
		if (canSearch_ && mouseover_ >= 0)
		{
			// Area A.
			if (mouseover_ < len_current_showlist_ && isInside_areaA_)
			{
				isHolding_areaA_ = true;
				isHolding_areaP_ = false;
				isHolding_areaN_ = false;
			}	// Area P.
			else if (mouseover_ < len_positives_ && len_positives_ > 0 && isInside_areaP_)
			{
				isHolding_areaA_ = false;
				isHolding_areaP_ = true;
				isHolding_areaN_ = false;
			}	// Area N.
			else if (mouseover_ < len_negatives_ && len_negatives_ > 0 && isInside_areaN_)
			{
				isHolding_areaA_ = false;
				isHolding_areaP_ = false;
				isHolding_areaN_ = true;
			}
			else
			{
				isHolding_areaA_ = false;
				isHolding_areaP_ = false;
				isHolding_areaN_ = false;
			}
		}
		else
		{
			isHolding_areaA_ = false;
			isHolding_areaP_ = false;
			isHolding_areaN_ = false;
		}
	}
	else
	{
		isHolding_areaA_ = false;
		isHolding_areaP_ = false;
		isHolding_areaN_ = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	if (isInsideWindow_)
	{
		if (holdImgNum_ >= 0)
		{
			if (isHolding_areaA_)
			{
				std::vector<int>* list = &showList_active_;

				if (isactive_)
					list = &showList_active_;
#ifndef OPENUSE
				else if (isorigin_)
					list = &showList_origin_;
				else if (isrerank_)
					list = &showList_rerank_;
#ifdef VISUALRANK
				else if (isvisualrank_)
					list = &showList_visualrank_;
#endif
#else
				else if (isresult_)
				{
					if (!isMainMethod_)
						list = &showList_origin_;
					else
						list = &showList_rerank_;
				}
#endif

				const int dragImgId = (*list)[holdImgNum_];
				ofImage dragImg = loader_->picture_[holdImgNum_];
				bool check_duplication_P = vector_finder(positives_, dragImgId);
				bool check_duplication_N = vector_finder(negatives_, dragImgId);

				if (isactive_)
				{
					if (!check_duplication_P && !check_duplication_N)
					{
						// A -> P.
						if (isInside_areaP_)
						{
							std::string active = "selection: " + ofToString(dragImgId)+ " --> positive";
							activelog_->Write(epoch_, active);
							std::cout << "[ofApp] " << active << std::endl;

							positives_.push_back(dragImgId);
							positive_images_.push_back(dragImg);
							len_positives_ = positives_.size();
							selection_count_++;
						}	// A -> N.
						else if (isInside_areaN_)
						{
							std::string active = "selection: " + ofToString(dragImgId) + " --> negative";
							activelog_->Write(epoch_, active);
							std::cout << "[ofApp] " << active << std::endl;

							negatives_.push_back(dragImgId);
							negative_images_.push_back(dragImg);
							len_negatives_ = negatives_.size();
							selection_count_++;
						}
					}
				}

				// Judging.
				if (isInside_propose_)
				{
					judged_img_ = dragImg;

					if (searchTarget_ == dragImgId)
					{
						std::string active = "judge: " + ofToString(dragImgId) + " --> true";
						activelog_->Write(epoch_, active);
						std::cout << "[ofApp] " << active << std::endl;

						isJudgeTrue_ = true;
					}
					else
					{
						std::string active = "judge: " + ofToString(dragImgId) + " --> false";
						activelog_->Write(epoch_, active);
						std::cout << "[ofApp] " << active << std::endl;

						isJudgeFalse_ = true;
						pause_timer_start_ = ofGetElapsedTimef();
					}
				}
			}
			else if (isHolding_areaP_)
			{
				std::vector<int> *list = &positives_;
				const int dragImgId = (*list)[holdImgNum_];
				ofImage dragImg = positive_images_[holdImgNum_];

				// P -> A.
				if (isInside_areaA_)
				{
					std::string active = "positive: " + ofToString(dragImgId) + " --> removed";
					activelog_->Write(epoch_, active);
					std::cout << "[ofApp] " << active << std::endl;

					positives_.erase(positives_.begin() + holdImgNum_);
					positive_images_.erase(positive_images_.begin() + holdImgNum_);
					len_positives_ = positives_.size();
					selection_count_++;
				}	// P -> N.
				else if (isInside_areaN_)
				{
					bool check_duplication = vector_finder(negatives_, dragImgId);
					if (!check_duplication)
					{
						std::string active = "positive: " +ofToString(dragImgId) + " --> negative";
						activelog_->Write(epoch_, active);
						std::cout << "[ofApp] " << active << std::endl;

						negatives_.push_back(dragImgId);
						negative_images_.push_back(dragImg);
						positives_.erase(positives_.begin() + holdImgNum_);
						positive_images_.erase(positive_images_.begin() + holdImgNum_);
						len_positives_ = positives_.size();
						len_negatives_ = negatives_.size();
						selection_count_++;
					}
				}
			}
			else if (isHolding_areaN_)
			{
				std::vector<int> *list = &negatives_;
				const int dragImgId = (*list)[holdImgNum_];
				ofImage dragImg = negative_images_[holdImgNum_];

				// N -> A.
				if (isInside_areaA_)
				{
					std::string active = "negative: " + ofToString(dragImgId) + " --> removed.";
					activelog_->Write(epoch_, active);
					std::cout << "[ofApp] " << active << std::endl;

					negatives_.erase(negatives_.begin() + holdImgNum_);
					negative_images_.erase(negative_images_.begin() + holdImgNum_);
					len_negatives_ = negatives_.size();
					selection_count_++;
				}	// N -> P.
				else if (isInside_areaP_)
				{
					bool check_duplication = vector_finder(positives_, dragImgId);
					if (!check_duplication)
					{
						std::string active = "negative: " + ofToString(dragImgId) + " --> positive.";
						activelog_->Write(epoch_, active);
						std::cout << "[ofApp] " << active << std::endl;

						positives_.push_back(dragImgId);
						positive_images_.push_back(dragImg);
						negatives_.erase(negatives_.begin() + holdImgNum_);
						negative_images_.erase(negative_images_.begin() + holdImgNum_);
						len_positives_ = positives_.size();
						len_negatives_ = negatives_.size();
						selection_count_++;
					}
				}
			}
		}
	}

	if (click_ == true)
	{
#ifndef OPENUSE
		if (isReleasedArea(buttonA_posx_, buttonposy_line1_, button_width_, button_height_))
		{
#else
		if (isReleasedArea(selection_button_posx_, buttonposy_line1_, selection_button_width_, button_height_))
		{
#endif
			isactive_ = true;
#ifndef OPENUSE
			isorigin_ = false;
			isrerank_ = false;
#ifdef VISUALRANK
			isvisualrank_ = false;
#endif
#else
			isresult_ = false;
#endif

			onPaint(showList_active_);
		}
#ifndef OPENUSE
		else if (isReleasedArea(buttonB_posx_, buttonposy_line1_, button_width_, button_height_))
		{
			isactive_ = false;
			isorigin_ = true;
			isrerank_ = false;
#ifdef VISUALRANK
			isvisualrank_ = false;
#endif
			onPaint(showList_origin_);
		}
		else if (isReleasedArea(buttonC_posx_, buttonposy_line1_, button_width_, button_height_))
		{
			isactive_ = false;
			isorigin_ = false;
			isrerank_ = true;
#ifdef VISUALRANK
			isvisualrank_ = false;
#endif
			onPaint(showList_rerank_);
		}
#ifdef VISUALRANK
		else if (isReleasedArea(buttonD_posx_, buttonposy_line1_, button_width_, button_height_))
		{
			isactive_ = false;
			isorigin_ = false;
			isrerank_ = false;
			isvisualrank_ = true;
			onPaint(showList_visualrank_);
		}
#endif
#else
		if (isReleasedArea(result_button_posx_, buttonposy_line1_, result_button_width_, button_height_))
		{
			isactive_ = false;
			isresult_ = true;
			if (!isMainMethod_)
				onPaint(showList_origin_);
			else
				onPaint(showList_rerank_);
		}
#endif

		if (canSearch_ && isReleasedArea(searchbutton_posx_, buttonposy_line1_, searchbutton_width_, button_height_))
		{
			if (len_positives_ == 0)
				std::cerr << "[Warning] Please select positive sample (at least 1)." << std::endl;
			else
			{
				isactive_ = true;
#ifndef OPENUSE
				isorigin_ = false;
				isrerank_ = false;
#ifdef VISUALRANK
				isvisualrank_ = false;
#endif
#else
				isresult_ = false;
#endif
				onPaint(showList_active_);
				autoselect_negative();
			}

			if (len_positives_ > 0 && len_negatives_ > 0)
			{
				epoch_++;
				std::cout << "[ofApp] " << "Feedback: " << epoch_ << std::endl;

				canSearch_ = false;
				total_search_time_ += ofGetElapsedTimef() - search_timer_start_;


				// Original search query vector (initail features).
				rocchio_init_->set_features(loading_->features_);
				rocchio_init_->setInput_multi(positives_, negatives_);
				rocchio_init_->set_weight(alpha_, beta_, gamma_);
				rocchio_init_->run();

				// Search neibors data by query vector.
				search_->set_queryvector(rocchio_init_->queryvector_);
				search_->startThread();
			}
		}
	}

	if (vscroll_areaA_.mouseReleased(x, y))
		return;

	if (vscroll_areaP_.mouseReleased(x, y))
		return;

	if (vscroll_areaN_.mouseReleased(x, y))
		return;

	clickx_ = -1;
	clicky_ = -1;
	click_ = false;

	dragx_ = -1;
	dragy_ = -1;
	holding_x_ = -1;
	holding_y_ = -1;

	isHoldAndDrag_ = false;
	isHolding_areaA_ = false;
	isHolding_areaP_ = false;
	isHolding_areaN_ = false;
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
	if (isInsideMouseoverArea(leftsize_, uppersize_, area_width_ + ScrollBarWidth_, area_height_))
	{
		int current = vscroll_areaA_.current();
		current -= change_by_button_ * scrollY;
		vscroll_areaA_.current(current);
	}
	else if (isInsideMouseoverArea(overview_areamargin_, overviewP_areaposy_, overview_areawidth_ + ScrollBarWidth_, overview_areaheight_))
	{
		int current = vscroll_areaP_.current();
		current -= change_by_button_overview_ * scrollY;
		vscroll_areaP_.current(current);
	}
	else if (isInsideMouseoverArea(overview_areamargin_, overviewN_areaposy_, overview_areawidth_ + ScrollBarWidth_, overview_areaheight_))
	{
		int current = vscroll_areaN_.current();
		current -= change_by_button_overview_ * scrollY;
		vscroll_areaN_.current(current);
	}
}

//--------------------------------------------------------------
bool ofApp::isReleasedArea(float x, float y, float w, float h)
{
	if (clickx_ >= x && clicky_ >= y)
	{
		if (clickx_ <= x + w && clicky_ <= y + h)
			return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofApp::isInsideDragingArea(float x, float y, float w, float h)
{
	if (dragx_ >= x && dragy_ >= y)
	{
		if (dragx_ <= x + w && dragy_ <= y + h)
			return true;
	}
	return false;
}

//--------------------------------------------------------------
bool ofApp::isInsideMouseoverArea(float x, float y, float w, float h)
{
	if (mouseoverx_ >= x && mouseovery_ >= y)
	{
		if (mouseoverx_ <= x + w && mouseovery_ <= y + h)
			return true;
	}
	return false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
	isInsideWindow_ = true;
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
	mouseover_ = -1;
	holdImgNum_ = -1;
	isInsideWindow_ = false;
	isInside_areaA_ = false;
	isInside_areaP_ = false;
	isInside_areaN_ = false;
	isInside_propose_ = false;
	isHoldAndDrag_ = false;
	isHolding_areaA_ = false;
	isHolding_areaP_ = false;
	isHolding_areaN_ = false;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	windowWidth_ = w;
	windowHeight_ = h;
	sizeChanged();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

//--------------------------------------------------------------
void ofApp::calculate()
{
	database_->makeShowList_active(active_size_);
	showList_active_ = database_->getShowList();

	database_->makeShowList_origin(show_size_);
	showList_origin_ = database_->getShowList();

	database_->makeShowList_rerank(show_size_);
	showList_rerank_ = database_->getShowList();

#ifdef VISUALRANK
	database_->makeShowList_visualrank(show_size_);
	showList_visualrank_ = database_->getShowList();
#endif
}

//--------------------------------------------------------------
void ofApp::onPaint(const std::vector<int>& list)
{
	sizeChanged();
	loader_->setShowList(list);
	loader_->load_images();
	len_current_showlist_ = list.size();

	drawHeight_areaA_ = d_size_ * rowShow_;
	drawHeight_areaP_ = overview_d_size_ * overviewP_rowShow_;
	drawHeight_areaN_ = overview_d_size_ * overviewN_rowShow_;

	vscroll_areaA_.current(0);
}

//--------------------------------------------------------------
void ofApp::writelog()
{
	std::vector<int> candidate_active, candidate_origin, candidate_rerank;
#ifdef VISUALRANK
	std::vector<int> candidate_visualrank;
#endif

	candidate_active.resize(active_size_);
	candidate_origin.resize(show_size_);
	candidate_rerank.resize(show_size_);
#ifdef VISUALRANK
	candidate_visualrank.resize(search_window_size_);
#endif

	for (int i = 0; i < active_size_; ++i)
	{
		int num_active = number_active_[i];
		candidate_active[i] = num_active;
	}

	for (int i = 0; i < show_size_; ++i)
	{
		int num_origin = number_origin_[i];
		int num_rerank = number_rerank_[i];
#ifdef VISUALRANK
		int num_visualrank = number_visualrank_[i];
#endif
		candidate_origin[i] = num_origin;
		candidate_rerank[i] = num_rerank;
#ifdef VISUALRANK
		candidate_visualrank[i] = num_visualrank;
#endif
	}

	logger_active_->writeCandidate(candidate_active);
	logger_origin_->writeCandidate(candidate_origin);
	logger_rerank_->writeCandidate(candidate_rerank);
#ifdef VISUALRANK
	logger_visualrank_->writeCandidate(candidate_visualrank);
#endif
}

//--------------------------------------------------------------
void ofApp::sizeChanged()
{
	std::vector<int>* sList;
	if (isactive_)
		sList = &showList_active_;
#ifndef OPENUSE
	else if (isorigin_)
		sList = &showList_origin_;
	else if (isrerank_)
		sList = &showList_rerank_;
#ifdef VISUALRANK
	else if (isvisualrank_)
		sList = &showList_visualrank_;
#endif
#else
	if (isresult_)
	{
		if (!isMainMethod_)
			sList = &showList_origin_;
		else
			sList = &showList_rerank_;
	}
#endif

	rowShow_ = (sList->size() + colShow_ - 1) / colShow_;
	if (rowShow_ < 1)
		rowShow_ = 1;

	overviewP_rowShow_ = (positives_.size() + overview_colShow_ - 1) / overview_colShow_;
	overviewN_rowShow_ = (negatives_.size() + overview_colShow_ - 1) / overview_colShow_;
}

//--------------------------------------------------------------
// Update the displayed size, scroll size, etc. of the scroll bars
// (Called when the window size or the image size is changed)
void ofApp::updateScrollBars()
{
	//---------------------------------------------------------------------------
	// All results viewer.
	if (drawHeight_areaA_ < area_height_)
	{
		vscroll_areaA_.max(0);
		vscroll_areaA_.change_by_bar(0);
		vscroll_areaA_.current(0);
	}
	else
		vscroll_areaA_.max(drawHeight_areaA_ - vscroll_areaA_.bar_length());

	//---------------------------------------------------------------------------
	// Positive results viewer.
	if (drawHeight_areaP_ < overview_areaheight_)
	{
		vscroll_areaP_.max(0);
		vscroll_areaP_.change_by_bar(0);
		vscroll_areaP_.current(0);
	}
	else
		vscroll_areaP_.max(drawHeight_areaP_ - vscroll_areaP_.bar_length());

	//---------------------------------------------------------------------------
	// Negative results viewer.
	if (drawHeight_areaN_ < overview_areaheight_)
	{
		vscroll_areaN_.max(0);
		vscroll_areaN_.change_by_bar(0);
		vscroll_areaN_.current(0);
	}
	else
		vscroll_areaN_.max(drawHeight_areaN_ - vscroll_areaN_.bar_length());
}

//--------------------------------------------------------------
void ofApp::initializeBars()
{
	//---------------------------------------------------------------------------
	// All results viewer.
	scroll_areaA_ = 0;
	vscroll_areaA_.min(0);
	vscroll_areaA_.bar_width(ScrollBarWidth_);
	vscroll_areaA_.bar_length(area_height_);
	vscroll_areaA_.bar_pos_widthdir(leftsize_ + area_width_);
	vscroll_areaA_.bar_pos_lengthdir(uppersize_);
	vscroll_areaA_.change_by_button(change_by_button_);
	vscroll_areaA_.current(0);

	//---------------------------------------------------------------------------
	// Positive sample viewer.
	scroll_areaP_ = 0;
	vscroll_areaP_.min(0);
	vscroll_areaP_.bar_width(ScrollBarWidth_);
	vscroll_areaP_.bar_length(overview_areaheight_);
	vscroll_areaP_.bar_pos_widthdir(overview_areamargin_ + overview_areawidth_);
	vscroll_areaP_.bar_pos_lengthdir(uppersize_);
	vscroll_areaP_.change_by_button(change_by_button_overview_);
	vscroll_areaP_.current(0);

	//---------------------------------------------------------------------------
	// Negative sample viewer.
	scroll_areaN_ = 0;
	vscroll_areaN_.min(0);
	vscroll_areaN_.bar_width(ScrollBarWidth_);
	vscroll_areaN_.bar_length(overview_areaheight_);
	vscroll_areaN_.bar_pos_widthdir(overview_areamargin_ + overview_areawidth_);
	vscroll_areaN_.bar_pos_lengthdir(overviewN_areaposy_);
	vscroll_areaN_.change_by_button(change_by_button_overview_);
	vscroll_areaN_.current(0);
}

//--------------------------------------------------------------
void ofApp::calculateHoldingOriginPoint()
{
	int x, y;
	x = dragx_ - holdImg_scale_ * d_size_ / 2;
	y = dragy_ - holdImg_scale_ * d_size_ / 2;

	holding_x_ = x;
	holding_y_ = y;
}

//--------------------------------------------------------------
void ofApp::showProcessingTime()
{
	float rocchio = rocchio_init_->process_time_;
	float search = search_->process_time_;
	float trainer = trainer_->process_time_;
	float loading = loading_->process_time_;
	float rerank = rerank_->process_time_;
	float back_process = trainer + loading + rocchio + search + rerank;
#ifdef VISUALRANK
	float visualrank = visualrank_->process_time_;
#endif

	std::cout << "-------------------------- Processing Time --------------------------" << std::endl;
	std::cout << "Rocchio algorithm: " << rocchio << " sec." << std::endl;
	std::cout << "Searching (ngt): " << search << " sec." << std::endl;
	std::cout << "Online training (train + extraction + selection): " << trainer << " sec." << std::endl;
	std::cout << "Loading new features: " << loading << " sec." << std::endl;
	std::cout << "Reranking (ours): " << rerank << " sec." << std::endl;
#ifdef VISUALRANK
	std::cout << "Reranking (visualrank): " << visualrank << " sec." << std::endl;
#endif
	std::cout << "Total back process: " << back_process << " sec." << std::endl;
	std::cout << "---------------------------------------------------------------------" << std::endl;
}

//--------------------------------------------------------------
void ofApp::autoselect_negative()
{
	for (int i = 0; i < active_size_; ++i)
	{
		int number = showList_active_[i];
		bool check_duplication_P = vector_finder(positives_, number);
		bool check_duplication_N = vector_finder(negatives_, number);

		if (!check_duplication_P && !check_duplication_N)
		{
			negatives_.push_back(number);
			negative_images_.push_back(loader_->picture_[i]);
		}
	}
	len_negatives_ = negatives_.size();
	std::cout << "[ofApp] Auto sample selection." << std::endl;
}

//--------------------------------------------------------------
void ofApp::update_overview_info()
{
	overviewP_rowShow_ = (positives_.size() + overview_colShow_ - 1) / overview_colShow_;
	overviewN_rowShow_ = (negatives_.size() + overview_colShow_ - 1) / overview_colShow_;
	drawHeight_areaP_ = overview_d_size_ * overviewP_rowShow_;
	drawHeight_areaN_ = overview_d_size_ * overviewN_rowShow_;
}

//--------------------------------------------------------------
void ofApp::logdir_name()
{
	int i = 2;
	std::string basename = selection_method_ + "_" + ofToString(searchTarget_) + "_" + subjectname_;
	std::string name = basename;
	while (1)
	{
		logdir_ = "/home/yugo/workspace/Interface/bin/log/" + name;
		if (!isFileexists(logdir_))
			break;
		else
		{
			name = basename + ofToString(i);
		}
		i++;
	}
}

