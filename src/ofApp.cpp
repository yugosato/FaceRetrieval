#include "ofApp.h"

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
	previousmouseover_ = -1;

	//-----------------------------------------
	// クエリ関連
	clickNo_ = -1;
	clickflag_ = false;

	//-----------------------------------------
	// その他/ウィンドウ設定
	windowWidth_ = initWidth_;
	windowHeight_ = initHeight_;

	// その他/フォント
	ttf_ = "fonts/RictyDiminished-Bold.ttf";

	// その他/データベース情報
	datasetdir_ = "/home/yugo/Desktop/dataset/";
	dataset_ = "lfw-cropped"; // [lfw-cropped, fuji-B, vgg-face-cropped]
	nameFile_ = datasetdir_ + dataset_ + "/images.txt";
	matrixfile_ = "bin/data/lfw/lfw-vgg.tsv";
	indexFile_ = "bin/data/lfw/index";

	onoff_ = false;
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

	// データベース情報取得
	input_ = new DataBase();
	input_->setup(nameFile_);
	row_ = input_->getRow();
	input_->getName(&name_);

	guiSetup();

	// 別スレッドで画像読み込み
	loader_ = new ImageLoader();
	loader_->setRow(row_);
	loader_->setName(name_);

	calculate();
	onPaint();

	// NGTセットアップ
	ngt_ = new Search();
	ngt_->setup(indexFile_);

	// 別スレッドで特徴量読み込み
	loading_ = new NowLoading();
	loading_->setMatFile(matrixfile_);
	loading_->setRow(row_);
	loading_->startThread();
	std::cout << "[NowLoading] start loading vgg-face features." << std::endl;
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
		std::cout << "[NowLoading] finished loading vgg-face features." << std::endl;
	}

	if (click_)
		presstime_++;
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofSetColor(255);

	if (!isLoaded_)
	{
		std::string nowload = "Now Loading...\n";
		std::string process = ofToString(loading_->count_ * 100 / loading_->row_) + "%";
		font_.drawString(nowload + process, inputImgposx_, inputImgposy_ + 15);
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
		else
			img.draw(drawx, drawy, d_size_, d_size_);

		if (i == mouseover_)
		{
			ofSetColor(ofColor(255.0f, 255.0f, 255.0f, 127.0f));
			ofDrawRectangle(leftsize_ + d_size_ * j, topsize_ + d_size_ * k + dragh_, d_size_, d_size_);
			ofSetColor(255);
		}
	}

	if (clickflag_)
	{
		ofSetHexColor(0xCCCCCC);
		picture_.draw(inputImgposx_, inputImgposy_, inputImgsize_, inputImgsize_);
		ofDrawRectangle(5, inputImgposy_ - 5, 5, inputImgsize_ + 10);
		ofDrawRectangle(5, inputImgposy_ - 5, inputImgsize_ + 10, 5);
		ofDrawRectangle(5, inputImgposy_ + inputImgsize_, inputImgsize_ + 10, 5);
		ofDrawRectangle(inputImgposx_ + inputImgsize_, inputImgposy_ - 5, 5, inputImgsize_ + 10);

		// 戻るボタン
		if (!canBack_)
			backbutton0_.draw(68, 240, 50, 50);
		else
			backbutton1_.draw(68, 240, 50, 50);

		// 進むボタン
		if (!canEnter_)
			enterbutton0_.draw(122, 240, 50, 50);
		else
			enterbutton1_.draw(122, 240, 50, 50);
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
				std::cout << "[ofApp] can't back." << std::endl;
			break;
		}
		case 13: // Enter
		{
			if (canEnter_)
				enter();
			else
				std::cout << "[ofApp] can't enter." << std::endl;
			break;
		}
		case 102: // Ctrl+F
		{
			if (onoff_)
				ofToggleFullscreen();
			else
				ofToggleFullscreen();
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
	initRange(2, 26);
	calculate();
	onPaint();
}

//--------------------------------------------------------------
void ofApp::enter()
{
	ishistory_ = true;
	enterhistory();
	input_->setNumber(numberhistory_[nowhistory_]);
	initRange(2, 26);
	calculate();
	onPaint();
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

	if (y_dash >= 0 && y > topsize_ && x_dash >= 0 && x > leftsize_)
	{
		previousmouseover_ = mouseover_;
		mouseover_ = (x_dash / d_size_) + (y_dash / d_size_) * colShow_; //クリックした場所
	}
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
				const int No = showList_[clickpos];
				if (button == 0 && isLoaded_)
				{
					clickNo_ = No;
					inputQuery();
					inputHistory();
				}
			}
		}

		if (canBack_ && pressbutton(68, 240, 50, 50))
			back();

		if (canEnter_ && pressbutton(122, 240, 50, 50))
			enter();
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
		if (clickx_ <= x + h && clicky_ <= y + h)
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

	if (clickflag_)
	{
		if (!ishistory_)
			picture_.load(name_[clickNo_]);
		else
			picture_.load(name_[queryhistory_[nowhistory_]]);
	}
	ishistory_ = false;
}

//--------------------------------------------------------------
void ofApp::inputQuery()
{
	clickflag_ = true;
	goback_ = true;

	ngt_->setInput(clickNo_);
	ngt_->search();
	ngt_->getNumber(&number_);
	input_->setNumber(number_);
	initRange(2, 26);

	calculate();
	onPaint();
	queryinfo();
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
	std::cout << "[ofApp] you clicked --> " << queryname_ << std::endl;
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
			queryhistory_.pop_back();
			numberhistory_.pop_back();
		}
	}

	queryhistory_.push_back(clickNo_);
	numberhistory_.push_back(number_);
	historysize_ = numberhistory_.size();

	std::cout << "click history: ";
	for (int i = 0; i < historysize_; ++i)
	{
		std::cout << queryhistory_[i] << " ";
	}
	std::cout << std::endl;
	std::cout << std::endl;

	if (historysize_ > 1)
	{
		canBack_ = true;
		nowhistory_ = historysize_ - 1;
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
