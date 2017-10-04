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
	std::string nameFile_;
	std::string initFile_;
	int row_;
	std::vector<std::string> name_;
	std::vector<int> number_active_;
	std::vector<int> number_origin_;
	std::vector<int> number_main_;
	std::vector<int> number_visualrank_;
	std::vector<int> showList_;
	std::vector<int> history_;
	std::vector<int> ids_;


public:
	inline void setup(const std::string nameFile, const std::string initFile)
	{
		nameFile_ = nameFile;
		initFile_ = initFile;
		loadFileName();

		// Allocate memory & initialize.
		number_origin_.resize(row_);
		init();
		//random();
		//init_clustering();
		number_active_ = number_origin_;
		number_main_ = number_origin_;
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

	inline void init()
	{
		for (int i = 0; i < row_; ++i)
			number_origin_[i] = i;
	}

	inline void random()
	{
		init();
		for (int i = 0; i < row_; ++i)
		{
			const int j = rand() % row_;
			const int tempNo = number_origin_[i];
			number_origin_[i] = number_origin_[j];
			number_origin_[j] = tempNo;
		}
	}

	// k-means clustering initialization
	inline void init_clustering()
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

			std::vector<int> num;
			for (int i = 0; i < (int) tokens.size(); ++i)
			{
				num.push_back(std::atoi(tokens[i].c_str()));
			}

			for (int i = 0; i < (int) num.size(); ++i)
			{
				number_origin_[i] = num[i];
			}
		}
	}

	inline void makeShowList_active(const int begin, const int end)
	{
		const int size = end - begin + 1;

		showList_.clear();
		std::vector<int>().swap(showList_);
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_active_[begin + i - 1];
	}

	inline void makeShowList_origin(const int begin, const int end)
	{
		const int size = end - begin + 1;

		showList_.clear();
		std::vector<int>().swap(showList_);
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_origin_[begin + i - 1];
	}

	inline void makeShowList_main(const int begin, const int end)
	{
		const int size = end - begin + 1;

		showList_.clear();
		std::vector<int>().swap(showList_);
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_main_[begin + i - 1];
	}

	inline void makeShowList_visualrank(const int begin, const int end)
	{
		const int size = end - begin + 1;

		showList_.clear();
		std::vector<int>().swap(showList_);
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_visualrank_[begin + i - 1];
	}

	void setHistory(const std::vector<int>& history)
	{
		history_ = history;
	}

	inline void setNumber_active(const std::vector<int>& number)
	{
		const int size = number.size();

		number_active_.clear();
		std::vector<int>().swap(number_active_);
		number_active_.resize(size);

		for (int i = 0; i < size; ++i)
			number_active_[i] = number[i];
	}

	inline void setNumber_origin(const std::vector<int>& number)
	{
		const int size = number.size();

		number_origin_.clear();
		std::vector<int>().swap(number_origin_);
		number_origin_.resize(size);

		for (int i = 0; i < size; ++i)
			number_origin_[i] = number[i];
	}

	inline void setNumber_main(const std::vector<int>& number)
	{
		const int size = number.size();

		number_main_.clear();
		std::vector<int>().swap(number_main_);
		number_main_.resize(size);

		for (int i = 0; i < size; ++i)
			number_main_[i] = number[i];
	}

	inline void setNumber_visualrank(const std::vector<int>& number)
	{
		const int size = number.size();

		number_visualrank_.clear();
		std::vector<int>().swap(number_visualrank_);
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
