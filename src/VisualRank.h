#ifndef SRC_VISURALRANK_H_
#define SRC_VISURALRANK_H_

#undef Success

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "ofMain.h"
#include "NGT/Index.h"
#include "Eigen/Core"
#include "Eigen/Geometry"
#include "sorting.h"


class VisualRank: public ofThread
{
public:
	std::vector<std::vector<double>> features_;
	std::vector<int> init_result_;
	std::vector<int> reranked_result_;
	Eigen::MatrixXf similarity_matrix_;
	int size_;
	bool isReranked_;
	float process_time_;


public:
	VisualRank()
	{
		size_ = 0;
		isReranked_ = false;
		process_time_ = 0.0f;
	}

	void set_features(const std::vector<std::vector<double>>& features)
	{
		features_ = features;
	}

	void set_init_result(const std::vector<int>& result)
	{
		init_result_ = result;
		size_ = (int) init_result_.size();
	}

	inline void make_similarityMatrix()
	{
		similarity_matrix_ = Eigen::MatrixXf(size_, size_);

		// Calculate similarity.
		int i = 0;
		while (i < size_)
		{
			int j = 0;
			while (j < size_)
			{
				if (i == j)
					similarity_matrix_(i, j) = 1.0;
				else if (i < j)
					similarity_matrix_(i, j) = cosine_similarity(features_[init_result_[i]], features_[init_result_[j]]);
				else
					similarity_matrix_(i, j) = similarity_matrix_(j, i);
				++j;
			}
			++i;
		}

		// Normalize values of each col (sum of cols == 1.0).
		Eigen::VectorXf col_sum = similarity_matrix_.colwise().sum();
		int k = 0;
		while (k < size_)
		{
			int l = 0;
			while (l < size_)
			{
				similarity_matrix_(k, l) /= col_sum(l);
				++l;
			}
			++k;
		}
	}

	inline void threadedFunction()
	{
		std::cout << "[ReRank] Start reranking results by VisualRank." << std::endl;
		isReranked_ = false;
		lock();
		float start = ofGetElapsedTimef();

		make_similarityMatrix();

		Eigen::VectorXf init_visualrank = init_visualrank_score(false);
		Eigen::VectorXf bias = Eigen::VectorXf::Constant(size_, 1.0f / (float) size_);
		Eigen::VectorXf visualrank = init_visualrank;

		const int alpha = 1.0;
		int iter = 10;

		int i = 0;
		while (i < iter)
		{
			visualrank = alpha * (similarity_matrix_ * visualrank).array(); // + (1 - alpha) * bias.array();
			++i;
		}

		std::vector<float> score = eigen2stlvector(visualrank);
		std::vector<int> reranked_idx = ArgSort(score, std::greater<float>());
		sort_result_by_index(reranked_idx);

		process_time_ = ofGetElapsedTimef() - start;
		unlock();
		isReranked_ = true;
		std::cout << "[ReRank] Finished reranking results by VisuralRank." << std::endl;
	}

	Eigen::VectorXf init_visualrank_score(const bool& Constant)
	{
		Eigen::VectorXf rankscore(size_);

		if (!Constant)
		{
			float score = (float) size_;
			int i = 0;
			while (i < size_)
			{
				rankscore(i) = score;
				--score;
				++i;
			}
			rankscore /= rankscore.sum();
		}
		else
		{
			rankscore = Eigen::VectorXf::Constant(size_, 1.0f / (float) size_);
		}

		return rankscore;
	}

	inline void sort_result_by_index(const std::vector<int>& index)
	{
		reranked_result_.clear();
		reranked_result_.resize(size_);

		int i = 0;
		while (i < size_)
		{
			reranked_result_[i] = init_result_[index[i]];
			++i;
		}
	}

	inline double cosine_similarity(const std::vector<double>& a, const std::vector<double>& b)
	{
		// Calculate the norm of A and B (the supplied vector).
		size_t dim = features_[0].size();
		double normA = 0.0F;
		double normB = 0.0F;
		double sum = 0.0F;

		size_t loc = 0;
		while (loc < dim)
		{
			normA += (double) a[loc] * (double) a[loc];
			normB += (double) b[loc] * (double) b[loc];
			sum += (double) a[loc] * (double) b[loc];
			++loc;
		}

		assert(normA > 0.0f);
		assert(normB > 0.0f);

		// Compute the dot product of the two vectors.
		double cosine = sum / (sqrt(normA) * sqrt(normB));

		return cosine;
	}

	// For visualrank score.
	std::vector<float> eigen2stlvector(const Eigen::VectorXf& eigenvector)
	{
		int size = (int) eigenvector.size();
		int i = 0;
		std::vector<float> stlvector(size);
		while (i < size)
		{
			stlvector[i] = eigenvector(i);
			++i;
		}

		return stlvector;
	}

	inline void getNumber(std::vector<int>* number) const
	{
		number->resize(size_);
		int i = 0;
		while (i < size_)
		{
			(*number)[i] = reranked_result_[i];
			++i;
		}
	}

};


#endif /* SRC_VISURALRANK_H_ */