#include "ofApp.h"


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
	picB_ = 200;
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
	colShow_ = 8;
	d_size_ = (initWidth_ - leftsize_ - ScrollBarWidth_) / colShow_;
	overview_margin_ = 15;
	width_areaA_ = 0;
	rowshort_ = false;

	//-----------------------------------------
	// Input Query.
	clickflag_ = false;
	selectList_.resize(picnum_);
	for (int i = 0; i < picnum_; ++i)
		selectList_[i] = false;
	selected_num_ = 0;

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
	matrixFile_ = binData_ + "cfd/cfd-vgg.tsv";
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
	logdir_ = "/home/yugo/workspace/Interface/bin/log/";
	candidatefile_main_ = logdir_ + "candidate_train.txt";
	candidatefile_removed_ = logdir_ + "candidate_train-removed.txt";
	candidatefile_eval_ = logdir_ + "candidate_nontrain.txt";
	init_candidatefile_ = binData_ + "cfd/initialize.txt";

	// Online Training Settings.
	pysettingfile_ = binData_ + "cfd/py_setting.txt";
	samplefile_ = logdir_ + "feedback.txt";
	pythonfile_ = "/home/yugo/workspace/Interface/trainer/trainmodel.py";

	//-----------------------------------------
	// Retrieval results.
	isremove_ = true;
	iseval_ = false;

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
	holdImgNum_ = -1;
	holding_x_ = -1;
	holding_y_ = -1;

	//-----------------------------------------
	// Others.
	epoch_ = 0;
	isLoaded_ = false;
	isSearchedAll_ = false;
	canSearch_ = false;
	draw_epoch_ = false;
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
	non_removebutton1_.load(binData_ + "items/non-remove1.png");
	non_removebutton2_.load(binData_ + "items/non-remove2.png");
	removebutton1_.load(binData_ + "items/remove1.png");
	removebutton2_.load(binData_ + "items/remove2.png");
	evalbutton1_.load(binData_ + "items/eval1.png");
	evalbutton2_.load(binData_ + "items/eval2.png");
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
	onPaint(showList_removed_);
	firstshowlist_ = showList_;

	// Setup NGT.
	ngt_ = new Search();
	ngt_->setup(indexFile_);

	// Load image features (multi thread).
	loading_ = new NowLoading();
	loading_->setMatFile(matrixFile_);
	loading_->setRow(row_);
	loading_->startThread();

	// Setup writer.
	if (!isFileexists(logdir_))
		mkdir(logdir_.c_str(), 0777);
	samplewriter_ = new SampleWriter(samplefile_);
	samplewriter_->write_init();

	// Setup online trainer.
	trainer_ = new Trainer;
	trainer_->setup(pythonfile_);

	std::cout << "[Setting] NGT-index: \"" << indexFile_ << "\"" << std::endl;
	std::cout << "[Setting] Matrix file: \"" << matrixFile_ << "\"" << std::endl;
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
	delete ngt_;
	delete samplewriter_;
	delete logger_main_;
	delete logger_removed_;
	delete logger_eval_;
	delete trainer_;
}

