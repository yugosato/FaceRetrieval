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
	bool target_isInside_;

public:
	SingleSearchEvaluater()
	{
		distance_ = 9999.9f;
		searchTarget_ = -1;
		count_ = 0;
		target_isInside_ = false;
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
		if (vector_finder(results_, searchTarget_))
			target_isInside_ = true;
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
		if (target_isInside_)
			writer << "," << "Target";
		writer << std::endl;

		writer.close();
		count_++;
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


#endif /* SRC_EVALUATION_H_ */
