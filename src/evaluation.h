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
	float distance_origin_;
	float distance_main_;
	int count_;
	std::string filename_;
	std::vector<std::vector<double>> features_;
	std::vector<double> inputpoint_origin_;
	std::vector<double> inputpoint_main_;
	std::vector<int> number_origin_;
	std::vector<int> number_main_;
	bool target_isInside_origin_;
	bool target_isInside_main_;


public:
	SingleSearchEvaluater()
	{
		distance_origin_ = 9999.9f;
		distance_main_ = 9999.9f;
		searchTarget_ = -1;
		count_ = 0;
		target_isInside_origin_ = false;
		target_isInside_main_ = false;
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

	void set_inputpoint(const std::vector<double>& inputpoint_origin, const std::vector<double>& inputpoint_main)
	{
		inputpoint_origin_ = inputpoint_origin;
		inputpoint_main_ = inputpoint_main;
	}

	void set_results(const std::vector<int>& number_origin, const std::vector<int>& number_main)
	{
		number_origin_ = number_origin;
		number_main_ = number_main;
	}

	void run()
	{
		calculate_distance();
		find_searchTarget();
		write();
	}

	// Calculate distance between the target and input point.
	void calculate_distance()
	{
		distance_origin_ = cosine(features_[searchTarget_], inputpoint_origin_, "distance");
		distance_main_ = cosine(features_[searchTarget_], inputpoint_main_, "distance");
	}

	void find_searchTarget()
	{
		if (vector_finder(number_origin_, searchTarget_))
			target_isInside_origin_ = true;

		if (vector_finder(number_main_, searchTarget_))
			target_isInside_main_ = true;
	}

	void write()
	{
		std::ofstream writer(filename_, std::ios::app);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << filename_ << std::endl;

		if (count_ == 0)
		{
			writer << "Target: " << searchTarget_ << std::endl;
			writer << "original" << "," << "main" << std::endl;
		}

		writer << distance_origin_ << "," << distance_main_;

		if (target_isInside_origin_)
			writer << "," << "target is in origin";

		if (target_isInside_main_)
			writer << "," << "target is in main";

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
