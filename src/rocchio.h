/*
 * rocchio.h
 *
 *  Created on: May 10, 2017
 *      Author: yugo
 */

#ifndef SRC_ROCCHIO_H_
#define SRC_ROCCHIO_H_

#undef Success

#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include "Eigen/Core"
#include "Eigen/Geometry"


class Rocchio
{
public:
	std::vector<std::vector<double>> relVec_;
	std::vector<std::vector<double>> inrelVec_;
	Eigen::VectorXd initquery_;
	Eigen::VectorXd newquery_;
	Eigen::VectorXd relVec_ave_;
	Eigen::VectorXd inrelVec_ave_;
	int dim_;
	int relnum_;
	int inrelnum_;


public:
	void setRelevance(const std::vector<std::vector<double>>& relVec)
	{
		relVec_ = relVec;
		relnum_ = relVec_.size();
		dim_ = relVec_[0].size();
	}

	void setInRelevance(const std::vector<std::vector<double>>& inrelVec)
	{
		inrelVec_ = inrelVec;
		inrelnum_ = inrelVec_.size();
	}

	inline void setInitquery(const std::vector<double> initquery, const int phase)
	{
		initquery_ = Eigen::VectorXd::Zero(dim_);
		if (phase > 0)
			for (int i = 0; i < dim_; ++i)
				initquery_[i] = initquery[i];
	}

	inline void calcAverage()
	{
		if (relnum_ > 0)
			average(relVec_, relVec_ave_);
		else
			relVec_ave_ = Eigen::VectorXd::Zero(dim_);

		if (inrelnum_ > 0)
			average(inrelVec_, inrelVec_ave_);
		else
			inrelVec_ave_ = Eigen::VectorXd::Zero(dim_);
	}

	inline void average(const std::vector<std::vector<double>>& srcVec, Eigen::VectorXd& dstVec)
	{
		const int size = srcVec.size();
		Eigen::VectorXd average = Eigen::VectorXd::Zero(dim_);

		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < dim_; ++j)
				average[j] += srcVec[i][j];
		}

		average = average.array() / size;
		dstVec = average;
	}

	inline void calculate(const float alpha, const float beta, const float gamma)
	{
		std::cout << "[Rocchio] Alpha: " << alpha << ", Beta: " << beta << ", Gamma: " << gamma << std::endl;
		calcAverage();
		newquery_ = Eigen::VectorXd::Zero(dim_);
		newquery_ = (alpha * initquery_.array()) + (beta * relVec_ave_.array())
				- (gamma * inrelVec_ave_.array());
	}

	inline void getquery(std::vector<double>* query) const
	{
		query->resize(dim_);
		for (int i = 0; i < dim_; ++i)
			(*query)[i] = newquery_[i];
	}
};

#endif /* SRC_ROCCHIO_H_ */
