#ifndef SRC_EVALUATION_H_
#define SRC_EVALUATION_H_

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <fstream>
#include "distance.h"


class SingleSearchEvaluater
{
public:
	int searchTarget_;
	float distance_;
	int count_;
	std::string filename_;
	std::vector<std::vector<double>> features_;
	std::vector<double> inputpoint_;
	std::vector<int> results_;
	int target_rank_;

public:
	SingleSearchEvaluater()
	{
		distance_ = 9999.9f;
		searchTarget_ = -1;
		count_ = 0;
		target_rank_ = -1;
	}

	void setup(const int searchTarget, const std::string filename)
	{
		searchTarget_ = searchTarget;
		filename_ = filename;
	}

	void set_features(const std::vector<std::vector<double>>& features)
	{
		features_ = features;
	}

	void set_inputpoint(const std::vector<double>& inputpoint)
	{
		inputpoint_ = inputpoint;
	}

	void set_results(const std::vector<int>& results)
	{
		results_ = results;
	}

	void run()
	{
		calculate_distance();
		find_searchTarget();
		write();
	}


private:
	// Calculate distance between the target and input point.
	void calculate_distance()
	{
		distance_ = cosine(features_[searchTarget_], inputpoint_, "distance");
	}

	void find_searchTarget()
	{
		target_rank_ = vector_finder(results_, searchTarget_);
	}

	void write()
	{
		std::ofstream writer(filename_, std::ios::app);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << filename_ << std::endl;

		if (count_ == 0)
		{
			writer << "Target: " << searchTarget_ << std::endl;
		}

		writer << distance_;
		if (target_rank_ >= 0)
			writer << "," << "Target: " << target_rank_;
		writer << std::endl;

		writer.close();
		count_++;
	}

	int vector_finder(const std::vector<int>& vector, const int number)
	{
		if (vector.size() > 0)
		{
			auto itr = std::find(vector.begin(), vector.end(), number);
			size_t index = std::distance(vector.begin(), itr);
			if (index != vector.size())
				return index;	// If the number exists in the vector.
			else
				return -1;	// If the number does not exist in the vector.
		}
		else
			return -1;
	}
};


#endif /* SRC_EVALUATION_H_ */
