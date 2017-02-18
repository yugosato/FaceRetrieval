#ifndef SRC_LOADER_H_
#define SRC_LOADER_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <NGT/Index.h>
#include "ofMain.h"

class NowLoading: public ofThread
{
public:
	std::string matFile_;

	std::vector<std::vector<double>> mat_;
	int row_;
	int col_;
	int count_;
	bool done_;

	NowLoading()
	{
		row_ = 0;
		col_ = 0;
		count_ = 0;
		done_ = false;
	}

	void threadedFunction()
	{
		loadMatrix();
		done_ = true;
	}

	void setMatFile(const std::string& matFile)
	{
		matFile_ = matFile;
	}

	void setRow(const int row)
	{
		row_ = row;
	}

	const int getCol()
	{
		return col_;
	}

	// 特徴量の読み込み
	void loadMatrix()
	{
		std::ifstream ifs(matFile_);
		if (!ifs)
		{
			std::cerr << "[NowLoading] Cannot open the specified file. " << matFile_ << std::endl;
		}
		else
		{
			std::string line;
			while (getline(ifs, line))
			{
				std::vector<std::string> tokens;
				NGT::Common::tokenize(line, tokens, "\t");
				std::vector<double> obj;
				for (std::vector<std::string>::iterator ti = tokens.begin(); ti != tokens.end(); ++ti)
				{
					obj.push_back(NGT::Common::strtod(*ti));
				}
				mat_.push_back(obj);
				count_++;
			}
			col_ = mat_[0].size();
		}
	}
};


class ImageLoader: public ofThread
{
public:
	std::vector<ofImage> picture_;
	std::vector<std::string> name_;
	std::vector<int> showList_;
	int row_;

public:
	void threadedFunction()
	{
		lock();
		for (int i = 0; i < row_; i++)
		{
			picture_[i].load(name_[showList_[i]]);
		}
		this->stopThread();
		unlock();
	}

	void setName(const std::vector<std::string>& name)
	{
		name_ = name;
	}

	void setShowList(const std::vector<int>& showList)
	{
		showList_.clear();
		showList_ = showList;
		row_ = showList_.size();
		picture_.clear();
		picture_.resize(row_);
	}

	void setRow(const int row)
	{
		picture_.reserve(row);
	}
};

#endif /* SRC_LOADER_H_ */
