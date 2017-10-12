#ifndef SRC_LOADER_H_
#define SRC_LOADER_H_

#include <iostream>
#include <vector>
#include <fstream>
#include "NGT/Index.h"
#include "ofMain.h"

class NowLoading: public ofThread
{
public:
	std::string featuresfile_;
	std::string new_featuresfile_;
	std::vector<std::vector<double>> features_;
	std::vector<std::vector<double>> new_features_;
	int row_;
	int col_;
	bool isLoaded_init_;
	bool isLoaded_new_;
	bool isLoadNew_;


public:
	NowLoading()
	{
		row_ = 0;
		col_ = 0;
		isLoaded_init_ = false;
		isLoaded_new_ = false;
		isLoadNew_ = false;
	}

	inline void threadedFunction()
	{
		lock();

		if (!isLoadNew_)
		{
			std::cout << "[NowLoading] Start loadinge features." << std::endl;
			isLoaded_init_ = false;
			load_features();
			isLoaded_init_ = true;
			std::cout << "[NowLoading] Finished loading features." << std::endl;
		}
		else
		{
			std::cout << "[NowLoading] Start loading new features." << std::endl;
			isLoaded_new_ = false;
			load_new_features();
			isLoaded_new_ = true;
			std::cout << "[NowLoading] Finished loading new features." << std::endl;
		}

		unlock();
	}

	void set_featuresfile(const std::string& featuresfile, const std::string& new_featuresfile)
	{
		featuresfile_ = featuresfile;
		new_featuresfile_ = new_featuresfile;
	}

	void setRow(const int row)
	{
		row_ = row;
	}

	const int getCol()
	{
		return col_;
	}


private:
	inline void load_features()
	{
		std::ifstream ifs(featuresfile_);
		if (!ifs)
			std::cerr << "[Warning] Cannot open the specified file. " << featuresfile_ << std::endl;
		else
		{
			std::string line;
			while (getline(ifs, line))
			{
				std::vector<std::string> tokens;
				NGT::Common::tokenize(line, tokens, "\t");
				std::vector<double> obj;
				for (std::vector<std::string>::iterator ti = tokens.begin(); ti != tokens.end(); ++ti)
					obj.push_back(NGT::Common::strtod(*ti));
				features_.push_back(obj);
			}
			col_ = features_[0].size();
		}
	}

	inline void load_new_features()
	{
		std::ifstream ifs(new_featuresfile_);
		if (!ifs)
			std::cerr << "[Warning] Cannot open the specified file. " << new_featuresfile_ << std::endl;
		else
		{
			new_features_.clear();
			std::string line;
			while (getline(ifs, line))
			{
				std::vector<std::string> tokens;
				NGT::Common::tokenize(line, tokens, "\t");
				std::vector<double> obj;
				for (std::vector<std::string>::iterator ti = tokens.begin(); ti != tokens.end(); ++ti)
					obj.push_back(NGT::Common::strtod(*ti));
				new_features_.push_back(obj);
			}
		}
	}
};


class ImageLoader
{
public:
	std::vector<ofImage> picture_;
	std::vector<std::string> name_;
	std::vector<int> showList_;
	int row_;


public:
	inline void load_images()
	{
		picture_.clear();
		picture_.resize(row_);
		for (int i = 0; i < row_; i++)
			picture_[i].load(name_[showList_[i]]);
	}

	void setName(const std::vector<std::string>& name)
	{
		name_.reserve(name.size());
		name_ = name;
	}

	inline void setShowList(const std::vector<int>& showList)
	{
		showList_.clear();
		std::vector<int>().swap(showList_);

		showList_ = showList;
		row_ = showList_.size();
	}

	void setRow(const int row)
	{
		picture_.reserve(row);
	}
};

#endif /* SRC_LOADER_H_ */
