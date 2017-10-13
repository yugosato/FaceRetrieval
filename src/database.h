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

	std::string nameFile_;
	std::string initFile_;
	int row_;
	std::vector<std::string> name_;
	std::vector<int> number_active_;
	std::vector<int> number_origin_;
	std::vector<int> number_rerank_;
	std::vector<int> number_visualrank_;
	std::vector<int> showList_;
	std::vector<int> history_;
	std::vector<int> ids_;
	int init_size_;


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

		if (init_method == "raadom")
			random();
		else if (init_method == "kmeans")
			init_clustering();
		else
			init();

		number_active_ = number_origin_;
		number_rerank_ = number_origin_;
		number_visualrank_ = number_origin_;
	}

	// Load image list.
	inline void loadFileName()
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
				ids_.push_back(std::atoi(tokens[1].c_str()));
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

			removeTarget();
		}
	}

	void removeTarget()
	{
		int size = number_origin_.size();
		for (int i = 0; i < size; ++i)
		{
			int num = number_origin_[i];
			if (num == searchTarget_)
			{
				number_origin_.erase(number_origin_.begin() + i);
				break;
			}
		}
	}


	inline void makeShowList_active(const int size)
	{
		showList_.clear();
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_active_[i];
	}

	inline void makeShowList_origin(const int size)
	{
		showList_.clear();
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_origin_[i];
	}

	inline void makeShowList_rerank(const int size)
	{
		showList_.clear();
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_rerank_[i];
	}

	inline void makeShowList_visualrank(const int size)
	{
		showList_.clear();
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_visualrank_[i];
	}

	void setHistory(const std::vector<int>& history)
	{
		history_ = history;
	}

	inline void setNumber_active(const std::vector<int>& number)
	{
		const int size = number.size();

		number_active_.clear();
		number_active_.resize(size);

		for (int i = 0; i < size; ++i)
			number_active_[i] = number[i];
	}

	inline void setNumber_origin(const std::vector<int>& number)
	{
		const int size = number.size();

		number_origin_.clear();
		number_origin_.resize(size);

		for (int i = 0; i < size; ++i)
			number_origin_[i] = number[i];
	}

	inline void setNumber_rerank(const std::vector<int>& number)
	{
		const int size = number.size();

		number_rerank_.clear();
		number_rerank_.resize(size);

		for (int i = 0; i < size; ++i)
			number_rerank_[i] = number[i];
	}

	inline void setNumber_visualrank(const std::vector<int>& number)
	{
		const int size = number.size();

		number_visualrank_.clear();
		number_visualrank_.resize(size);

		for (int i = 0; i < size; ++i)
			number_visualrank_[i] = number[i];
	}

	inline void getName(std::vector<std::string>* nameList) const
	{
		nameList->resize(row_);
		for (int i = 0; i < row_; ++i)
			(*nameList)[i] = name_[i];
	}

	inline void getPersonID(std::vector<int>* person_ids) const
	{
		person_ids->resize(row_);
		for (int i = 0; i < row_; ++i)
			(*person_ids)[i] = ids_[i];
	}

	const int& getRow() const
	{
		return row_;
	}

	const std::vector<int>& getShowList() const
	{
		return showList_;
	}
};

#endif /* SRC_DATABASE_H_ */
