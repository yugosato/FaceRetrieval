#ifndef SRC_SELECTION_H_
#define SRC_SELECTION_H_

#define BOOST_PYTHON_STATIC_LIB

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>


class Selection
{
public:
	int searchTarget_;

	std::string positive_indexfile_;
	std::string negative_indexfile_;

	std::string uncertain_indexfile_;
	std::string cueflik_indexfile_;
	std::string random_indexfile_;

	std::vector<int> number_estPositive_;
	std::vector<int> number_estNegative_;

	std::vector<int> number_uncertain_;
	std::vector<int> number_cueflik_;
	std::vector<int> number_random_;

	bool isLoaded_;
	int size_;


public:
	void setup(const std::string& positive_indexfile, const std::string& negative_indexfile,
			const std::string& uncertain_indexfile, const std::string& cueflik_indexfile,
			const std::string& random_indexfile)
	{
		positive_indexfile_ = positive_indexfile;
		negative_indexfile_ = negative_indexfile;
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
	}

	void load()
	{
		isLoaded_ = false;
		//read_index(positive_indexfile_, number_estPositive_);
		//read_index(negative_indexfile_, number_estNegative_);
		read_index(uncertain_indexfile_, number_uncertain_);
		//read_index(cueflik_indexfile_, number_cueflik_);
		//read_index(random_indexfile_, number_random_);
		isLoaded_ = true;
	}

	inline void read_index(const std::string fname, std::vector<int>& vec)
	{
		std::ifstream ifs(fname);
		if (!ifs)
			std::cerr << "[Warning] Cannot open the specified file. " << fname << std::endl;

		vec.clear();
		std::vector<int>().swap(vec);
		std::string buf;

		int i = 0;
		while (ifs && std::getline(ifs, buf) && i < size_)
		{
			int index = std::atoi(buf.c_str());
			if (index == searchTarget_)
				continue;
			vec.push_back(index);
			i++;
		}
	}

	inline void getNumber(std::vector<int>* number) const
	{
		number->clear();
		int size = (int) number_uncertain_.size();
		number->resize(size);
		for (int i = 0; i < size; ++i)
			(*number)[i] = number_uncertain_[i];
	}

};


#endif /* SRC_SELECTION_H_ */
