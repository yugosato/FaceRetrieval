#ifndef SRC_TRAINER_H_
#define SRC_TRAINER_H_

#define BOOST_PYTHON_STATIC_LIB

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include "boost/python.hpp"
#include "boost/python/numpy.hpp"


class Trainer
{
public:
	std::string pythonfile_;
	std::string script_;
	boost::python::object main_module_;
	boost::python::object main_namespace_;
	std::vector<std::vector<float>> weight_;
	std::vector<float> bias_;


public:
	void setupPython(const std::string& pythonfile)
	{
		pythonfile_ = pythonfile;
		std::ifstream ifs(pythonfile_);
		if (!ifs)
			std::cerr << "[BoostPython] Cannot open the specified file. " << pythonfile_ << std::endl;

		std::string script((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		script_ = script;

		char python_home[] = "/home/yugo/anaconda2";
		Py_SetPythonHome(python_home);
		Py_Initialize();
		main_module_ = boost::python::import("__main__");
		main_namespace_ = main_module_.attr("__dict__");
	}

	void run()
	{
		try
		{
			boost::python::exec(script_.c_str(), main_namespace_);
			getWeight();
			getBias();
			std::cout << 1 << std::endl;
		}
		catch(boost::python::error_already_set const &)
		{
			PyErr_Print();
		}
	}

	void getWeight()
	{
		auto w = main_namespace_["weight"];
		int rows = boost::python::len(w);
		int cols = boost::python::len(w[0]);

		weight_.resize(rows);
		for (int i = 0; i < rows; ++i)
		{
			weight_[i].resize(cols);
			for(int j = 0; j < cols; ++j)
			{
				std::string str = boost::python::extract<std::string>(boost::python::str(w[i][j]));
				weight_[i][j] = std::atof(str.c_str());
			}
		}
	}

	void getBias()
	{
		auto b = main_namespace_["bias"];
		int cols = boost::python::len(b);

		bias_.resize(cols);
		for (int j = 0; j < cols; ++j)
		{
			std::string str = boost::python::extract<std::string>(boost::python::str(b[j]));
			bias_[j] = std::atof(str.c_str());
		}
	}

};


#endif /* SRC_TRAINER_H_ */

