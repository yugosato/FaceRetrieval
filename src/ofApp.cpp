#include "ofApp.h"


inline int toInt(std::string s) {int v; std::istringstream sin(s);sin>>v;return v;}
template<class T> inline std::string toString(T x) {std::ostringstream sout;sout<<x;return sout.str();}

void ofApp::initparam()
{
	//-----------------------------------------
	// History.
	historysize_ = 0;
	backcount_ = 0;
	forwardcount_ = 0;
	nowhistory_ = -1;
	ishistory_ = false;
	canBack_ = false;
	canForward_ = false;

	//-----------------------------------------
	//  The number of displayed images.
	picA_ = 1;
	picB_ = 25;
	picnum_ = picB_ - picA_ + 1;

	//-----------------------------------------
	// Mouse & Keyboard.
	clickx_ = -1;
	clicky_ = -1;
	dragx_ = -1;
	dragy_= -1;
	mouseover_ = -1;
	holdImgNum_ = -1;
	click_ = false;
	leftsideclick_ = false;
	isHolding_areaA_ = false;
	isHoldAndDrag_ = false;
	isInsideWindow_ = false;

	//-----------------------------------------
	// Display Settings.
	colShow_ = 5;
	d_size_ = (initWidth_ - leftsize_ - ScrollBarWidth_) / colShow_;
	width_areaA_ = 0;
	rowshort_ = false;

	//-----------------------------------------
	// Input Query.
	clickflag_ = false;

	//-----------------------------------------
	// Window Information.
	windowWidth_ = initWidth_;
	windowHeight_ = initHeight_;

	//-----------------------------------------
	// Font.
	ttf_ = binData_ + "fonts/RictyDiminished-Bold.ttf";

	//-----------------------------------------
	// Path Settings.
	binData_ = "/home/yugo/workspace/Interface/bin/data/";
	datasetdir_ = "/home/yugo/Desktop/dataset/";
	dataset_ = "cfd-cropped/";
	nameFile_ = datasetdir_ + dataset_ + "images_selected.txt";

#ifdef VGG
	featuresfile_ = binData_ + "cfd/cfd-vgg.tsv";
	npyFile_ = binData_ + "cfd/cfd-vgg.npy";
	indexFile_ = binData_ + "cfd/cfd-vgg_index-angle";
#endif
#ifdef HISTOGRAM
	indexFile_ = binData_ + "cfd/cfd-histogram_index-angle";
	matrixFile_ = binData_ + "cfd/cfd-histogram.tsv";
	npyFile_ = binData_ + "cfd/cfd-histogram.npy";
#endif
#ifdef GABOR
	indexFile_ = binData_ + "cfd/cfd-gabor_index-angle";
	matrixFile_ = binData_ + "cfd/cfd-gabor.tsv";
	npyFile_ = binData_ + "cfd/cfd-gabor.npy";
#endif
#ifdef HISTOGRAM_GABOR
	indexFile_ = binData_ + "cfd/cfd-histogram-gabor_index-angle";
	matrixFile_ = binData_ + "cfd/cfd-histogram-gabor.tsv";
	npyFile_ = binData_ + "cfd/cfd-histogra"
#endif

	// Log Settings.
	std::string time;
	put_time(time);
	logdir_ = "/home/yugo/workspace/Interface/bin/log/" + time + "/";
	candidatefile_active_ = logdir_ + "candidate_active.txt";
	candidatefile_origin_ = logdir_ + "candidate_origin.txt";
	candidatefile_main_ = logdir_ + "candidate_main.txt";
	candidatefile_visualrank_ = logdir_ + "candidate_visualrank.txt";
	init_candidatefile_ = binData_ + "cfd/initialize.txt";
	evaluationfile_ = logdir_ + "evaluation.csv";

	// Python Settings.
	pysettingfile_ = binData_ + "cfd/py_setting.txt";
	samplefile_ = logdir_ + "feedback.txt";
	scriptfile_ = "/home/yugo/workspace/Interface/trainer/main_process.py";
	positiveIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/positive_index.txt";
	negativeIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/negative_index.txt";
	activeIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/active_index.txt";
	cueflikIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/cueflik_index.txt";
	randomIndexfile_ = "/home/yugo/workspace/Interface/trainer/result/random_index.txt";
	resultGraphfile_ = "/home/yugo/workspace/Interface/trainer/result/acc_val.png";
	new_featuresfile_ = "/home/yugo/workspace/Interface/trainer/result/features.tsv";

	//-----------------------------------------
	// Retrieval results.
	isactive_ = true;
	isorigin_ = false;
	ismain_ = false;
	isvisualrank_ = false;

	//-----------------------------------------
	// Overview Settings.
	isHolding_areaA_ = false;
	isHolding_areaP_ = false;
	isHolding_areaN_ = false;
	isHoldAndDrag_ = false;
	isInsideWindow_ = false;
	isInside_areaA_ = false;
	isInside_areaP_ = false;
	isInside_areaN_ = false;
	overview_colShow_ = 15;
	overviewN_rowShow_ = 0;
	overviewN_rowShow_ = 0;
	holdImgNum_ = -1;
	holding_x_ = -1;
	holding_y_ = -1;
	int perHeight = windowHeight_ / 3;
	positive_txt_posy_ = 25;
	negative_txt_posy_ = 25 + perHeight;
	reliability_txt_posy_ = 25 + 2 * perHeight;
	overviewP_areaposy_ = positive_txt_posy_ + 10;
	overviewN_areaposy_ = negative_txt_posy_ + 10;
	overviewR_areaposy_ = reliability_txt_posy_ + 10;
	len_positives_ = 0;
	len_negatives_ = 0;

	//-----------------------------------------
	// Others.
	epoch_ = 0;
	isSearchedAll_ = false;
	isReady_ = false;
	isFinishedInitSet_ = false;
	isActiveSelected_ = false;
	canSearch_ = false;
}

