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
	int target_;
	float distance_origin_;
	float distance_main_;
	int count_;
	std::string filename_;
	std::vector<std::vector<double>> features_;
	std::vector<double> inputpoint_origin_;
	std::vector<double> inputpoint_main_;


public:
	SingleSearchEvaluater()
	{
		distance_origin_ = 9999.9f;
		distance_main_ = 9999.9f;
		target_ = -1;
		count_ = 0;
	}

	void setup(const int target, const std::string filename)
	{
		target_ = target;
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

	void run()
	{
		calculate_distance();
		write();
	}

	// Calculate distance between the target and input point.
	void calculate_distance()
	{
		distance_origin_ = cosine(features_[target_], inputpoint_origin_, "distance");
		distance_main_ = cosine(features_[target_], inputpoint_main_, "distance");
	}

	void write()
	{
		std::ofstream writer(filename_, std::ios::app);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << filename_ << std::endl;

		if (count_ == 0)
		{
			writer << "Target: " << target_ << std::endl;
			writer << "original" << "," << "main" << std::endl;
		}

		writer << distance_origin_ << "," << distance_main_ << std::endl;
		writer.close();
		count_++;
	}
};


#endif /* SRC_EVALUATION_H_ */
