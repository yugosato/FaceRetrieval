#ifndef SRC_SEARCH_H_
#define SRC_SEARCH_H_

#include <iostream>
#include <vector>
#include <stdio.h>
#include "NGT/Index.h"
#include "rocchio.h"


class Search
{
public:
	NGT::Index* index_;
	NGT::ObjectDistances objects_;
	std::string indexFile_;
	std::vector<std::vector<double>> matrix_;
	std::vector<double> queryvector_;
	int phase_;
	bool issearched_;
	int clickNo_;
	const int size_ = 100;
	const float radius_ = FLT_MAX;
	const float epsilon_ = 0.1;


public:
	void setup(const std::string indexFile)
	{
		clickNo_ = -1;
		indexFile_ = indexFile;
		index_ = new NGT::Index(indexFile_);
		phase_ = 0;
		issearched_ = false;
	}

	void setInput(const int clickNo)
	{
		issearched_ = false;
		clickNo_ = clickNo;
		queryvector_ = matrix_[clickNo_];
	}

	void setInput_multi(const std::vector<int>& rel, const std::vector<int>& inrel)
	{
		issearched_ = false;
		Rocchio* rocchio;
		rocchio = new Rocchio;

		std::vector<std::vector<double>> relVec;
		std::vector<std::vector<double>> inrelVec;
		relVec.reserve(rel.size());
		inrelVec.reserve(inrel.size());

		for (int i = 0; i < (int) rel.size(); ++i)
			relVec.push_back(matrix_[rel[i]]);

		for (int i = 0; i < (int) inrel.size(); ++i)
			inrelVec.push_back(matrix_[inrel[i]]);

		rocchio->setRelevance(relVec);
		rocchio->setInRelevance(inrelVec);
		rocchio->setInitquery(queryvector_, phase_);
		rocchio->calculate(1.0f, 0.8f, 0.1f);
		rocchio->getquery(&queryvector_);

		phase_++;

		relVec.clear();
		inrelVec.clear();
		std::vector<std::vector<double>>().swap(relVec);
		std::vector<std::vector<double>>().swap(inrelVec);

		delete rocchio;
	}

	void setMatrix(const std::vector<std::vector<double>>& matrix)
	{
		matrix_ = matrix;
	}

	void search()
	{
		try
		{
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
			issearched_ = true;
		}
		catch (NGT::Exception &err)
		{
			std::cerr << "Error " << err.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << "Error" << std::endl;
		}
	}

	void getNumber(std::vector<int>* number) const
	{
		number->resize(size_);
		for (int i = 0; i < size_; ++i)
			(*number)[i] = (int) objects_[i].id - 1;
	}

	virtual ~Search()
	{
		delete index_;
	}

};

#endif /* SRC_SEARCH_H_ */