//--------------------------------------------------------------
void ofApp::loadImageandFont()
{
	font_.load(ttf_, fontsize_);

	backbutton0_.load(binData_ + "items/cantBack.png");
	backbutton1_.load(binData_ + "items/canBack1.png");

	forwardbutton0_.load(binData_ + "items/cantForward.png");
	forwardbutton1_.load(binData_ + "items/canForward1.png");

	searchbutton1_.load(binData_ + "items/search1.png");
	searchbutton2_.load(binData_ + "items/search2.png");

	button1_active_.load(binData_ + "items/active1.png");
	button2_active_.load(binData_ + "items/active2.png");
	button1_origin_.load(binData_ + "items/origin1.png");
	button2_origin_.load(binData_ + "items/origin2.png");
	button1_main_.load(binData_ + "items/main1.png");
	button2_main_.load(binData_ + "items/main2.png");
	button1_visualrank_.load(binData_ + "items/visualrank1.png");
	button2_visualrank_.load(binData_ + "items/visualrank2.png");

	graph_.load(binData_ + "items/init_graph_wide.png");
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

	// Get Database information.
	database_ = new DataBase();
	database_->setup(nameFile_, init_candidatefile_);
	row_ = database_->getRow();
	database_->getName(&name_);
	database_->getPersonID(&person_ids_);

	// Initialize scroll bar.
	initializeBars();

	// Load images.
	loader_ = new ImageLoader();
	loader_->setRow(row_);
	loader_->setName(name_);
	calculate();
	onPaint(showList_active_);
	firstshowlist_ = showList_active_;

	// Setup search method.
	search_ = new Search();
	search_->setup(indexFile_, picnum_);

	// Load image features (multi thread).
	loading_ = new NowLoading();
	loading_->set_featuresfile(featuresfile_, new_featuresfile_);
	loading_->setRow(row_);
	loading_->startThread();

	// Setup writer.
	if (!isFileexists(logdir_))
		mkdir(logdir_.c_str(), 0777);
	samplewriter_ = new SampleWriter(samplefile_);
	samplewriter_->init_write();

	// Setup online trainer.
	trainer_ = new Trainer;
	trainer_->setup(scriptfile_);

	// Setup active selection.
	selection_ = new Selection;
	selection_->setup(positiveIndexfile_, negativeIndexfile_, activeIndexfile_,
			cueflikIndexfile_, randomIndexfile_);

	// Setup reranking method.
	rerank_ = new ReRank;
	visualrank_ = new VisualRank;

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
	gui_->setScrollAreaHeight(guiScrollarea_height_);
	gui_->setScrollableDirections(false, false);
	gui_->setColorBack(ofColor(0.0f, 0.0f, 0.0f, 0.0f));
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
	delete logger_main_;
	delete logger_visualrank_;
	delete trainer_;
	delete selection_;
	delete rerank_;
	delete visualrank_;
	delete single_evaluater_;
}

//--------------------------------------------------------------

