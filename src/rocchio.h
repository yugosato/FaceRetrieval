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
#include <Eigen/Core>
#include <Eigen/Geometry>


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


public:

	void setRelevance(const std::vector<std::vector<double>>& relVec)
	{
		relVec_ = relVec;
		dim_ = relVec_[0].size();
	}


	void setInRelevance(const std::vector<std::vector<double>>& inrelVec)
	{
		inrelVec_ = inrelVec;
	}


	void setInitquery(const std::vector<double>& initquery, const int phase)
	{
		initquery_ = Eigen::VectorXd::Zero(dim_);
		if (phase > 0)
		{
			for (int i = 0; i < dim_; ++i)
				initquery_[i] = initquery[i];
		}
	}


	void calcAverage()
	{
		average(relVec_, relVec_ave_);
		average(inrelVec_, inrelVec_ave_);
	}


	void average(const std::vector<std::vector<double>>& srcVec, Eigen::VectorXd& dstVec)
	{
		const int size = srcVec.size();
		Eigen::VectorXd average = Eigen::VectorXd::Zero(dim_);

		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < dim_; ++j)
			{
				average[j] += srcVec[i][j];
			}
		}

		average = average.array() / size;
		dstVec = average;
	}


	void calculate(const float alpha, const float beta, const float gamma)
	{
		newquery_ = Eigen::VectorXd::Zero(dim_);
		newquery_ = (alpha * initquery_.array()) + (beta * relVec_ave_.array())
				- (gamma * inrelVec_ave_.array());
	}


	void getquery(std::vector<double>* query) const
	{
		query->resize(dim_);
		for (int i = 0; i < dim_; ++i)
			(*query)[i] = newquery_[i];
	}

};

#endif /* SRC_ROCCHIO_H_ */