//--------------------------------------------------------------
void ofApp::update()
{
	ofBackground(ofColor(0x000000));

	if (loading_->done_ && !isLoaded_)
	{
		loading_->stopThread();
		loading_->unlock();
		isLoaded_ = true;
		ngt_->setMatrix(loading_->mat_);
		canSearch_ = true;

		logger_main_ = new Logger;
		logger_removed_ = new Logger;
		logger_eval_ = new Logger;
		logger_main_->setup(candidatefile_main_, pysettingfile_, npyFile_, loading_->col_);
		logger_removed_->setup(candidatefile_removed_, pysettingfile_, npyFile_, loading_->col_);
		logger_eval_->setup(candidatefile_eval_, pysettingfile_, npyFile_, loading_->col_);
		logger_main_->writePySetting();
		number_main_ = database_->number_;
		number_eval_ = number_main_;
		writelog();
	}

	if (trainer_->isTrained_)
	{
		endtime_trainer_ = clock();
		std::cout << "[ofApp] Training time: " << (double)(endtime_trainer_ - starttime_trainer_) / CLOCKS_PER_SEC << "sec." << std::endl;
		trainer_->stopThread();
		trainer_->isTrained_ = false;
		ngt_->setExtracter(trainer_->extracter_);
		ngt_->setInput_multi(selectedquery_, nonselectedquery_);
		ngt_->train_ = true;
		ngt_->startThread();
		starttime_ngt_ = clock();
	}

	if (!isSearchedAll_ && ngt_->isSearched_)
	{
		ngt_->stopThread();
		ngt_->isSearched_ = false;

		if (ngt_->train_)
		{
			ngt_->getNumber(&number_main_);
			ngt_->train_ = false;
			ngt_->startThread();
		}
		else
		{
			ngt_->getNumber(&number_eval_);
			isSearchedAll_ = true;
		}
	}
	else if (isSearchedAll_)
	{
		endtime_ngt_ = clock();
		std::cout << "[ofApp] Searching time: " << (double)(endtime_ngt_ - starttime_ngt_) / CLOCKS_PER_SEC << "sec." << std::endl;
		database_->setNumber(number_main_);
		database_->setNumber_eval(number_eval_);
		calculate();
		onPaint(showList_removed_);
		inputHistory();

		draw_epoch_ = true;
		selectedquery_.clear();
		nonselectedquery_.clear();
		std::vector<int>().swap(selectedquery_);
		std::vector<int>().swap(nonselectedquery_);
		selected_num_ = 0;

		for (int i = 0; i < (int) selectList_.size(); ++i)
			selectList_[i] = false;

		canSearch_ = true;
		isSearchedAll_ = false;
		endtime_ = clock();
		std::cout << "[ofApp] Total processing time: " << (double)(endtime_ - starttime_) / CLOCKS_PER_SEC << "sec." << std::endl;
		std::cout << "================================" << std::endl;

		vscroll_areaA_.current(0);
	}

	scroll_areaA_ = - vscroll_areaA_.current();
	vscroll_areaA_.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	if (!isLoaded_)
	{
		std::string nowload = "Now Loading...\n";
		std::string process = ofToString(loading_->count_ * 100 / loading_->row_) + "%";
		font_.drawString(nowload + process, 15, 36);
	}

	if (draw_epoch_ && epoch_ > 0)
	{
		std::string search = "Search: ";
		std::string epoch = ofToString(epoch_);
		font_.drawString(search + epoch, 10, 150);
	}

	ofImage img;
	const int len = loader_->row_;

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
		else if (isremove_)
		{
			if (i == mouseover_ || selectList_[i])
			{
				ofSetColor(255);
				img.draw(drawx, drawy, d_size_, d_size_);
			}
			else
			{
				ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 130.0f));
				img.draw(drawx, drawy, d_size_, d_size_);
			}
		}
		else if (!isremove_)
		{
			ofSetColor(255);
			img.draw(drawx, drawy, d_size_, d_size_);
		}
	}

    ofSetColor(0);
    ofDrawRectangle(1000, 0, 600, 40);

	ofSetColor(255);
