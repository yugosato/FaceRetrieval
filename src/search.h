#ifndef SRC_SEARCH_H_
#define SRC_SEARCH_H_

#include <iostream>
#include <vector>
#include <NGT/Index.h>
#include "rocchio.h"

class Search
{
public:
	NGT::Index* index_;
	NGT::Object* query_ = 0;
	NGT::ObjectDistances objects_;
	std::string indexFile_;
	std::vector<std::vector<double>> matrix_;
	std::vector<double> updatedquery_;
	int phase_;

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
	}

	void setInput(const int clickNo)
	{
		clickNo_ = clickNo;
		query_ = 0;
		query_ = index_->allocateObject(matrix_[clickNo_]);
	}

	void setInput_multi(std::vector<int>& rel, std::vector<int>& inrel)
	{
		Rocchio* rocchio;
		rocchio = new Rocchio;

		std::vector<std::vector<double>> relVec;
		std::vector<std::vector<double>> inrelVec;

		for (int i = 0; i < (int) rel.size(); ++i)
			relVec.push_back(matrix_[rel[i]]);

		for (int i = 0; i < (int) inrel.size(); ++i)
			inrelVec.push_back(matrix_[inrel[i]]);

		rocchio->setRelevance(relVec);
		rocchio->setInRelevance(inrelVec);
		rocchio->setInitquery(updatedquery_, phase_);
		rocchio->calcAverage();
		rocchio->calculate(1.0f, 0.8f, 0.2f);
		rocchio->getquery(&updatedquery_);

		query_ = 0;
		query_ = index_->allocateObject(updatedquery_);
		phase_++;

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
			NGT::SearchContainer sc(*query_);

			sc.setResults(&objects_);
			sc.setSize(size_);
			sc.setRadius(radius_);
			sc.setEpsilon(epsilon_);

			index_->search(sc);
			index_->deleteObject(query_);
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
