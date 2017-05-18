#include "ofApp.h"
#define EXPERIMENT


void ofApp::initparam()
{
	//-----------------------------------------
	// 履歴
	historysize_ = 0;
	backcount_ = 0;
	entercount_ = 0;
	ishistory_ = false;
	canBack_ = false;
	canEnter_ = false;
	nowhistory_ = -1;

	//-----------------------------------------
	// 表示枚数
	picA_ = 1;
	picB_ = 25;
	picnum_ = picB_ - picA_ + 1;

	//-----------------------------------------
	// GUI設定
	guiHeight_ = 5400;
	guiScrollarea_ = 280;

#ifndef EXPERIMENT
	buttonposy_ = 240;
#else
	buttonposy_ = 5;
#endif

	//-----------------------------------------
	// データベースからの返り値
	isLoaded_ = false;

	//-----------------------------------------
	// マウス＆キーボード
	clickx_ = 0;
	clicky_ = 0;
	dragx_ = 0;
	dragy_ = 0;
	dragh_ = 0;
	dragw_ = 0;

	click_ = false;
	dontmove_ = false;
	goback_ = false;
	leftside_ = false;
	velocity_ = 0;

	presstime_ = 0;

	//-----------------------------------------
	// 画像表示関連
	colShow_ = 5;
	d_size_ = (initWidth_ - leftsize_) / colShow_;
	rowshort_ = false;
	mouseover_ = -1;

	//-----------------------------------------
	// クエリ関連
	clickNo_ = -1;
	clickflag_ = false;
	selected_num_ = 0;

	selectList_.resize(picnum_);
	for (int i = 0; i < picnum_; ++i)
		selectList_[i] = false;

	//-----------------------------------------
	// その他/ウィンドウ設定
	windowWidth_ = initWidth_;
	windowHeight_ = initHeight_;

	// その他/フォント
	ttf_ = "fonts/RictyDiminished-Bold.ttf";

	// その他/データベース情報
	datasetdir_ = "/home/yugo/Desktop/dataset/";
	dataset_ = "cfd-cropped";
	nameFile_ = datasetdir_ + dataset_ + "/images_selected.txt";
	matrixfile_ = "bin/data/cfd/cfd-vgg.tsv";
	indexFile_ = "bin/data/cfd/index-angle";

	// 探索評価関連
	logdir_ = "bin/log/";
	person_logfile_ = logdir_ + "person.txt";
	candidate_logfile_ = logdir_ + "candidate.txt";
}

//--------------------------------------------------------------
bool ofApp::isFileexists(const std::string& filepath)
{
	struct stat st;
	int ret = stat(filepath.c_str(), &st);
	if (ret == 0)
	{
		return true;
	}
	else
		return false;
}

//--------------------------------------------------------------
void ofApp::setup()
{
	initparam();
	ofSetWindowShape(windowWidth_, windowHeight_);
	font_.load(ttf_, fontsize_);
	backbutton0_.load("items/cantBack.png");
	backbutton1_.load("items/canBack1.png");
	enterbutton0_.load("items/cantEnter.png");
	enterbutton1_.load("items/canEnter1.png");
	searchbutton1_.load("items/search1.png");
	searchbutton2_.load("items/search2.png");

	// データベース情報取得
	input_ = new DataBase();
	input_->setup(nameFile_);
	row_ = input_->getRow();
	input_->getName(&name_);
	input_->getPersonID(&person_ids_);

	guiSetup();

	// 画像読み込み
	loader_ = new ImageLoader();
	loader_->setRow(row_);
	loader_->setName(name_);

	calculate();
	onPaint();

	// 初期表示リストの登録
	firstshowlist_ = showList_;

	// NGTセットアップ
	ngt_ = new Search();
	ngt_->setup(indexFile_);

	// 別スレッドで特徴量読み込み
	loading_ = new NowLoading();
	loading_->setMatFile(matrixfile_);
	loading_->setRow(row_);
	loading_->startThread();

	// 検索ログ
	if (!isFileexists(logdir_))
	{
		mkdir(logdir_.c_str(), 0777);
	}

	// 初期表示の記録
	writelog(1);
}