//	// Forward button.
//	if (!canBack_)
//		backbutton0_.draw(backbuttonposx_, buttonposy_line1_, historybuttonwidth_, buttonheight_);
//	else
//		backbutton1_.draw(backbuttonposx_, buttonposy_line1_, historybuttonwidth_, buttonheight_);
//
//	// Back button.
//	if (!canForward_)
//		forwardbutton0_.draw(forwardbuttonposx_, buttonposy_line1_, historybuttonwidth_,
//				buttonheight_);
//	else
//		forwardbutton1_.draw(forwardbuttonposx_, buttonposy_line1_, historybuttonwidth_, buttonheight_);

	backbutton0_.draw(backbuttonposx_, buttonposy_line1_, historybuttonwidth_, buttonheight_);
	forwardbutton0_.draw(forwardbuttonposx_, buttonposy_line1_, historybuttonwidth_, buttonheight_);
	searchbutton1_.draw(searchbuttonposx_, buttonposy_line1_, searchbuttonwidth_, buttonheight_);

	if (!isremove_ && !iseval_)
	{
		non_removebutton2_.draw(non_removebuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_);
		removebutton1_.draw(removebuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_);
		evalbutton1_.draw(evalbuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_);
	}
	else if (isremove_ && !iseval_)
	{
		non_removebutton1_.draw(non_removebuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_);
		removebutton2_.draw(removebuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_);
		evalbutton1_.draw(evalbuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_);
	}
	else if (!isremove_ && iseval_)
	{
		non_removebutton1_.draw(non_removebuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_);
		removebutton1_.draw(removebuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_);
		evalbutton2_.draw(evalbuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_);
	}

	if (!canSearch_)
	{
		std::string nowsearch = "Now Searching...";
		font_.drawString(nowsearch, 15, 36);
	}

	vscroll_areaA_.draw();

	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 130.0f));
	if (isInside_areaP_)
	{
		ofDrawRectangle(overview_margin_, overviewP_posy_, overview_width_, overview_height_);
	}
	else if (isInside_areaN_)
	{
		ofDrawRectangle(overview_margin_, overviewN_posy_, overview_width_, overview_height_);
	}
	ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
	ofDrawRectangle(overview_margin_, overviewR_posy_, overview_width_, overview_height_);

	if (isLoaded_)
	{
		ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
		std::string positive = "Positive Sample";
		std::string negative = "Negative Sample";
		std::string reliability = "Reliability Graph";

		int perHeight = windowHeight_ / 3;
		int posy_positive_txt = 25;
		int posy_negative_txt = 25 + perHeight;
		int posy_reliability_txt = 25 + 2 * perHeight;

		overviewP_posy_ = posy_positive_txt + 10;
		overviewN_posy_ = posy_negative_txt + 10;
		overviewR_posy_ = posy_reliability_txt + 10;
		overview_width_ = leftsize_ - 2 * overview_margin_;
		overview_height_ = d_size_ * 4;

		font_.drawString(positive, overview_margin_, posy_positive_txt);
		font_.drawString(negative, overview_margin_, posy_negative_txt);
		font_.drawString(reliability, overview_margin_, posy_reliability_txt);
	}

	if (isHolding_areaA_ && isHoldAndDrag_)
	{
		ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 255.0f));
		ofImage holdImg;
		holdImg = loader_->picture_[holdImgNum_];
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
			std::string savepath = "/home/yugo/workspace/Interface/bin/snapshot/" + ofGetTimestampString() + ".jpeg";
			img.save(savepath);
			std::cout << "[ofApp] Saved snapshot image." << std::endl;
			std::cout << "[ofApp] --> \"" << savepath << "\"" << std::endl;
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::back()
{
	ishistory_ = true;
	backhistory();
	database_->setNumber(numberhistory_[nowhistory_]);

	calculate();
	onPaint(showList_);

	selectedquery_.clear();
	nonselectedquery_.clear();
	std::vector<int>().swap(selectedquery_);
	std::vector<int>().swap(nonselectedquery_);
	selected_num_ = 0;

	for (int i = 0; i < (int) selectList_.size(); ++i)
		selectList_[i] = false;
}

