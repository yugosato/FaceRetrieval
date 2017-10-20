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
	std::vector<std::vector<double>> features_;
	std::vector<std::vector<double>> positive_features_;
	std::vector<std::vector<double>> negative_features_;
	std::vector<double> queryvector_;
	Eigen::VectorXd queryvector_eigen_;
	Eigen::VectorXd positive_ave_;
	Eigen::VectorXd negative_ave_;
	int dim_;
	int positive_num_;
	int negative_num_;
	float alpha_, beta_, gamma_;
	float process_time_;


public:
	Rocchio()
	{
		dim_ = 4096;
		positive_num_ = 0;
		negative_num_ = 0;
		queryvector_eigen_ = Eigen::VectorXd::Zero(dim_);
		alpha_ = 1.0f;
		beta_ = 0.8f;
		gamma_ = 0.3f;
		process_time_ = 0.0f;
	}

	void set_features(const std::vector<std::vector<double>>& features)
	{
		features_ = features;
	}

	void setInput_multi(const std::vector<int>& positives, const std::vector<int>& negatives)
	{
		positive_features_.clear();
		negative_features_.clear();

		positive_features_.resize(positives.size());
		negative_features_.resize(negatives.size());

		for (int i = 0; i < (int) positives.size(); ++i)
			positive_features_[i] = features_[positives[i]];

		for (int i = 0; i < (int) negatives.size(); ++i)
			negative_features_[i] = features_[negatives[i]];

		positive_num_ = positives.size();
		negative_num_ = negatives.size();
	}

	void set_weight(const float alpha, const float beta, const float gamma)
	{
		alpha_ = alpha;
		beta_ = beta;
		gamma_ = gamma;
	}

	inline void run()
	{
		std::cout << "[Rocchio] Start calculating query vector by rocchio algorithm." << std::endl;
		std::cout << "[Rocchio] Alpha: " << alpha_ << ", Beta: " << beta_ << ", Gamma: " << gamma_ << std::endl;
		float start = ofGetElapsedTimef();

		calcAverage();

		Eigen::VectorXd term1 = alpha_ * queryvector_eigen_.array();
		Eigen::VectorXd term2 = beta_ * positive_ave_.array();
		Eigen::VectorXd term3 = gamma_ * negative_ave_.array();

		queryvector_eigen_ = term1 + term2 - term3;
		queryvector_ = eigen2stlvector(queryvector_eigen_);

		process_time_ = ofGetElapsedTimef() - start;
		std::cout << "[Rocchio] Finished calculating query vector by rocchio algorithm." << std::endl;
	}


private:
	void calcAverage()
	{
		if (positive_num_ > 0)
			average(positive_features_, positive_ave_);
		else
			positive_ave_ = Eigen::VectorXd::Zero(dim_);

		if (negative_num_ > 0)
			average(negative_features_, negative_ave_);
		else
			negative_ave_ = Eigen::VectorXd::Zero(dim_);
	}

	void average(const std::vector<std::vector<double>>& srcVec, Eigen::VectorXd& dstVec)
	{
		const int size = srcVec.size();
		Eigen::VectorXd average = Eigen::VectorXd::Zero(dim_);

		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < dim_; ++j)
				average(j) += srcVec[i][j];
		}

		average = average.array() / size;
		dstVec = average;
	}

	std::vector<double> eigen2stlvector(const Eigen::VectorXd& eigenvector)
	{
		int size = (int) eigenvector.size();
		int i = 0;
		std::vector<double> stlvector(size);
		while (i < size)
		{
			stlvector[i] = eigenvector(i);
			++i;
		}

		return stlvector;
	}
};

#endif /* SRC_ROCCHIO_H_ */