//--------------------------------------------------------------
void ofApp::guiSetup()
{
	gui_ = new ofxUIScrollableCanvas(0, 0, guiWidth_, guiHeight_);
	gui_->setScrollAreaHeight(guiScrollarea_);
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
	delete input_;
	delete loading_;
	delete loader_;
	delete ngt_;
}

//--------------------------------------------------------------
void ofApp::update()
{
	ofBackground(ofColor(0x000000));
	//一番上に戻るとき
	if (goback_ && dragh_ <= 0)
	{
		//速度は0にしておく
		velocity_ = 0.0f;

		const float v = (float) ((-dragh_ / 20.0f) + 1.0f);
		dragh_ += v;
		if (dragh_ > 0)
		{
			dragh_ = 0;
			goback_ = false;
		}
	}

	//速度減衰
	velocity_ = velocity_ * 0.98f;

	//速度が一定値より低かったら0にする
	if (-0.5f < velocity_ && velocity_ < 0.5f)
		velocity_ = 0.0f;

	//速度を加算
	dragh_ += velocity_;

	//draghが0より大きくならないように
	if (dragh_ > 0)
		dragh_ = 0;

	//draghがbottomより小さくならないように
	else if (dragh_ < bottom_)
	{
		//行数が少ない場合は動かさない
		if (rowshort_)
		{
			dragh_ = 0;
			velocity_ = 0.0f;
		}
		else
			dragh_ = bottom_;
	}

	//データベースが読み込めたかどうか
	if (loading_->done_)
	{
		loading_->stopThread();
		loading_->done_ = false;
		isLoaded_ = true;
		ngt_->setMatrix(loading_->mat_);
	}

	if (click_)
		presstime_++;
}

