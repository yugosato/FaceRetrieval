#ifndef SRC_DATABASE_H_
#define SRC_DATABASE_H_

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <typeinfo>
#include "NGT/Index.h"


class DataBase
{
public:
	std::string nameFile_; 						// 画像パスリストファイル

public:
	int row_;									// 枚数
	std::vector<std::string> name_;				// ファイル名リスト(サイズ：全画像数)
	std::vector<int> number_;					// 表示順(サイズ：全画像数)
	std::vector<int> showList_;					// 表示する画像のリスト
	std::vector<int> ids_;						// 各画像に対応する人物番号

public:
	void setup(std::string nameFile)
	{
		nameFile_ = nameFile;
		loadFileName();

		// メモリ確保＆初期化
		number_.resize(row_);
		init();
		random();
	}

	// 画像パスリスト読み込み
	void loadFileName()
	{
		std::ifstream ifs(nameFile_);
		if (!ifs)
		{
			std::cerr << "[DataBase] Cannot open the specified file. " << nameFile_ << std::endl;
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
		{
			number_[i] = i;
		}
	}

	// ランダム初期化
	void random()
	{
		for (int i = 0; i < row_; ++i)
		{
			const int j = rand() % row_;
			const int tempNo = number_[i];
			number_[i] = number_[j];
			number_[j] = tempNo;
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
		{
			showList_[i] = number_[begin + i - 1];
		}
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