void ofApp::update()
{
	ofBackground(ofColor(0x000000));

	if (loading_->isLoaded_init_)
	{
		loading_->stopThread();
		loading_->isLoaded_init_ = false;
		search_->set_features(loading_->features_);

		isFinishedInitSet_ = true;
		canSearch_ = true;

		logger_active_ = new Logger;
		logger_origin_ = new Logger;
		logger_main_ = new Logger;
		logger_visualrank_ = new Logger;

		logger_active_->setup(samplefile_, candidatefile_active_, pysettingfile_, npyFile_, loading_->col_);
		logger_origin_->setup(samplefile_, candidatefile_origin_, pysettingfile_, npyFile_, loading_->col_);
		logger_main_->setup(samplefile_, candidatefile_main_, pysettingfile_, npyFile_, loading_->col_);
		logger_visualrank_->setup(samplefile_, candidatefile_visualrank_, pysettingfile_, npyFile_, loading_->col_);

		logger_main_->writePySetting();

		single_evaluater_ = new SingleSearchEvaluater;
		single_evaluater_->setup(searchTarget_, evaluationfile_);
		single_evaluater_->set_features(loading_->features_);

		number_active_ = database_->number_main_;
		number_origin_ = database_->number_origin_;
		number_main_ = database_->number_main_;
		number_visualrank_ = database_->number_visualrank_;

		writelog();
	}

	if (trainer_->isTrained_)
	{
		trainer_->stopThread();
		trainer_->isTrained_ = false;

		loading_->isLoadNew_ = true;
		loading_->startThread();	// Load new features.
	}

	if (loading_->isLoaded_new_)
	{
		loading_->stopThread();
		loading_->isLoaded_new_ = false;

		search_->startThread();
	}

	if (search_->isSearched_)
	{
		search_->stopThread();
		search_->isSearched_ = false;
		search_->getNumber(&number_origin_);

		// Main Reranking.
		rerank_->set_features(loading_->new_features_);
		rerank_->set_init_result(number_origin_);
		rerank_->setInput_multi(positives_, negatives_);
		rerank_->startThread();
	}

	if (rerank_->isReranked_)
	{
		rerank_->stopThread();
		rerank_->isReranked_ = false;
		rerank_->getNumber(&number_main_);

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

		isSearchedAll_ = true;
	}

	if (isSearchedAll_)
	{
		isSearchedAll_ = false;
		selection_->load();
	}

	if (selection_->isLoaded_)
	{
		selection_->isLoaded_ = false;
		selection_->getNumber(&number_active_);
		isReady_ = true;
	}

	if (isReady_)
	{
		isReady_ = false;

		// Single Search Evaluation.
		single_evaluater_->set_inputpoint(search_->queryvector_, rerank_->queryvector_);
		single_evaluater_->run();

		database_->setNumber_active(number_active_);
		database_->setNumber_origin(number_origin_);
		database_->setNumber_main(number_main_);
		database_->setNumber_visualrank(number_visualrank_);

		graph_.load(resultGraphfile_);
		graph_.update();

		isactive_ = true;
		isorigin_ = false;
		ismain_ = false;
		isvisualrank_ = false;

		calculate();
		onPaint(showList_active_);
		inputHistory();

		canSearch_ = true;
		process_time_ = ofGetElapsedTimef() - timer_start_;

		showProcessingTime();

		std::cout << "#####################################################################################################" << std::endl;
		vscroll_areaA_.current(0);
	}

	vscroll_areaA_.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofFill();
	ofSetLineWidth(1);

	ofImage img;
	const int len = loader_->row_;
	scroll_areaA_ = - vscroll_areaA_.current();

	for (int i = 0; i < len; ++i)
	{
		const int j = i % colShow_;
		const int k = i / colShow_;

		if (windowHeight_ < uppersize_ + d_size_ * k + scroll_areaA_)
			break;
		else if (0 > uppersize_ + d_size_ * (k + 1) + scroll_areaA_)
			continue;

		int drawx = leftsize_ + d_size_ * j;
		int drawy = uppersize_ + d_size_ * k + scroll_areaA_;

		img = loader_->picture_[i];

		if (!img.isAllocated())
		{
			break;
		}
		else if (isactive_)
		{
			int imgId = loader_->showList_[i];
			int exist_positive = vector_finder(positives_, imgId);
			int exist_negative = vector_finder(negatives_, imgId);

			if ((isHolding_areaA_ && i == holdImgNum_) || exist_positive >= 0 || exist_negative >= 0)
			{
				if (exist_positive >= 0)
				{
					int margin = 5;
					ofNoFill();
					ofSetLineWidth(5);
					ofSetColor(ofColor(255.0f, 0.0f, 0.0f, 255.0f));
					ofDrawRectangle(drawx + margin, drawy + margin, d_size_ - 2*margin, d_size_ - 2*margin);

				}
				else if (exist_negative >=0)
				{
					int margin = 5;
					ofNoFill();
					ofSetLineWidth(5);
					ofSetColor(ofColor(0.0f, 0.0f, 255.0f, 255.0f));
					ofDrawRectangle(drawx + margin, drawy + margin, d_size_ - 2 * margin, d_size_ - 2 * margin);
				}

				ofFill();
				ofSetLineWidth(1);
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 130.0f));
				img.draw(drawx, drawy, d_size_, d_size_);
			}
			else
			{
				ofFill();
				ofSetLineWidth(1);
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
				img.draw(drawx, drawy, d_size_, d_size_);
			}
		}
		else if (!isactive_)
		{
			ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
			img.draw(drawx, drawy, d_size_, d_size_);
		}
	}

    ofSetColor(ofColor(0.0f, 0.0f, 0.0f, 255.0f));
    ofDrawRectangle(leftsize_, 0, windowWidth_ - leftsize_, uppersize_);

   	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
   	searchbutton1_.draw(searchbuttonposx_, buttonposy_line1_, searchbuttonwidth_, buttonheight_);

	std::string text;
	if (isactive_)
	{
		button2_active_.draw(buttonposx_active_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button1_origin_.draw(buttonposx_origin_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button1_main_.draw(buttonposx_main_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button1_visualrank_.draw(buttonposx_visualrank_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		text = "ActiveSelection";
	}
	else if (isorigin_)
	{
		button1_active_.draw(buttonposx_active_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button2_origin_.draw(buttonposx_origin_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button1_main_.draw(buttonposx_main_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button1_visualrank_.draw(buttonposx_visualrank_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		text = "Original";
	}
	else if (ismain_)
	{
		button1_active_.draw(buttonposx_active_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button1_origin_.draw(buttonposx_origin_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button2_main_.draw(buttonposx_main_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button1_visualrank_.draw(buttonposx_visualrank_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		text = "Rerank";
	}
	else if (isvisualrank_)
	{
		button1_active_.draw(buttonposx_active_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button1_origin_.draw(buttonposx_origin_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button1_main_.draw(buttonposx_main_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		button2_visualrank_.draw(buttonposx_visualrank_, buttonposy_line1_, buttonwidth_active_, buttonheight_);
		text = "VisualRank";
	}

	if (isFinishedInitSet_ && !canSearch_)
		text = "Searching...";

	float w = font_.stringWidth(text);
	font_.drawString(text, windowWidth_ - w - ScrollBarWidth_ - 10, 27);

	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 130.0f));
	if (isInsideWindow_ && isactive_)
	{
		if (isInside_areaP_)
		{
			ofDrawRectangle(overview_areamargin_, overviewP_areaposy_, overview_areawidth_, overview_areaheight_);
		}
		else if (isInside_areaN_)
		{
			ofDrawRectangle(overview_areamargin_, overviewN_areaposy_, overview_areawidth_, overview_areaheight_);
		}
	}

	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
	std::string positive = "Positive Sample: ";
	std::string negative = "Negative Sample: ";
	std::string reliability = "Reliability Graph";

	font_.drawString(positive + ofToString(len_positives_), overview_areamargin_, positive_txt_posy_);
	font_.drawString(negative + ofToString(len_negatives_), overview_areamargin_, negative_txt_posy_);

	const int len_positive_images = positive_images_.size();
	const int len_negative_images = negative_images_.size();

	if (len_positive_images > 0)
	{
		for (int i = 0; i < len_positive_images; ++i)
		{
			const int j = i % overview_colShow_;
			const int k = i / overview_colShow_;

			int drawx = overview_areamargin_ + overview_d_size_ * j;
			int drawy = overviewP_areaposy_ + overview_d_size_ * k;

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

	if (len_negative_images > 0)
	{
		for (int i = 0; i < len_negative_images; ++i)
		{
			const int j = i % overview_colShow_;
			const int k = i / overview_colShow_;

			int drawx = overview_areamargin_ + overview_d_size_ * j;
			int drawy = overviewN_areaposy_ + overview_d_size_ * k;

			ofImage img_negative = negative_images_[i];

			if (!img_negative.isAllocated())
			{
				break;
			}
			else
			{
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
				img_negative.draw(drawx, drawy, overview_d_size_, overview_d_size_);
			}
		}
	}

	//vscroll_areaA_.draw();

	font_.drawString(reliability, overview_areamargin_, reliability_txt_posy_);
	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
	graph_.draw(overview_areamargin_, overviewR_areaposy_, overview_areawidth_wide_, overview_areaheight_);

	if (isHoldAndDrag_)
	{
		ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
		ofImage holdImg;
		if (isHolding_areaA_)
		{
			holdImg = loader_->picture_[holdImgNum_];
		}
		else if (isHolding_areaP_)
		{
			holdImg = positive_images_[holdImgNum_];
		}
		else if (isHolding_areaN_)
		{
			holdImg = negative_images_[holdImgNum_];
		}

		if (holdImg.isAllocated())
			holdImg.draw(holding_x_, holding_y_, d_size_, d_size_);
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
			std::string savepath = logdir_ + time + ".jpeg";
			img.save(savepath);
			std::cout << "[ofApp] Saved snapshot image." << std::endl;
			std::cout << "[ofApp] --> \"" << savepath << "\"" << std::endl;
			break;
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
		if (isactive_)
		{
			// Area A.
			if (isInsideMouseoverArea(leftsize_, uppersize_, width_areaA_, windowHeight_ - uppersize_))
			{
				isInside_areaA_ = true;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
				x_dash = x - leftsize_;
				y_dash = y - scroll_areaA_ - uppersize_;
				mouseover_ = x_dash / d_size_ + y_dash / d_size_ * colShow_;
			}	// Area P.
			else if (isInsideMouseoverArea(overview_areamargin_, overviewP_areaposy_, overview_areawidth_, overview_areaheight_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = true;
				isInside_areaN_ = false;
				x_dash = x - overview_areamargin_;
				y_dash = y - overviewP_areaposy_;
				mouseover_ = x_dash / overview_d_size_ + y_dash / overview_d_size_ * overview_colShow_;
			}	// Area N.
			else if (isInsideMouseoverArea(overview_areamargin_, overviewN_areaposy_, overview_areawidth_, overview_areaheight_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = true;
				x_dash = x - overview_areamargin_;
				y_dash = y - overviewN_areaposy_;
				mouseover_ = x_dash / overview_d_size_ + y_dash / overview_d_size_ * overview_colShow_;
			}
			else
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
				mouseover_ = -1;
			}
		}
		else
		{
			isInside_areaA_ = false;
			isInside_areaP_ = false;
			isInside_areaN_ = false;
			mouseover_ = -1;
		}
	}
	else
	{
		isInside_areaA_ = false;
		isInside_areaP_ = false;
		isInside_areaN_ = false;
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

	if (isInsideWindow_)
	{
		if (isactive_)
		{
			if (canSearch_ && mouseover_ >= 0)
			{
				if (isHolding_areaA_)
				{
					int imgId = showList_active_[mouseover_];
					int exist_positive = vector_finder(positives_, imgId);
					int exist_negative = vector_finder(negatives_, imgId);

					if (exist_positive < 0 && exist_negative < 0)
					{
						holdImgNum_ = mouseover_;
						isHoldAndDrag_ = true;
						calculateHoldingOriginPoint();
					}
				}
				else if (isHolding_areaP_)
				{
					int imgId = positives_[mouseover_];
					int exist_negative = vector_finder(negatives_, imgId);

					if (exist_negative < 0)
					{
						holdImgNum_ = mouseover_;
						isHoldAndDrag_ = true;
						calculateHoldingOriginPoint();
					}
				}
				else if (isHolding_areaN_)
				{
					int imgId = negatives_[mouseover_];
					int exist_positive = vector_finder(positives_, imgId);

					if (exist_positive < 0)
					{
						holdImgNum_ = mouseover_;
						isHoldAndDrag_ = true;
						calculateHoldingOriginPoint();
					}
				}
			}

			// Area A.
			if (isInsideDragingArea(leftsize_, uppersize_, width_areaA_, windowHeight_ - uppersize_))
			{
				isInside_areaA_ = true;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
			}	// Area P.
			else if (isInsideDragingArea(overview_areamargin_, overviewP_areaposy_, overview_areawidth_, overview_areaheight_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = true;
				isInside_areaN_ = false;
			}	// Area N.
			else if (isInsideDragingArea(overview_areamargin_, overviewN_areaposy_, overview_areawidth_, overview_areaheight_))
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = true;
			}
			else
			{
				isInside_areaA_ = false;
				isInside_areaP_ = false;
				isInside_areaN_ = false;
			}
		}
		else
		{
			isInside_areaA_ = false;
			isInside_areaP_ = false;
			isInside_areaN_ = false;
		}
	}
	else
	{
		isInside_areaA_ = false;
		isInside_areaP_ = false;
		isInside_areaN_ = false;
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

	if (isInsideWindow_)
	{
		if (isactive_ && mouseover_ >= 0)
		{
			// Area A.
			if (mouseover_ < picnum_ && isInside_areaA_)
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
		if (isactive_ && holdImgNum_ >= 0)
		{
			if (isHolding_areaA_)
			{
				std::vector<int> *list = &showList_active_;
				const int dragImgId = (*list)[holdImgNum_];
				ofImage dragImg = loader_->picture_[holdImgNum_];
				int check_duplication_P = vector_finder(positives_, dragImgId);
				int check_duplication_N = vector_finder(negatives_, dragImgId);

				if (check_duplication_P < 0 && check_duplication_N < 0)
				{
					// A -> P.
					if (isInside_areaP_)
					{
						std::cout << "[ofApp] ActiveSelection No." << holdImgNum_ << " (ID:" << dragImgId << ") -> Positive." << std::endl;
						positives_.push_back(dragImgId);
						positive_images_.push_back(dragImg);
						len_positives_ = positives_.size();
					}	// A -> N.
					else if (isInside_areaN_)
					{
						std::cout << "[ofApp] ActiveSelection No." << holdImgNum_ << " (ID:" << dragImgId << ") -> Negative." << std::endl;
						negatives_.push_back(dragImgId);
						negative_images_.push_back(dragImg);
						len_negatives_ = negatives_.size();
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
					std::cout << "[ofApp] Positive No." << holdImgNum_ << " (ID:" << dragImgId << ") -> Removed." << std::endl;
					positives_.erase(positives_.begin() + holdImgNum_);
					positive_images_.erase(positive_images_.begin() + holdImgNum_);
					len_positives_ = positives_.size();
				}	// P -> N.
				else if (isInside_areaN_)
				{
					int check_duplication = vector_finder(negatives_, dragImgId);
					if (check_duplication < 0)
					{
						std::cout << "[ofApp] Positive No." << holdImgNum_ << " (ID:" << dragImgId << ") -> Negative." << std::endl;
						negatives_.push_back(dragImgId);
						negative_images_.push_back(dragImg);
						positives_.erase(positives_.begin() + holdImgNum_);
						positive_images_.erase(positive_images_.begin() + holdImgNum_);
						len_positives_ = positives_.size();
						len_negatives_ = negatives_.size();
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
					std::cout << "[ofApp] Negative No." << holdImgNum_ << " (ID:" << dragImgId << ") -> Removed." << std::endl;
					negatives_.erase(negatives_.begin() + holdImgNum_);
					negative_images_.erase(negative_images_.begin() + holdImgNum_);
					len_negatives_ = negatives_.size();
				}	// N -> P.
				else if (isInside_areaP_)
				{
					int check_duplication = vector_finder(positives_, dragImgId);
					if (check_duplication < 0)
					{
						std::cout << "[ofApp] Negative No." << holdImgNum_ << " (ID:" << dragImgId << ") -> Positive." << std::endl;
						positives_.push_back(dragImgId);
						positive_images_.push_back(dragImg);
						negatives_.erase(negatives_.begin() + holdImgNum_);
						negative_images_.erase(negative_images_.begin() + holdImgNum_);
						len_positives_ = positives_.size();
						len_negatives_ = negatives_.size();
					}
				}
			}
		}
	}

	if (click_ == true)
	{
		if (isReleasedArea(buttonposx_active_, buttonposy_line1_, buttonwidth_active_, buttonheight_))
		{
			isactive_ = true;
			isorigin_ = false;
			ismain_ = false;
			isvisualrank_ = false;
			onPaint(showList_active_);
		}
		else if (isReleasedArea(buttonposx_origin_, buttonposy_line1_, buttonwidth_active_, buttonheight_))
		{
			isactive_ = false;
			isorigin_ = true;
			ismain_ = false;
			isvisualrank_ = false;
			onPaint(showList_origin_);
		}
		else if (isReleasedArea(buttonposx_main_, buttonposy_line1_, buttonwidth_active_, buttonheight_))
		{
			isactive_ = false;
			isorigin_ = false;
			ismain_ = true;
			isvisualrank_ = false;
			onPaint(showList_main_);
		}
		else if (isReleasedArea(buttonposx_visualrank_, buttonposy_line1_, buttonwidth_active_, buttonheight_))
		{
			isactive_ = false;
			isorigin_ = false;
			ismain_ = false;
			isvisualrank_ = true;
			onPaint(showList_visualrank_);
		}


		if (isReleasedArea(searchbuttonposx_, buttonposy_line1_, searchbuttonwidth_, buttonheight_))
		{
			if (len_positives_ == 0)
				std::cerr << "[Warning] Please select positive sample (at least 1)." << std::endl;
			else if (len_positives_ + len_negatives_ != (int) showList_active_.size())
				autoselect_negative();

			if (len_positives_ > 0 && len_negatives_ > 0)
			{
				epoch_++;
				std::cout << "[ofApp] " << "Feedback: " << epoch_ << std::endl;

				clickflag_ = true;
				canSearch_ = false;

				samplewriter_->write(positives_, negatives_);
				search_->setInput_multi(positives_, negatives_);

				trainer_->startThread();
				timer_start_ = ofGetElapsedTimef();
			}
		}
	}

	if (vscroll_areaA_.mouseReleased(x, y))
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
	database_->makeShowList_active(picA_, picB_);
	showList_active_ = database_->getShowList();

	database_->makeShowList_origin(picA_, picB_);
	showList_origin_ = database_->getShowList();

	database_->makeShowList_main(picA_, picB_);
	showList_main_ = database_->getShowList();

	database_->makeShowList_visualrank(picA_, picB_);
	showList_visualrank_ = database_->getShowList();
}

//--------------------------------------------------------------
void ofApp::onPaint(const std::vector<int>& list)
{
	sizeChanged();
	loader_->setShowList(list);
	loader_->load_images();
	ishistory_ = false;

	width_areaA_ = d_size_ * colShow_;
	drawHeight_areaA_ = d_size_ * rowShow_;

	overview_d_size_ = (leftsize_ - 2 * overview_areamargin_) / overview_colShow_;
	overview_areawidth_ = overview_d_size_ * overview_colShow_;
	overview_areawidth_wide_ = windowWidth_ - 2 * overview_areamargin_;
	overview_areaheight_ = overview_d_size_ * 4;
	drawHeight_areaP_ = overview_d_size_ * overviewP_rowShow_;
	drawHeight_areaN_ = overview_d_size_ * overviewN_rowShow_;

	updateScrollBars();
}

//--------------------------------------------------------------
void ofApp::inputHistory()
{
	if (canForward_)
	{
		int iter = historysize_ - 1 - nowhistory_;
		for (int i = 0; i < iter; ++i)
		{
			canForward_ = false;
			numberhistory_.pop_back();
		}
	}

	numberhistory_.push_back(number_main_);
	historysize_ = numberhistory_.size();

	if (historysize_ > 1)
	{
		canBack_ = true;
		nowhistory_ = historysize_ - 1;
	}
	writelog();
}

//--------------------------------------------------------------
void ofApp::writelog()
{
	std::vector<int> candidate_active, candidate_origin, candidate_main, candidate_visualrank;

	candidate_active.resize(picnum_);
	candidate_origin.resize(picnum_);
	candidate_main.resize(picnum_);
	candidate_visualrank.resize(picnum_);

	for (int i = 0; i < picnum_; ++i)
	{
		int num_active = number_active_[i];
		int num_origin = number_origin_[i];
		int num_main = number_main_[i];
		int num_visualrank = number_visualrank_[i];

		candidate_active[i] = num_active;
		candidate_origin[i] = num_origin;
		candidate_main[i] = num_main;
		candidate_visualrank[i] = num_visualrank;

		candidatehistory_.push_back(num_active);
	}
	database_->setHistory(candidatehistory_);
	logger_active_->writeCandidate(candidate_active);
	logger_origin_->writeCandidate(candidate_origin);
	logger_main_->writeCandidate(candidate_main);
	logger_visualrank_->writeCandidate(candidate_visualrank);
}

//--------------------------------------------------------------
void ofApp::initRange(const int& begin, const int& end)
{
	picA_ = begin;
	picB_ = end;
	picnum_ = picB_ - picA_ + 1;
}

//--------------------------------------------------------------
void ofApp::sizeChanged()
{
	d_size_ = (windowWidth_ - leftsize_ - ScrollBarWidth_) / colShow_;

	if (d_size_ < 1)
	{
		d_size_ = 1;
		colShow_ = windowWidth_ - leftsize_ - ScrollBarWidth_;
	}

	std::vector<int>* sList;
	if (isactive_)
		sList = &showList_active_;
	else if (ismain_)
		sList = &showList_main_;
	else if (isvisualrank_)
		sList = &showList_visualrank_;
	else if (isorigin_)
		sList = &showList_origin_;

	rowShow_ = (sList->size() + colShow_ - 1) / colShow_;
	if (rowShow_ < 1)
		rowShow_ = 1;
	bottom_ = - uppersize_ - (d_size_ * rowShow_) + windowHeight_;

	if (d_size_ * rowShow_ < windowHeight_)
		rowshort_ = true;
	else
		rowshort_ = false;

	if (len_positives_ > 0)
		overviewP_rowShow_ = (positives_.size() + overview_colShow_ - 1) / overview_colShow_;
	else
		overviewP_rowShow_ = 0;

	if (len_negatives_ > 0)
		overviewN_rowShow_ = (negatives_.size() + overview_colShow_ - 1) / overview_colShow_;
	else
		overviewN_rowShow_ = 0;
}

//--------------------------------------------------------------
// Update the displayed size, scroll size, etc. of the scroll bars
// (Called when the window size or the image size is changed)
void ofApp::updateScrollBars()
{
	if (drawHeight_areaA_ < vscroll_areaA_.bar_length())
	{
		vscroll_areaA_.bar_length(drawHeight_areaA_);
		vscroll_areaA_.max(0);
		vscroll_areaA_.change_by_bar(0);
		vscroll_areaA_.current(0);
	}
	else
	{
		vscroll_areaA_.max(drawHeight_areaA_ - vscroll_areaA_.bar_length());
		vscroll_areaA_.change_by_bar(vscroll_areaA_.max() / 4);
		vscroll_areaA_.current(0);
	}
}

//--------------------------------------------------------------
void ofApp::initializeBars()
{
	// All results viewer.
	scroll_areaA_ = 0;
	vscroll_areaA_.min(0);
	vscroll_areaA_.bar_width(ScrollBarWidth_);
	vscroll_areaA_.bar_length(windowHeight_ - uppersize_);
	vscroll_areaA_.bar_pos_widthdir(windowWidth_ - ScrollBarWidth_);
	vscroll_areaA_.bar_pos_lengthdir(uppersize_);
	vscroll_areaA_.change_by_button(30);
	vscroll_areaA_.current(0);
}

//--------------------------------------------------------------
void ofApp::calculateHoldingOriginPoint()
{
	int x, y;
	x = dragx_ - d_size_ / 2;
	y = dragy_ - d_size_ / 2;

	holding_x_ = x;
	holding_y_ = y;
}

//--------------------------------------------------------------
int ofApp::vector_finder(std::vector<int>& vec, int number)
{
	if (vec.size() > 0)
	{
		auto itr = std::find(vec.begin(), vec.end(), number);
		size_t index = std::distance(vec.begin(), itr);
		if (index != vec.size())
			return index;	// If the number exists in the vector.
		else
			return -1;		// If the number does not exist in the vector.
	}
	else
		return -1;
}

//--------------------------------------------------------------
void ofApp::put_time(std::string& time_str)
{
    time_t     current;
    struct tm  *local;

    time(&current);
    local = localtime(&current);

    std::string year = toString<int>(local->tm_year + 1900);
    std::string month = toString<int>(local->tm_mon);
    std::string day = toString<int>(local->tm_mday);
    std::string hour = toString<int>(local->tm_hour);
    std::string min = toString<int>(local->tm_min);
    std::string sec = toString<int>(local->tm_sec);
    std::string delim = "-";

    time_str = year + delim + month + delim + day + delim + hour + delim + min + delim + sec;
}

//--------------------------------------------------------------
void ofApp::showProcessingTime()
{
	float others = process_time_ - trainer_->process_time_ - search_->process_time_
			- rerank_->process_time_ - visualrank_->process_time_;

	std::cout << "-------------------------- Processing Time --------------------------" << std::endl;
	std::cout << "Online Training (Main + LOOCV + Selection): " << trainer_->process_time_ << " sec." << std::endl;
	std::cout << "Searching (Rocchio + NGT): " << search_->process_time_ << " sec." << std::endl;
	std::cout << "Reranking (Main): " << rerank_->process_time_ + visualrank_->process_time_ << " sec." << std::endl;
	std::cout << "Reranking (VisualRank): " << visualrank_->process_time_ << " sec." << std::endl;
	std::cout << "Others: " << others << " sec." << std::endl;
	std::cout << "Total: " << process_time_ << " sec." << std::endl;
	std::cout << "---------------------------------------------------------------------" << std::endl;
}

//--------------------------------------------------------------
void ofApp::autoselect_negative()
{
	for (int i = 0; i < picnum_; ++i)
	{
		int number = showList_active_[i];
		int check_duplication_P = vector_finder(positives_, number);
		int check_duplication_N = vector_finder(negatives_, number);

		if (check_duplication_P < 0 && check_duplication_N < 0)
		{
			ofImage image = loader_->picture_[number];
			negatives_.push_back(number);
			negative_images_.push_back(image);
			len_negatives_ = negatives_.size();
		}
	}
	std::cout << "[ofApp] Auto sample selection." << std::endl;
}