//--------------------------------------------------------------
void ofApp::draw()
{
	if (!isLoaded_)
	{
		std::string nowload = "Now Loading...\n";
		std::string process = ofToString(loading_->count_ * 100 / loading_->row_) + "%";
		font_.drawString(nowload + process, inputImgposx_ + 5, inputImgposy_ + 22);
	}

	ofImage img;
	const int len = (int) showList_.size();
	for (int i = 0; i < len; ++i)
	{
		const int j = i % colShow_;
		const int k = i / colShow_;

		if (windowHeight_ < topsize_ + d_size_ * k + dragh_)
			break;
		else if (0 > topsize_ + d_size_ * (k + 1) + dragh_)
			continue;

		int drawx = leftsize_ + d_size_ * j;
		int drawy = topsize_ + d_size_ * k + dragh_;

		img = loader_->picture_[i];

		if (!img.isAllocated())
			break;
		else if (i == mouseover_ || selectList_[i])
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

	ofSetColor(255);

	if (isLoaded_)
	{
#ifndef EXPERIMENT
		ofDrawRectangle(5, inputImgposy_ - 5, 5, inputImgsize_ + 10);
		ofDrawRectangle(5, inputImgposy_ - 5, inputImgsize_ + 10, 5);
		ofDrawRectangle(5, inputImgposy_ + inputImgsize_, inputImgsize_ + 10, 5);
		ofDrawRectangle(inputImgposx_ + inputImgsize_, inputImgposy_ - 5, 5, inputImgsize_ + 10);

		if (clickflag_)
		{
			//ofSetHexColor(0xCCCCCC);
			picture_.draw(inputImgposx_, inputImgposy_, inputImgsize_, inputImgsize_);
		}
#endif

		// 戻るボタン
		if (!canBack_)
			backbutton0_.draw(backbuttonposx_, buttonposy_, historybuttonsize_, historybuttonsize_);
		else
			backbutton1_.draw(backbuttonposx_, buttonposy_, historybuttonsize_, historybuttonsize_);

		// 進むボタン
		if (!canEnter_)
			enterbutton0_.draw(enterbuttonposx_, buttonposy_, historybuttonsize_, historybuttonsize_);
		else
			enterbutton1_.draw(enterbuttonposx_, buttonposy_, historybuttonsize_, historybuttonsize_);

		// submitボタン
		searchbutton1_.draw(searchbuttonposx_, buttonposy_, searchbuttonwidth_, historybuttonsize_);
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
			std::string savepath = "./snapshot/" + ofGetTimestampString() + ".png";
			img.save(savepath);
			std::cout << "[ofApp] saved snapshot image." << std::endl;
			break;
		}
		case 8: // BackSpace
		{
			if (canBack_)
				back();
			else
				std::cout << "[warning] can't back." << std::endl;
			break;
		}
		case 13: // Enter
		{
			if (canEnter_)
				enter();
			else
				std::cout << "[warning] can't enter." << std::endl;
			break;
		}
		case 114: // Ctrl+r
		{
			if (clickflag_)
			{
				std::cout << "[ofApp] restart" << std::endl;

				historysize_ = 0;
				backcount_ = 0;
				entercount_ = 0;
				ishistory_ = false;
				canBack_ = false;
				canEnter_ = false;
				nowhistory_ = -1;
				clickflag_ = false;
				ngt_->phase_ = 0;

				showList_ = firstshowlist_;
				onPaint();

#ifndef EXPERIMENT
				queryhistory_.clear();
				personhistory_.clear();
				std::vector<int>().swap(queryhistory_);
				std::vector<int>().swap(personhistory_);
#endif

				numberhistory_.clear();
				selectedquery_.clear();
				nonselectedquery_.clear();

				std::vector<std::vector<int>>().swap(numberhistory_);
				std::vector<int>().swap(selectedquery_);
				std::vector<int>().swap(nonselectedquery_);
				selected_num_ = 0;

				for (int i = 0; i < (int) selectList_.size(); ++i)
					selectList_[i] = false;
			}
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::back()
{
	ishistory_ = true;
	backhistory();
	input_->setNumber(numberhistory_[nowhistory_]);

#ifndef EXPERIMENT
	initRange(2, 26);
#else
	initRange(1, 25);
#endif

	calculate();
	onPaint();

	selectedquery_.clear();
	nonselectedquery_.clear();
	std::vector<int>().swap(selectedquery_);
	std::vector<int>().swap(nonselectedquery_);
	selected_num_ = 0;

	for (int i = 0; i < (int) selectList_.size(); ++i)
		selectList_[i] = false;
}

//--------------------------------------------------------------
void ofApp::enter()
{
	ishistory_ = true;
	enterhistory();
	input_->setNumber(numberhistory_[nowhistory_]);

#ifndef EXPERIMENT
	initRange(2, 26);
#else
	initRange(1, 25);
#endif

	calculate();
	onPaint();

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
	velocity_ = 0.0f;
	presstime_ = 0;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
	const int y_dash = y - dragh_ - topsize_;
	const int x_dash = x - dragw_ - leftsize_;

	if (x_dash >= 0 && y_dash >= 0 && topsize_ < y && leftsize_ < x)
		mouseover_ = (x_dash / d_size_) + (y_dash / d_size_) * colShow_; //クリックした場所
	else
		mouseover_ = -1;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	click_ = false;

	if (!dontmove_ && x > leftsize_ && !leftside_)
	{
		dragx_ = x - clickx_;
		dragy_ = y - clicky_;
		dragh_ += dragy_;

		if (dragh_ > 0)
			dragh_ = 0;

		if (dragh_ < bottom_)
		{
			if (rowshort_)
			{
				dragh_ = 0;
				velocity_ = 0.0f;
			}
			else
				dragh_ = bottom_;
		}
		clickx_ = x;
		clicky_ = y;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	if (x < leftsize_)
		leftside_ = true;
	else
		leftside_ = false;

	clickx_ = x;
	clicky_ = y;
	click_ = true;
	dontmove_ = false;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	if (click_ == true)
	{
		velocity_ = 0.0f;
		goback_ = false;

		const int y_dash = y - dragh_ - topsize_;
		const int x_dash = x - leftsize_;

		if (y_dash >= 0 && y > topsize_ && x_dash >= 0 && x > leftsize_)
		{
			const int clickpos = (x_dash / d_size_) + (y_dash / d_size_) * colShow_;	//クリックした場所

			if (clickpos < (int) showList_.size())
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

		if (canBack_ && pressbutton(backbuttonposx_, buttonposy_, historybuttonsize_, historybuttonsize_))
			back();

		if (canEnter_ && pressbutton(enterbuttonposx_, buttonposy_, historybuttonsize_, historybuttonsize_))
			enter();

		if (pressbutton(searchbuttonposx_, buttonposy_, searchbuttonwidth_, historybuttonsize_))
		{
			if (selected_num_ != 0)
			{
				for (int i = 0; i < (int) selectList_.size(); ++i)
				{
					const int No = showList_[i];

					if (selectList_[i])
					{
						selectedquery_.push_back(No);
					}
					else
					{
						nonselectedquery_.push_back(No);
					}
				}

				clickNo_ = selectedquery_[0];
				inputQuery();
				inputHistory();

				selectedquery_.clear();
				nonselectedquery_.clear();
				std::vector<int>().swap(selectedquery_);
				std::vector<int>().swap(nonselectedquery_);
				selected_num_ = 0;

				for (int i = 0; i < (int) selectList_.size(); ++i)
					selectList_[i] = false;
			}
			else
			{
				std::cout << "[warning] please select queries." << std::endl;
			}
		}
	}
	else
		velocity_ = (float) (dragy_);	//カーソル速度取得

	clickx_ = 0;
	clicky_ = 0;
	click_ = false;
	presstime_ = 0;
}

//--------------------------------------------------------------
bool ofApp::pressbutton(float x, float y, float w, float h)
{
	if (clickx_ >= x && clicky_ >= y)
	{
		if (clickx_ <= x + w && clicky_ <= y + h)
			return true;
	}
	return false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
	mouseover_ = -1;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	velocity_ = 0.0f;
	goback_ = false;

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
	input_->makeShowList(picA_, picB_);
	showList_ = input_->getShowList();
	sizeChanged();
}

//--------------------------------------------------------------
void ofApp::onPaint()
{
	loader_->setShowList(showList_);
	loader_->load();

#ifndef EXPERIMENT
	if (clickflag_)
	{
		if (!ishistory_)
			picture_.load(name_[clickNo_]);
		else
			picture_.load(name_[queryhistory_[nowhistory_]]);
	}
#endif

	ishistory_ = false;
}

//--------------------------------------------------------------
void ofApp::inputQuery()
{
	clickflag_ = true;
	goback_ = true;

#ifndef EXPERIMENT
	ngt_->setInput(clickNo_);
#else
	ngt_->setInput_multi(selectedquery_, nonselectedquery_);
#endif

	ngt_->search();
	ngt_->getNumber(&number_);
	input_->setNumber(number_);

#ifndef EXPERIMENT
	initRange(2, 26);
#else
	initRange(1, 25);
#endif

	calculate();
	onPaint();

#ifndef EXPERIMENT
	queryinfo();
#endif
}

//--------------------------------------------------------------
void ofApp::queryname(const std::string& fullpath)
{
	int path_i = fullpath.find("/");
	int path_j = fullpath.rfind("/");
	std::string tempname = fullpath.substr(path_i, path_j);

	int size = tempname.size();
	int path_i2 = tempname.rfind("/") + 1;

	queryname_ = tempname.substr(path_i2, size);
}

//--------------------------------------------------------------
void ofApp::queryinfo()
{
	const std::string fullpath = name_[clickNo_];
	queryname(fullpath);
	std::cout << "[ofApp] person name: " << queryname_ << std::endl;
	std::cout << "[ofApp] person id: " << person_ids_[clickNo_] << std::endl;
	std::cout << "[ofApp] image id: " << clickNo_ << std::endl;
}

//--------------------------------------------------------------
void ofApp::inputHistory()
{
	// 戻ってる途中でクエリをクリックしたら一部の履歴削除
	if (canEnter_)
	{
		int iter = historysize_ - 1 - nowhistory_;
		for (int i = 0; i < iter; ++i)
		{
			canEnter_ = false;

#ifndef EXPERIMENT
			queryhistory_.pop_back();
			personhistory_.pop_back();
#endif

			numberhistory_.pop_back();
		}
	}

#ifdef EXPERIMENT
	queryhistory_.push_back(clickNo_);
	personhistory_.push_back(person_ids_[clickNo_]);
#endif

	numberhistory_.push_back(number_);
	historysize_ = numberhistory_.size();

#ifndef EXPERIMENT
	std::cout << "[ofApp] person id history: ";
	for (int i = 0; i < historysize_; ++i)
	{
		std::cout << personhistory_[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "[ofApp] image id history: ";
	for (int i = 0; i < historysize_; ++i)
	{
		std::cout << queryhistory_[i] << " ";
	}
	std::cout << std::endl;
#endif

	if (historysize_ > 1)
	{
		canBack_ = true;
		nowhistory_ = historysize_ - 1;
	}

	writelog();
}

//--------------------------------------------------------------
void ofApp::writelog(int init)
{
#ifndef EXPERIMENT
	std::ofstream log1(person_logfile_, ios::app);
#endif

	std::ofstream log2(candidate_logfile_, ios::app);

	if (init == 0)
	{
#ifndef EXPERIMENT
		log1 << person_ids_[clickNo_] << std::endl;
#endif

#ifndef EXPERIMENT
		for (int i = 0; i < picnum_; ++i)
		{
			if (i < picnum_ - 1)
				log2 << number_[i + 1] << " ";
			else
				log2 << number_[i + 1] << std::endl;
		}
#else
		for (int i = 0; i < picnum_; ++i)
		{
			if (i < picnum_ - 1)
				log2 << number_[i] << " ";
			else
				log2 << number_[i] << std::endl;
		}
#endif
	}
	else if (init == 1)
	{
#ifndef EXPERIMENT
		log1 << -1 << std::endl;
#endif

		for (int i = 0; i < picnum_; ++i)
		{
			if (i < picnum_ - 1)
				log2 << input_->number_[i] << " ";
			else
				log2 << input_->number_[i] << std::endl;
		}
	}
}

//--------------------------------------------------------------
void ofApp::backhistory()
{
	if (nowhistory_ < 2)
	{
		canEnter_ = true;
		canBack_ = false;
		nowhistory_ = 0;
	}
	else
	{
		canEnter_ = true;
		canBack_ = true;
		nowhistory_ += -1;
	}
}

//--------------------------------------------------------------
void ofApp::enterhistory()
{
	if (nowhistory_ > historysize_ - 3)
	{
		canEnter_ = false;
		canBack_ = true;
		nowhistory_ = historysize_ - 1;
	}
	else
	{
		canEnter_ = true;
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
	d_size_ = (windowWidth_ - leftsize_) / colShow_;

	if (d_size_ < 1)
	{
		d_size_ = 1;
		colShow_ = windowWidth_ - leftsize_;
	}

	imagerow_ = (showList_.size() + colShow_ - 1) / colShow_;
	if (imagerow_ < 1)
		imagerow_ = 1;

	bottom_ = -topsize_ - (d_size_ * imagerow_) + windowHeight_;

	if (d_size_ * imagerow_ < windowHeight_)
		rowshort_ = true;
	else
		rowshort_ = false;

	dragw_ = 0;
}
