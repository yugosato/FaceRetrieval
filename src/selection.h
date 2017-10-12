#ifndef SRC_SELECTION_H_
#define SRC_SELECTION_H_

#define BOOST_PYTHON_STATIC_LIB

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>


class Selection
{
public:
	int searchTarget_;
	std::string uncertain_indexfile_;
	std::string cueflik_indexfile_;
	std::string random_indexfile_;
	std::vector<int> selection_;
	std::vector<int> full_selection_;
	std::vector<int> result_;
	int size_;


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

	void set_size(const int size)
	{
		size_ = size;
	}

	void set_result(std::vector<int>& result)
	{
		result_ = result;
	}

	void load(const std::string method, const bool mix = false)
	{
		std::vector<int> index;
		if (method == "uncertain")
			read_index(uncertain_indexfile_);
		else if (method == "cueflik")
			read_index(cueflik_indexfile_);
		else if (method == "random")
			read_index(random_indexfile_);
		else
		{
			std::cerr << "[Warning] Cannot open specified selection: " << method << std::endl;
			std::abort();
		}

		if (!mix)
			init_selection();
		else
			mix_selection();
	}

	inline void getNumber(std::vector<int>* number) const
	{
		number->clear();
		int size = (int) selection_.size();
		number->resize(size);
		for (int i = 0; i < size; ++i)
			(*number)[i] = selection_[i];
	}


private:
	void init_selection()
	{
		selection_.clear();
		selection_.resize(size_);
		for (int i = 0; i < size_; i++)
			selection_[i] = full_selection_[i];

		random();
	}

	// Mix active selection & reranked results.
	void mix_selection()
	{
		selection_.clear();
		selection_.resize(size_);

		int active_num = 15;
		int result_num = (size_ - active_num) / 2;

		int i = 0, j = 0, k = result_.size() - 1;
		int loc = 0;

		while (loc < active_num + result_num)
		{
			if (loc < active_num)
			{
				if (!vector_finder(result_, full_selection_[i]))
				{
					selection_[loc] = full_selection_[i];
					loc++;
				}
				i++;
			}
			else
			{
				selection_[loc] = result_[j];
				selection_[result_num + loc] = result_[k];
				j++;
				k--;
				loc++;
			}
		}

		random();
	}

	void random()
	{
		for (int m = 0; m < size_; ++m)
		{
			const int n = rand() % size_;
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
			if (num == searchTarget_)
				continue;
			full_selection_.push_back(num);
		}
	}

	bool vector_finder(const std::vector<int>& vector, const int number)
	{
		if (vector.size() > 0)
		{
			auto itr = std::find(vector.begin(), vector.end(), number);
			size_t index = std::distance(vector.begin(), itr);
			if (index != vector.size())
				return true;	// If the number exists in the vector.
			else
				return false;	// If the number does not exist in the vector.
		}
		else
			return false;
	}

};


#endif /* SRC_SELECTION_H_ */
