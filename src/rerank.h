#ifndef SRC_RERANK_H_
#define SRC_RERANK_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "ofMain.h"
#include "rocchio.h"
#include "sorting.h"
#include "distance.h"


class ReRank: public ofThread
{
public:
	std::vector<double> queryvector_;
	std::vector<std::vector<double>> features_;
	std::vector<int> init_result_;
	std::vector<int> reranked_result_;
	int size_;
	bool isReranked_;
	float process_time_;


public:
	ReRank()
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

	void set_queryvector(const std::vector<double>& queryvector)
	{
		queryvector_ = queryvector;
	}

	inline void threadedFunction()
	{
		std::cout << "[ReRank] Start reranking results by rocchio query vector." << std::endl;
		isReranked_ = false;
		lock();
		float start = ofGetElapsedTimef();

		std::vector<double> distance;
		distance.resize(size_);

		double cos_distance = 0;
		int i = 0;
		while(i < size_)
		{
			cos_distance = cosine(queryvector_, features_[init_result_[i]], "distance");
			distance[i] = cos_distance;
			++i;
		}

		std::vector<int> reranked_idx = ArgSort(distance, std::less<double>());
		sort_result_by_index(reranked_idx);

		process_time_ = ofGetElapsedTimef() - start;
		unlock();
		isReranked_ = true;
		std::cout << "[ReRank] Finished reranking results by rocchio query vector." << std::endl;
	}

	inline void getNumber(std::vector<int>* number) const
	{
		number->clear();
		number->resize(size_);
		int i = 0;
		while (i < size_)
		{
			(*number)[i] = reranked_result_[i];
			++i;
		}
	}


private:
	void sort_result_by_index(const std::vector<int>& index)
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
};

#endif /* SRC_RERANK_H_ */
