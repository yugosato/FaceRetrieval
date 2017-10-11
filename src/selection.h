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
	std::vector<int> number_selection_;
	std::vector<int> full_number_selection_;
	bool isLoaded_;
	int size_;


public:
	void setup(	const std::string& uncertain_indexfile, const std::string& cueflik_indexfile,
			const std::string& random_indexfile)
	{
		uncertain_indexfile_ = uncertain_indexfile;
		cueflik_indexfile_ = cueflik_indexfile;
		random_indexfile_ = random_indexfile;
		isLoaded_ = false;
	}

	void set_searchTarget(const int searchTarget)
	{
		searchTarget_ = searchTarget;
	}

	void set_size(const int size)
	{
		size_ = size;
		number_selection_.resize(size_);
	}

	void load(const std::string method)
	{
		isLoaded_ = false;
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

		for (int i = 0; i < size_; i++)
			number_selection_[i] = full_number_selection_[i];

		isLoaded_ = true;
	}

	// Mix active selection & reranked results.
	void mix_selection(const std::vector<int>& result)
	{
		// Store each value.
		int active_num = 15;
		int top_result_num = 5;
		//int low_result_num = 5;

		int i = 0, j = 0, k = result.size() - 1;
		int loc = 0;
		std::vector<int> mixed_selection(size_);

		while (loc < size_)
		{
			if (loc < active_num)// loc:0-14
			{
				if (!vector_finder(result, full_number_selection_[i]))
				{
					mixed_selection[loc] = full_number_selection_[i];
					loc++;
				}
				i++;
			}
			else if (active_num <= loc && loc < active_num + top_result_num)// loc:15-19
			{
				mixed_selection[loc] = result[j];
				loc++;
				j++;
			}
			else if (active_num + top_result_num <= loc)// loc:20-24
			{
				mixed_selection[loc] = result[k];
				loc++;
				k--;
			}
		}

		// Randomize
		for (int m = 0; m < size_; ++m)
		{
			const int n = rand() % size_;
			const int tempNo = mixed_selection[m];
			mixed_selection[m] = mixed_selection[n];
			mixed_selection[n] = tempNo;
		}

		number_selection_.clear();
		number_selection_ = mixed_selection;
	}

	inline void getNumber(std::vector<int>* number) const
	{
		number->clear();
		int size = (int) number_selection_.size();
		number->resize(size);
		for (int i = 0; i < size; ++i)
			(*number)[i] = number_selection_[i];
	}


private:
	void read_index(const std::string fname)
	{
		std::ifstream ifs(fname);
		if (!ifs)
			std::cerr << "[Warning] Cannot open the specified file. " << fname << std::endl;

		full_number_selection_.clear();
		std::string buf;

		while (ifs && std::getline(ifs, buf))
		{
			int num = std::atoi(buf.c_str());
			if (num == searchTarget_)
				continue;
			full_number_selection_.push_back(num);
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
