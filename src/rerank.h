#ifndef SRC_RERANK_H_
#define SRC_RERANK_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "ofMain.h"
#include "NGT/Index.h"


template<typename Sequence, typename BinaryPredicate>
struct IndexCompareT
{
	IndexCompareT(const Sequence& seq, const BinaryPredicate comp) :
			seq_(seq), comp_(comp)
	{
	}
	bool operator()(const size_t a, const size_t b) const
	{
		return comp_(seq_[a], seq_[b]);
	}
	const Sequence seq_;
	const BinaryPredicate comp_;
};

template<typename Sequence, typename BinaryPredicate>
IndexCompareT<Sequence, BinaryPredicate> IndexCompare(const Sequence& seq,
		const BinaryPredicate comp)
{
	return IndexCompareT<Sequence, BinaryPredicate>(seq, comp);
}

template<typename Sequence, typename BinaryPredicate>
std::vector<int> ArgSort(const Sequence& seq, BinaryPredicate func)
{
	std::vector<int> index(seq.size());
	for (int i = 0; i < (int) index.size(); i++)
	{
		index[i] = i;
	}
	std::sort(index.begin(), index.end(), IndexCompare(seq, func));

	return index;
}


class ReRank: public ofThread
{
public:
	std::string featuresfile_;
	std::vector<double> queryvector_;
	std::vector<std::vector<double>> features_;
	std::vector<int> number_result_;
	std::vector<int> number_reranked_;
	size_t size_;
	size_t dim_;
	bool isReranked_;


public:
	void setup(const std::string& featuresfile)
	{
		featuresfile_ = featuresfile;
		isReranked_ = false;
	}

	inline void load()
	{
		std::ifstream ifs(featuresfile_);
		if (!ifs)
			std::cerr << "[Warning] Cannot open the specified file. " << featuresfile_ << std::endl;
		else
		{
			std::string line;
			while (getline(ifs, line))
			{
				std::vector<std::string> tokens;
				NGT::Common::tokenize(line, tokens, "\t");
				std::vector<double> obj;
				for (std::vector<std::string>::iterator ti = tokens.begin(); ti != tokens.end(); ++ti)
					obj.push_back(NGT::Common::strtod(*ti));
				features_.push_back(obj);
			}
			dim_ = features_[0].size();
		}
	}

	void set_result(const std::vector<int>& number_result)
	{
		number_result_ = number_result;
	}

	void set_queryvector(const std::vector<double>& queryvector)
	{
		queryvector_ = queryvector;
	}

	inline void threadedFunction()
	{
		std::cout << "[ReRank] Start reranking results by trained model." << std::endl;
		isReranked_ = false;
		lock();

		size_ = number_result_.size();
		std::vector<double> distance;
		distance.resize(size_);

		double cos_distance = 0;
		size_t i = 0;
		while(i < size_)
		{
			cos_distance = cosine_distance(queryvector_, features_[i]);
			distance[i] = cos_distance;
			++i;
		}

		number_reranked_.clear();
		number_reranked_ = ArgSort(distance, std::less<double>());

		unlock();
		isReranked_ = true;
		std::cout << "[ReRank] Finished reranking results by trained model." << std::endl;
	}

	inline double cosine_distance(const std::vector<double>& a, const std::vector<double>& b)
	{
		// Calculate the norm of A and B (the supplied vector).
		double normA = 0.0F;
		double normB = 0.0F;
		double sum = 0.0F;

		size_t loc = 0;
		while (loc < dim_)
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

		return 1.0 - cosine;
	}

	inline void getNumber(std::vector<int>* number) const
	{
		number->resize(size_);
		size_t i = 0;
		while (i < size_)
		{
			(*number)[i] = number_reranked_[i];
			++i;
		}
	}

};

#endif /* SRC_RERANK_H_ */