//--------------------------------------------------------------
void ofApp::forward()
{
	ishistory_ = true;
	forwardhistory();
	database_->setNumber(numberhistory_[nowhistory_]);

	calculate();
	onPaint(showList_);

	selectedquery_.clear();
	nonselectedquery_.clear();
	std::vector<int>().swap(selectedquery_);
	std::vector<int>().swap(nonselectedquery_);
	selected_num_ = 0;

	for (int i = 0; i < (int) selectList_.size(); ++i)
		selectList_[i] = false;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
	const int x_dash = x - leftsize_;
	const int y_dash = y - scroll_areaA_ - uppersize_;

	if (x_dash >= 0 && y_dash >= 0 && uppersize_ < y)
		mouseover_ = x_dash / d_size_ + y_dash / d_size_ * colShow_;
	else
		mouseover_ = -1;

	if ((windowWidth_ - ScrollBarWidth_ - 2) <= x)
		mouseover_ = -1;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	dragx_ = x;
	dragy_ = y;
	click_ = false;

	if (vscroll_areaA_.mouseDragged(x, y))
		return;

	if (isremove_ && isHolding_areaA_)
	{
		isHoldAndDrag_ = true;
		holdImgNum_ = mouseover_;
		calculateHoldingOriginPoint();

		if (isInsideDragingArea(leftsize_, uppersize_, width_areaA_, windowHeight_ - uppersize_))
		{
			isInside_areaA_ = true;
			isInside_areaP_ = false;
			isInside_areaN_ = false;
		}
		else if (isInsideDragingArea(overview_margin_, overviewP_posy_, overview_width_, overview_height_))
		{
			isInside_areaA_ = false;
			isInside_areaP_ = true;
			isInside_areaN_ = false;
		}
		else if (isInsideDragingArea(overview_margin_, overviewN_posy_, overview_width_, overview_height_))
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
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	clickx_ = x;
	clicky_ = y;
	click_ = true;
	isHoldAndDrag_ = false;

	if (clickx_ < leftsize_)
		leftsideclick_ = true;
	else
		leftsideclick_ = false;

	if (vscroll_areaA_.mousePressed(clickx_, clicky_))
		return;

	if (isremove_ && isClickedArea(leftsize_, uppersize_, width_areaA_, windowHeight_ - uppersize_))
		isHolding_areaA_ = true;
	else if (isremove_ && isClickedArea(overview_margin_, overviewP_posy_, overview_width_, overview_height_))
		isHolding_areaP_ = true;
	else if (isremove_ && isClickedArea(overview_margin_, overviewN_posy_, overview_width_, overview_height_))
		isHolding_areaN_ = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
//	clickx_ = -1;
//	clicky_ = -1;
	dragx_ = -1;
	dragy_ = -1;
	holding_x_ = -1;
	holding_y_ = -1;
	isHoldAndDrag_ = false;
	isHolding_areaA_ = false;
	isHolding_areaP_ = false;
	isHolding_areaN_ = false;
	isInside_areaA_ = false;
	isInside_areaP_ = false;
	isInside_areaN_ = false;
	holdImgNum_ = -1;

	if (x < leftsize_ || (leftsize_ + width_areaA_) < x || y < uppersize_)
		mouseover_ = -1;

	if (click_ == true)
	{
		const int x_dash = x - leftsize_;
		const int y_dash = y - scroll_areaA_ - uppersize_;

		if (canSearch_)
		{
			std::vector<int> *sList = &showList_removed_;

			if (isClickedArea(removebuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_))
			{
				isremove_ = true;
				iseval_ = false;
				onPaint(showList_removed_);
			}
			else if (isClickedArea(non_removebuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_))
			{
				isremove_ = false;
				iseval_ = false;
				onPaint(showList_);
			}
#ifndef EXPERIMENT
			else if (isClickedArea(evalbuttonposx_, buttonposy_line1_, removebuttonwidth_, buttonheight_))
			{
				isremove_ = false;
				iseval_ = true;
				onPaint(showList_nonTrain_);
			}
#endif

			if (isremove_)
			{
				if (x_dash >= 0 && y_dash >= 0 && y > uppersize_ && x < (windowWidth_ - ScrollBarWidth_))
				{
					if (x < (windowWidth_ - ScrollBarWidth_ - 2))
					{
						const int clickpos = mouseover_;

						if (clickpos < (int) sList->size())
						{
							if (button == 0 && isLoaded_)
							{
								if (!selectList_[clickpos])
								{
									selected_num_++;
									selectList_[clickpos] = true;
								}
								else
								{
									selected_num_--;
									selectList_[clickpos] = false;
								}
							}
						}
					}
				}

//				if (canBack_ && isClickedArea(backbuttonposx_, buttonposy_line1_, historybuttonwidth_, buttonheight_))
//					back();
//
//				if (canForward_ && isClickedArea(forwardbuttonposx_, buttonposy_line1_, historybuttonwidth_, buttonheight_))
//					forward();

				if (isClickedArea(searchbuttonposx_, buttonposy_line1_, searchbuttonwidth_, buttonheight_))
				{
					if (selected_num_ != 0)
					{
						epoch_++;
						draw_epoch_ = false;
						std::cout << "[ofApp] " << epoch_ << " feedbacks." << std::endl;

						starttime_ = clock();
						for (int i = 0; i < (int) selectList_.size(); ++i)
						{
							const int No = (*sList)[i];

							if (selectList_[i])
								selectedquery_.push_back(No);
							else
								nonselectedquery_.push_back(No);
						}

						clickflag_ = true;
						samplewriter_->write(selectedquery_, nonselectedquery_);
						trainer_->startThread();
						starttime_trainer_ = clock();
						canSearch_ = false;
					}
					else
					{
						std::cerr << "[Warning] Please select queries." << std::endl;
					}
				}
			}
			else
			{
				if (isClickedArea(searchbuttonposx_, buttonposy_line1_, searchbuttonwidth_, buttonheight_))
				{
					std::cerr << "[Warning] Cannot search. Please search on state \"A\"" << std::endl;
				}
			}
		}
		else if (y_dash >= 0 && y > uppersize_ && x_dash >= 0 && x > leftsize_)
		{
			std::cerr << "[Warning] Cannot select. please wait." << std::endl;
		}
	}

	clickx_ = 0;
	clicky_ = 0;
	click_ = false;

	if (vscroll_areaA_.mouseReleased(x, y))
		return;
}

//--------------------------------------------------------------
bool ofApp::isClickedArea(float x, float y, float w, float h)
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
void ofApp::mouseEntered(int x, int y)
{
	isInsideWindow_ = true;
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
	mouseover_ = -1;
	isInsideWindow_ = false;
	isHoldAndDrag_ = false;
	isHolding_areaA_ = false;
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
	database_->makeShowList(picA_, picB_);
	showList_ = database_->getShowList();

	database_->makeShowList_removed(picA_, picB_);
	showList_removed_ = database_->getShowList();

	database_->makeShowList_eval(picA_, picB_);
	showList_nonTrain_ = database_->getShowList();
}

//--------------------------------------------------------------
void ofApp::onPaint(const std::vector<int>& list)
{
	sizeChanged();
	loader_->setShowList(list);
	loader_->load();
	ishistory_ = false;

	int len = loader_->row_;
	int draw_rows;

	if (len % colShow_ > 0)
		draw_rows = len / colShow_ + 1;
	else
		draw_rows = len / colShow_;

	width_areaA_ = d_size_ * colShow_;
	drawHeight_areaA_ = d_size_ * draw_rows;
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
	std::vector<int> candidate_train, candidate_ntrain;;
	candidate_train.resize(picnum_);
	candidate_ntrain.resize(picnum_);
	for (int i = 0; i < picnum_; ++i)
	{
		int num_train = number_main_[i];
		int num_rm = database_->number_removed_[i];
		int num_ntrain = number_eval_[i];
		candidate_train[i] = num_train;
		candidate_ntrain[i] = num_ntrain;
		candidatehistory_.push_back(num_rm);
	}
	database_->setHistory(candidatehistory_);
	logger_main_->writeCandidate(candidate_train);
	logger_removed_->writeCandidate(database_->number_removed_);
	logger_eval_->writeCandidate(candidate_ntrain);
}

//--------------------------------------------------------------
void ofApp::backhistory()
{
	if (nowhistory_ < 2)
	{
		canForward_ = true;
		canBack_ = false;
		nowhistory_ = 0;
	}
	else
	{
		canForward_ = true;
		canBack_ = true;
		nowhistory_ += -1;
	}
}

//--------------------------------------------------------------
void ofApp::forwardhistory()
{
	if (nowhistory_ > historysize_ - 3)
	{
		canForward_ = false;
		canBack_ = true;
		nowhistory_ = historysize_ - 1;
	}
	else
	{
		canForward_ = true;
		canBack_ = true;
		nowhistory_ += 1;
	}
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
	if (isremove_ && !iseval_)
		sList = &showList_removed_;
	else if (!isremove_ && !iseval_)
		sList = &showList_;
	else if (!isremove_ && iseval_)
		sList = &showList_nonTrain_;

	rowShow_ = (sList->size() + colShow_ - 1) / colShow_;
	if (rowShow_ < 1)
		rowShow_ = 1;
	bottom_ = - uppersize_ - (d_size_ * rowShow_) + windowHeight_;

	if (d_size_ * rowShow_ < windowHeight_)
		rowshort_ = true;
	else
		rowshort_ = false;
}

//--------------------------------------------------------------
// Update the displayed size, scroll size, etc. of the scroll bars
// (Called when the window size or the image size is changed)
void ofApp::updateScrollBars()
{
	vscroll_areaA_.bar_length(windowHeight_ - uppersize_);
	vscroll_areaA_.max(std::max<float>(drawHeight_areaA_ - vscroll_areaA_.bar_length(), 0));
	vscroll_areaA_.change_by_bar(vscroll_areaA_.max() / 10);
}

//--------------------------------------------------------------
void ofApp::initializeBars()
{
	scroll_areaA_ = 0;
	vscroll_areaA_.min(0);
	vscroll_areaA_.bar_width(ScrollBarWidth_);
	vscroll_areaA_.bar_pos_widthdir(windowWidth_ - ScrollBarWidth_);
	vscroll_areaA_.bar_pos_lengthdir(uppersize_);
	vscroll_areaA_.change_by_button(30);
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
