#ifndef SRC_SELECTION_H_
#define SRC_SELECTION_H_

#define BOOST_PYTHON_STATIC_LIB

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include "util.h"


class Selection
{
public:
	std::string uncertain_indexfile_;
	std::string cueflik_indexfile_;
	std::string random_indexfile_;
	std::vector<int> selection_;
	std::vector<int> full_selection_;
	std::vector<int> result_origin_;
	std::vector<int> result_rerank_;
	std::string method_;
	int searchTarget_;
	int active_size_;
	int row_;
	bool mix_;


public:
	void setup(	const std::string& uncertain_indexfile, const std::string& cueflik_indexfile,
			const std::string& random_indexfile)
	{
		uncertain_indexfile_ = uncertain_indexfile;
		cueflik_indexfile_ = cueflik_indexfile;
		random_indexfile_ = random_indexfile;
	}

	void set_searchTarget(const int searchTarget)
	{
		searchTarget_ = searchTarget;
	}

	void set_active_size(int active_size)
	{
		active_size_ = active_size;
	}

	void set_row(int row)
	{
		row_ = row;
	}

	void set_method(const std::string method)
	{
		method_ = method;

		if(method_ == "random" || method_ == "traditional")
			mix_ = false;
		else if (method_ == "uncertain" || method_ == "cueflik")
			mix_ = true;
		else
		{
			std::cerr << "[Warning] Cannot select specified method: " << method_ << std::endl;
			std::abort();
		}
	}

	void set_result(const std::vector<int>& result_origin, const std::vector<int>& result_rerank)
	{
		result_origin_ = result_origin;
		result_rerank_ = result_rerank;
	}

	inline void load()
	{
		std::vector<int> index;
		if (method_ == "uncertain")
			read_index(uncertain_indexfile_);
		else if (method_ == "cueflik")
			read_index(cueflik_indexfile_);
		else if (method_ == "random")
			read_index(random_indexfile_);
		else if (method_ == "traditional")
			traditional();

		if (!mix_)
			default_selection();
		else
			mix_selection();
	}

	void getNumber(std::vector<int>* number) const
	{
		number->clear();
		int size = (int) selection_.size();
		number->resize(size);
		for (int i = 0; i < size; ++i)
			(*number)[i] = selection_[i];
	}


private:
	void default_selection()
	{
		selection_.resize(active_size_);
		for (int i = 0; i < active_size_; i++)
			selection_[i] = full_selection_[i];
	}

	// Mix active selection & reranked results.
	void mix_selection()
	{
		selection_.resize(active_size_, -1);

		int active_num = active_size_ * 0.4;
		int result_num = active_size_ - active_num;

		// Insert top rank result.
		int loc = 0;
		while (loc < result_num)
		{
			selection_[loc] = result_rerank_[loc];
			loc++;
		}

		// Insert active selection result.
		int j = 0;
		while (loc < active_size_ && j < (int) full_selection_.size())
		{
			int num = full_selection_[j];
			if (!vector_finder(selection_, num))
			{
				selection_[loc] = num;
				loc++;
			}
			j++;
		}

		// Insert if empty.
		while (loc < active_size_)
		{
			int num = rand() % row_;
			if (!vector_finder(selection_, num))
			{
				selection_[loc] = num;
				loc++;
			}
		}

		random();
	}

	void traditional()
	{
		full_selection_.clear();

		int size = result_origin_.size();
		int i = 0;
		while (i < size)
		{
			int num = result_origin_[i];

			full_selection_.push_back(num);
			i++;
		}
	}


	void random()
	{
		for (int m = 0; m < active_size_; ++m)
		{
			const int n = rand() % active_size_;
			const int tempNo = selection_[m];
			selection_[m] = selection_[n];
			selection_[n] = tempNo;
		}
	}

	void read_index(const std::string fname)
	{
		std::ifstream ifs(fname);
		if (!ifs)
			std::cerr << "[Warning] Cannot open the specified file. " << fname << std::endl;

		full_selection_.clear();
		std::string buf;

		while (ifs && std::getline(ifs, buf))
		{
			int num = std::atoi(buf.c_str());
			full_selection_.push_back(num);
		}
	}

};


#endif /* SRC_SELECTION_H_ */
