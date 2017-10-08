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
	std::vector<double> queryvector_;
	bool isSearched_;
	int clickNo_;
	int size_;
	const float radius_ = FLT_MAX;
	const float epsilon_ = 0.1;
	float process_time_;


public:
	void setup(const std::string indexFile, const int size)
	{
		clickNo_ = -1;
		indexFile_ = indexFile;
		size_ = size;
		index_ = new NGT::Index(indexFile_);
		isSearched_ = false;
	}

	void set_queryvector(const std::vector<double>& queryvector)
	{
		queryvector_ = queryvector;
	}

	inline void threadedFunction()
	{
		try
		{
			std::cout << "[Search] Start searching." << std::endl;
			isSearched_ = false;
			lock();
			float start = ofGetElapsedTimef();

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
	}

};

#endif /* SRC_SEARCH_H_ */
