#ifndef SRC_SELECTION_H_
#define SRC_SELECTION_H_

#define BOOST_PYTHON_STATIC_LIB

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include "ofMain.h"
#include "boost/python.hpp"
#include "boost/python/numpy.hpp"


class Selection: public ofThread
{
public:
	std::string pythonfile_;
	std::string script_;
	std::string positive_indexfile_;
	std::string negative_indexfile_;
	std::string random_indexfile_;
	std::string uncertain_indexfile_;
	std::vector<int> number_estPositive_;
	std::vector<int> number_estNegative_;
	std::vector<int> number_Uncertain_;
	std::vector<int> number_Random_;
	boost::python::object main_module_;
	boost::python::object main_namespace_;
	boost::python::object selection_;
	bool isSelected_;


public:
	void setup(const std::string& pythonfile, const std::string& positive_indexfile, const std::string& negative_indexfile,
			const std::string& uncertain_indexfile, const std::string& random_indexfile)
	{
		pythonfile_ = pythonfile;
		std::ifstream ifs(pythonfile_);
		if (!ifs)
			std::cerr << "[Warning] Cannot open the specified file. " << pythonfile_ << std::endl;
		std::string script((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

		script_ = script;
		positive_indexfile_ = positive_indexfile;
		negative_indexfile_ = negative_indexfile;
		uncertain_indexfile_ = uncertain_indexfile;
		random_indexfile_ = random_indexfile;
		isSelected_ = false;
	}

	void threadedFunction()
	{
		try
		{
			isSelected_ = false;
			lock();
			char python_home[] = "/home/yugo/anaconda2";
			Py_SetPythonHome(python_home);
			Py_Initialize();
			main_module_ = boost::python::import("__main__");
			main_namespace_ = main_module_.attr("__dict__");
			boost::python::exec(script_.c_str(), main_namespace_, main_namespace_);
			selection_ = main_namespace_["active_selection"];

			selection_();
			read_index(positive_indexfile_, number_estPositive_);
			read_index(negative_indexfile_, number_estNegative_);
			read_index(uncertain_indexfile_, number_Uncertain_);
			read_index(random_indexfile_, number_Random_);
			unlock();
			isSelected_ = true;
		}
		catch(boost::python::error_already_set const &)
		{
			PyErr_Print();
		}
	}

	void read_index(const std::string fname, std::vector<int>& vec)
	{
		std::ifstream ifs(fname);
		if (!ifs)
			std::cerr << "[Warning] Cannot open the specified file. " << fname << std::endl;

		vec.clear();
		std::vector<int>().swap(vec);
		std::string buf;

		while (ifs && std::getline(ifs, buf))
		{
			int index = std::atoi(buf.c_str());
			vec.push_back(index);
		}
	}

	void getNumber_Uncertain(std::vector<int>* number) const
	{
		int size = number_Uncertain_.size();
		number->resize(size);
		for (int i = 0; i < size; ++i)
			(*number)[i] = number_Uncertain_[i];
	}

};


#endif /* SRC_SELECTION_H_ */
