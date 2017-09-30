#ifndef SRC_SEARCH_H_
#define SRC_SEARCH_H_

#include <iostream>
#include <vector>
#include <stdio.h>
#include "NGT/Index.h"
#include "rocchio.h"


class Search: public ofThread
{
public:
	NGT::Index* index_;
	NGT::ObjectDistances objects_;
	std::string indexFile_;
	std::vector<std::vector<double>> features_;
	std::vector<double> queryvector_;
	std::vector<std::vector<double>> relevance_;
	std::vector<std::vector<double>> irrelevance_;
	bool isSearched_;
	int clickNo_;
	const int size_ = 500;
	const float radius_ = FLT_MAX;
	const float epsilon_ = 0.1;
	Rocchio* rocchio_;
	float process_time_;


public:
	void setup(const std::string indexFile)
	{
		clickNo_ = -1;
		indexFile_ = indexFile;
		index_ = new NGT::Index(indexFile_);
		isSearched_ = false;
		rocchio_ = new Rocchio;
	}

	void set_features(const std::vector<std::vector<double>>& features)
	{
		features_ = features;
	}

	inline void setInput_multi(const std::vector<int>& positives, const std::vector<int>& negatives)
	{
		relevance_.clear();
		irrelevance_.clear();

		relevance_.resize(positives.size());
		irrelevance_.resize(negatives.size());

		for (int i = 0; i < (int) positives.size(); ++i)
			relevance_[i] = features_[positives[i]];

		for (int i = 0; i < (int) negatives.size(); ++i)
			irrelevance_[i] = features_[negatives[i]];
	}

	inline void threadedFunction()
	{
		try
		{
			std::cout << "[Search] Start searching." << std::endl;
			isSearched_ = false;
			lock();
			float start = ofGetElapsedTimef();

			// Rocchio Algorithm.
			rocchio_->set_vector(relevance_, irrelevance_);
			rocchio_->setInitquery(queryvector_);
			rocchio_->calculate(1.0f, 0.8f, 0.3f);
			rocchio_->getquery(&queryvector_);

			// NGT Search.
			NGT::Object* query = 0;
			query = index_->allocateObject(queryvector_);

			NGT::SearchContainer sc(*query);
			NGT::ObjectDistances objects;

			sc.setResults(&objects);
			sc.setSize(size_);
			sc.setRadius(radius_);
			sc.setEpsilon(epsilon_);

			index_->search(sc);
			index_->deleteObject(query);

			objects_.clear();
			NGT::ObjectDistances().swap(objects_);

			objects_ = objects;

			process_time_ = ofGetElapsedTimef() - start;
			unlock();
			isSearched_ = true;
			std::cout << "[Search] Finished Searching." << std::endl;
		}
		catch (NGT::Exception &err)
		{
			std::cerr << "[Warning]" << err.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << "[Warning] Unknown error in searching" << std::endl;
		}
	}

	inline void getNumber(std::vector<int>* number) const
	{
		number->clear();
		number->resize(size_);
		for (int i = 0; i < size_; ++i)
			(*number)[i] = (int)objects_[i].id - 1;
	}

	virtual ~Search()
	{
		delete index_;
		delete rocchio_;
	}

};

#endif /* SRC_SEARCH_H_ */
