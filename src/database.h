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
	std::string nameFile_; 						// 画像パスリストファイル
	std::string initFile_;
	int row_;									// 枚数
	std::vector<std::string> name_;				// ファイル名リスト(サイズ：全画像数)
	std::vector<int> number_;					// 表示順(サイズ：全画像数)
	std::vector<int> number_removed_;
	std::vector<int> number_eval_;
	std::vector<int> showList_;					// 表示する画像のリスト
	std::vector<int> history_;					// 表示された候補履歴
	std::vector<int> ids_;						// 各画像に対応する人物番号


public:
	void setup(const std::string nameFile, const std::string initFile)
	{
		nameFile_ = nameFile;
		initFile_ = initFile;
		loadFileName();

		// メモリ確保＆初期化
		number_.resize(row_);
//		init();
//		random();
		init_clustering();
		number_eval_ = number_;
	}

	// 画像パスリスト読み込み
	void loadFileName()
	{
		std::ifstream ifs(nameFile_);
		if (!ifs)
		{
			std::cerr << "[warning] cannot open the specified file. " << nameFile_ << std::endl;
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

	// 初期化
	void init()
	{
		for (int i = 0; i < row_; ++i)
			number_[i] = i;
	}

	// ランダム初期化
	void random()
	{
		init();
		for (int i = 0; i < row_; ++i)
		{
			const int j = rand() % row_;
			const int tempNo = number_[i];
			number_[i] = number_[j];
			number_[j] = tempNo;
		}
	}

	// k-means clustering initialization
	void init_clustering()
	{
		std::ifstream ifs(initFile_, std::ios::in);
		if(!ifs)
		{
			std::cerr << "[warning] cannot open the specified file. " << initFile_ << std::endl;
		}
		else
		{
			std::string line;
			getline(ifs, line);
			std::vector<std::string> tokens;
			NGT::Common::tokenize(line, tokens, " ");

			std::vector<int> num;
			for (int i = 0; i < tokens.size(); ++i)
			{
				num.push_back(std::atoi(tokens[i].c_str()));
			}

			for (int i = 0; i < num.size(); ++i)
			{
				number_[i] = num[i];
			}
		}
	}

	// 表示リスト更新
	void makeShowList(const int begin, const int end)
	{
		const int size = end - begin + 1;

		showList_.clear();
		std::vector<int>().swap(showList_);
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_[begin + i - 1];
	}

	// 表示リスト更新 (重複除去)
	void makeShowList_removed(const int begin, const int end)
	{
		const int size = end - begin + 1;

		showList_.clear();
		std::vector<int>().swap(showList_);
		showList_.resize(size);

		number_removed_.clear();
		std::vector<int>().swap(number_removed_);
		number_removed_.resize(size);

		int count = 0;
		int iter = 0;
		while (count < size)
		{
			auto pos = std::find(history_.begin(), history_.end(), number_[iter]);
			if (pos != history_.end())
			{
				iter++;
				continue;
			}
			else
			{
				showList_[count] = number_[iter];
				number_removed_[count] = number_[iter];
				iter++;
				count++;
			}
		}
	}

	void makeShowList_eval(const int begin, const int end)
	{
		const int size = end - begin + 1;

		showList_.clear();
		std::vector<int>().swap(showList_);
		showList_.resize(size);

		for (int i = 0; i < size; ++i)
			showList_[i] = number_eval_[begin + i - 1];
	}

	// 表示された候補履歴
	void setHistory(const std::vector<int>& history)
	{
		history_ = history;
	}

	// 表示順の更新
	void setNumber(const std::vector<int>& number)
	{
		const int size = number.size();

		number_.clear();
		std::vector<int>().swap(number_);
		number_.resize(size);

		for (int i = 0; i < size; ++i)
			number_[i] = number[i];
	}

	void setNumber_eval(const std::vector<int>& number)
	{
		const int size = number.size();

		number_eval_.clear();
		std::vector<int>().swap(number_eval_);
		number_eval_.resize(size);

		for (int i = 0; i < size; ++i)
			number_eval_[i] = number[i];
	}

	// 画像名を返す
	void getName(std::vector<std::string>* nameList) const
	{
		nameList->resize(row_);
		for (int i = 0; i < row_; ++i)
			(*nameList)[i] = name_[i];
	}

	// 画像に対応する人物IDを返す
	void getPersonID(std::vector<int>* person_ids) const
	{
		person_ids->resize(row_);
		for (int i = 0; i < row_; ++i)
			(*person_ids)[i] = ids_[i];
	}

	// 画像枚数を返す
	const int& getRow() const
	{
		return row_;
	}

	// 表示リストを返す
	const std::vector<int>& getShowList() const
	{
		return showList_;
	}
};

#endif /* SRC_DATABASE_H_ */
