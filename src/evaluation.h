#ifndef SRC_EVALUATION_H_
#define SRC_EVALUATION_H_

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <fstream>
#include "distance.h"
#include "util.h"


class SingleSearchEvaluater
{
public:
	int searchTarget_;
	float distance_;
	int count_;
	int target_rank_;
	std::string filename_;
	std::vector<std::vector<double>> features_;
	std::vector<double> inputpoint_;
	std::vector<int> results_;


public:
	void setup(const int searchTarget, const std::string filename)
	{
		searchTarget_ = searchTarget;
		filename_ = filename;
		distance_ = 0.0f;
		count_ = 0;
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
	void calculate_distance()
	{
		distance_ = mycosine(features_[searchTarget_], inputpoint_, "distance");
	}

	void find_searchTarget()
	{
		target_rank_ = vector_index_finder(results_, searchTarget_);
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

};


#endif /* SRC_EVALUATION_H_ */
