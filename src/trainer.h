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


class Trainer: public ofThread
{
public:
	std::string pythonfile_;
	std::string script_;
	boost::python::object main_module_;
	boost::python::object main_namespace_;
	boost::python::object trainer_;
	boost::python::object extracter_;
	bool isTrained_;


public:
	void setup(const std::string& pythonfile)
	{
		pythonfile_ = pythonfile;
		std::ifstream ifs(pythonfile_);
		if (!ifs)
			std::cerr << "[Warning] Cannot open the specified file. " << pythonfile_ << std::endl;

		std::string script((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		script_ = script;


		isTrained_ = false;
	}

	void threadedFunction()
	{
		try
		{
			isTrained_ = false;
			lock();

			char python_home[] = "/home/yugo/anaconda2";
			Py_SetPythonHome(python_home);
			Py_Initialize();
			main_module_ = boost::python::import("__main__");
			main_namespace_ = main_module_.attr("__dict__");
			boost::python::exec(script_.c_str(), main_namespace_, main_namespace_);
			trainer_ = main_namespace_["train_model"];
			extracter_ = main_namespace_["feature_extract"];

			trainer_();
			unlock();
			isTrained_ = true;
		}
		catch(boost::python::error_already_set const &)
		{
			PyErr_Print();
		}
	}

};


#endif /* SRC_TRAINER_H_ */

