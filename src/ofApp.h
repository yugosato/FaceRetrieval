#pragma once

#include <iostream>
#include <vector>
#include <NGT/Index.h>
#include "ofMain.h"
#include "ofxUI.h"
#include "database.h"
#include "loader.h"
#include "search.h"

enum Mode
{
	query, bin, multi, rvector, cluster, vector5, mapping
};

const int initWidth_ = 960;				// 初期ウィンドウサイズ(幅)
const int initHeight_ = 720;			// 初期ウィンドウサイズ(高)

class ofApp: public ofBaseApp
{
public:
	//-----------------------------------------
	Mode mode_;							// モード

	//-----------------------------------------
	// 履歴
	int historysize_;					// 履歴サイズ
	std::vector<int> queryhistory_;		// クリックした画像番号履歴
	int backcount_;						// backした回数
	int entercount_;
	std::vector<std::vector<int>> numberhistory_;
	bool ishistory_;
	bool canBack_;
	bool canEnter_;
	int nowhistory_;

	//-----------------------------------------
	// 表示枚数(表示範囲[picA - picB])
	int picA_;							// スタート番号
	int picB_;							// エンド番号
	int picnum_;						// 総数

	//-----------------------------------------
	// GUI設定
	const int leftsize_ = 240;			// 左に確保する領域
	const int topsize_ = 0;				// 上に確保する領域

	const int fontsize_ = 18;			// フォントサイズ

//	const int inputTextposx_ = 84;		// "INPUT"の表示x座標
//	const int inputTextposy_ = 260;		// "INPUT"の表示y座標
	const int inputImgposx_ = 10;		// クエリ画像表示x座標
	const int inputImgposy_ = 10;		// クエリ画像表示y座標
	const int inputImgsize_ = 220;		// クエリ画像サイズ

	const int inputInfoposx_ = 14;		// クエリ画像情報x座標(基準)
	const int inputInfoposy_ = 310;		// クエリ画像情報y座標(基準)

	const int guiWidth_ = 240;			// GUIの幅
	int guiHeight_;						// GUIの高さ
	int guiScrollarea_;					// ?

	//-----------------------------------------
	// データベースからの返り値
	int row_;							// 画像枚数
	int col_;							// 特徴量次元数
	std::vector<std::string> name_; 	// 画像パスリストデータ
	std::vector<int> showList_;			// 表示リスト
	std::vector<int> number_;			// 表示順
	bool isLoaded_;						// 特徴量読み込み完了フラグ

	//-----------------------------------------
	// マウス＆キーボード
	int clickx_;						// クリック時のx座標
	int clicky_;						// クリック時のy座標
	int dragx_;							// ドラッグ後のx座標
	int dragy_;							// ドラッグ後のy座標
	int dragh_;							// ドラッグの縦の移動量
	int dragw_;							// ドラッグの横の移動量

	bool click_;						// クリックフラグ
	bool dontmove_;						// 動かさない時
	bool goback_;						// 一番上に戻るとき
	bool leftside_;						// 左サイドをクリックした時
	float velocity_;					// フリック速度

	int presstime_;						// プレス時間
	const int pressthreshold_ = 20;		// プレス時間制限

	//-----------------------------------------
	// 画像表示関連
	int colShow_;						// 表示する列数
	int d_size_;						// 表示する画像サイズ
	int imagerow_;						// 表示列数
	int bottom_;						// 現在表示されている底辺座標
	bool rowshort_;						// 表示する行数のほうが小さい時
	int mouseover_;						// 現在のカーソルが指している画像番号
	int previousmouseover_;				// 一つ前にカーソルが指していた画像番号

	//-----------------------------------------
	// クエリ関連
	int clickNo_;						// クリック番号
	bool clickflag_;					// クエリクリック

	//-----------------------------------------
	// その他/ウィンドウ設定
	int windowWidth_;					// ウィンドウの幅
	int windowHeight_;					// ウィンドウの高さ
	bool onoff_;						// フルスクリーンフラグ

	// その他/フォント
	std::string ttf_;					// フォントデータ

	// その他/パス
	std::string binData_;				// データファイルディレクトリ
	std::string matrixfile_;			// 特徴量ファイル
	std::string nameFile_;				// 画像パスリストファイル
	std::string indexFile_;				// NGTインデックス

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
	inline void onPaint();
	inline void inputQuery();
	inline void inputHistory();
	inline void initRange(const int& begin_, const int& end_);
	inline bool isReady();
	inline void sizeChanged();
	inline void backhistory();
	inline void enterhistory();
	inline void back();
	inline void enter();
	inline bool pressbutton(float x, float y, float w, float h);

public:
	ofTrueTypeFont font_;				// フォントデータ
	ofImage picture_;					// クエリ画像
	ofImage backbutton0_;
	ofImage backbutton1_;
	ofImage backbutton2_;
	ofImage enterbutton0_;
	ofImage enterbutton1_;
	ofImage enterbutton2_;

	// ofxUI
	ofxUIScrollableCanvas* gui_;
	void guiSetup();
	void guiEvent(ofxUIEventArgs& e);
	void exit();

	DataBase* input_;					// データベース情報
	NowLoading* loading_;				// 特徴量読み込み
	ImageLoader* loader_; 				// 画像読み込み
	Search* ngt_;						// NGT
};
