#ifndef SRC_DATABASE_H_
#define SRC_DATABASE_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <typeinfo>
#include "NGT/Index.h"


class DataBase
{
public:
	int searchTarget_;
	int init_size_;
	int row_;
	std::string nameFile_;
	std::string initFile_;
	std::vector<std::string> name_;
	std::vector<int> number_active_;
	std::vector<int> number_origin_;
	std::vector<int> number_rerank_;
	std::vector<int> number_visualrank_;
	std::vector<int> showList_;
	ofImage target_img_;
	ofImage toprank_img_origin_;
	ofImage toprank_img_rerank_;
	ofImage toprank_img_visualrank_;


public:
	void setup(const int searchTarget, const int init_size, const std::string nameFile, const std::string initFile)
	{
		searchTarget_ = searchTarget;
		init_size_ = init_size;
		nameFile_ = nameFile;
		initFile_ = initFile;
	}

	void initialize(std::string init_method)
	{
		loadFileName();

		if (init_method == "random")
			random();
		else if (init_method == "kmeans")
			init_clustering();
		else
			init();

		number_active_ = number_origin_;
		number_rerank_ = number_origin_;
		number_visualrank_ = number_origin_;

		target_img_.load(name_[searchTarget_]);
	}

	void makeShowList_active(const int size)
	{
		showList_.resize(size);
		for (int i = 0; i < size; ++i)
			showList_[i] = number_active_[i];
	}

	void makeShowList_origin(const int size)
	{
		showList_.resize(size);
		for (int i = 0; i < size; ++i)
			showList_[i] = number_origin_[i];
	}

	void makeShowList_rerank(const int size)
	{
		showList_.resize(size);
		for (int i = 0; i < size; ++i)
			showList_[i] = number_rerank_[i];
	}

	void makeShowList_visualrank(const int size)
	{
		showList_.resize(size);
		for (int i = 0; i < size; ++i)
			showList_[i] = number_visualrank_[i];
	}

	void setNumber_active(const std::vector<int>& number)
	{
		const int size = number.size();
		number_active_.resize(size);
		for (int i = 0; i < size; ++i)
			number_active_[i] = number[i];
	}

	void setNumber_origin(const std::vector<int>& number)
	{
		const int size = number.size();
		number_origin_.resize(size);
		for (int i = 0; i < size; ++i)
			number_origin_[i] = number[i];
		toprank_img_origin_.load(name_[number_origin_[0]]);
	}

	void setNumber_rerank(const std::vector<int>& number)
	{
		const int size = number.size();
		number_rerank_.resize(size);
		for (int i = 0; i < size; ++i)
			number_rerank_[i] = number[i];
		toprank_img_rerank_.load(name_[number_rerank_[0]]);
	}

	void setNumber_visualrank(const std::vector<int>& number)
	{
		const int size = number.size();
		number_visualrank_.resize(size);
		for (int i = 0; i < size; ++i)
			number_visualrank_[i] = number[i];
		toprank_img_visualrank_.load(name_[number_visualrank_[0]]);
	}

	const std::vector<int>& getShowList() const
	{
		return showList_;
	}


private:
	// Load image list.
	void loadFileName()
	{
		std::ifstream ifs(nameFile_);
		if (!ifs)
		{
			std::cerr << "[Warning] Cannot open the specified file. " << nameFile_ << std::endl;
		}
		else
		{
			std::string line;
			while (getline(ifs, line))
			{
				std::vector<std::string> tokens;
				NGT::Common::tokenize(line, tokens, " ");
				name_.push_back(tokens[0]);
			}
			row_ = name_.size();
		}
	}

	void init()
	{
		number_origin_.resize(init_size_);

		int loc = 0;
		int num = 0;
		while (loc < init_size_)
		{
			if (num != searchTarget_)
			{
				number_origin_[loc] = num;
				loc++;
			}
			num++;
		}
	}

	void random()
	{
		init();
		for (int i = 0; i < init_size_; ++i)
		{
			const int j = rand() % init_size_;
			const int tempNo = number_origin_[i];
			number_origin_[i] = number_origin_[j];
			number_origin_[j] = tempNo;
		}
	}

	// k-means clustering initialization
	void init_clustering()
	{
		std::ifstream ifs(initFile_, std::ios::in);
		if(!ifs)
		{
			std::cerr << "[Warning] Cannot open the specified file. " << initFile_ << std::endl;
		}
		else
		{
			std::string line;
			getline(ifs, line);
			std::vector<std::string> tokens;
			NGT::Common::tokenize(line, tokens, " ");

			number_origin_.resize(init_size_);
			for (int i = 0; i < init_size_; ++i)
			{
				number_origin_[i] = std::atoi(tokens[i].c_str());
			}
		}
	}
};

#endif /* SRC_DATABASE_H_ */
