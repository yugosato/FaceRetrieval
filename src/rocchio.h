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
	std::vector<std::vector<double>> relevance_;
	std::vector<std::vector<double>> irrelevance_;
	Eigen::VectorXd initquery_;
	Eigen::VectorXd newquery_;
	Eigen::VectorXd relevance_ave_;
	Eigen::VectorXd irrelevance_ave_;
	int dim_;
	int relnum_;
	int irrelnum_;
	int count_;


public:
	Rocchio()
	{
		dim_ = 0;
		relnum_ = 0;
		irrelnum_ = 0;
		count_ = 0;
	}

	void set_vector(const std::vector<std::vector<double>>& relevance,
			const std::vector<std::vector<double>>& irrelevance)
	{
		relevance_ = relevance;
		irrelevance_ = irrelevance;
		relnum_ = relevance_.size();
		irrelnum_ = irrelevance_.size();
		dim_ = relevance_[0].size();
	}

	inline void setInitquery(const std::vector<double> initquery)
	{
		initquery_ = Eigen::VectorXd::Zero(dim_);
		if (count_ > 0)
			for (int i = 0; i < dim_; ++i)
				initquery_[i] = initquery[i];
		++count_;
	}

	inline void calcAverage()
	{
		if (relnum_ > 0)
			average(relevance_, relevance_ave_);
		else
			relevance_ave_ = Eigen::VectorXd::Zero(dim_);

		if (irrelnum_ > 0)
			average(irrelevance_, irrelevance_ave_);
		else
			irrelevance_ave_ = Eigen::VectorXd::Zero(dim_);
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
		newquery_ = (alpha * initquery_.array()) + (beta * relevance_ave_.array())
				- (gamma * irrelevance_ave_.array());
	}

	inline void getquery(std::vector<double>* query) const
	{
		query->resize(dim_);
		for (int i = 0; i < dim_; ++i)
			(*query)[i] = newquery_[i];
	}
};

#endif /* SRC_ROCCHIO_H_ */
