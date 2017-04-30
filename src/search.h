#ifndef SRC_SEARCH_H_
#define SRC_SEARCH_H_

#include <iostream>
#include <vector>
#include <NGT/Index.h>


class Search
{
public:
	std::string indexFile_;
	NGT::Index* index_;

	int clickNo_;
	std::vector<std::vector<double>> matrix_;

	NGT::ObjectDistances objects_;
	const int size_ = 100;
	const float radius_ = FLT_MAX;
	const float epsilon_ = 0.1;


public:
	void setup(const std::string indexFile)
	{
		clickNo_ = -1;
		indexFile_ = indexFile;
		index_ = new NGT::Index(indexFile_);
	}

	void setInput(const int clickNo)
	{
		clickNo_ = clickNo;
	}

	void setMatrix(const std::vector<std::vector<double>>& matrix)
	{
		matrix_ = matrix;
	}

	void search()
	{
		try
		{
			NGT::Object *query = 0;
			query = index_->allocateObject(matrix_[clickNo_]);
			NGT::SearchContainer sc(*query);

			sc.setResults(&objects_);
			sc.setSize(size_);
			sc.setRadius(radius_);
			sc.setEpsilon(epsilon_);

			index_->search(sc);
			index_->deleteObject(query);
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
			(*number)[i] = (int)objects_[i].id - 1;
	}

	virtual ~Search()
	{
		delete index_;
	}

};

#endif /* SRC_SEARCH_H_ */
