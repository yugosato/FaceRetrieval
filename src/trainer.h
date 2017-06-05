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
	boost::python::object weight_;
	boost::python::object bias_;


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
		weight_ = main_namespace_["weight"];
	}

	void getBias()
	{
		bias_ = main_namespace_["bias"];
	}

};


#endif /* SRC_TRAINER_H_ */

