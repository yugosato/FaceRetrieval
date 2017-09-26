#ifndef SRC_TRAINER_H_
#define SRC_TRAINER_H_

#define BOOST_PYTHON_STATIC_LIB

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include "ofMain.h"
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
	float process_time_;
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

	inline void threadedFunction()
	{
		try
		{
			std::cout << "[Trainer] Start online training." << std::endl;
			isTrained_ = false;
			lock();
			float start = ofGetElapsedTimef();

			char python_home[] = "/home/yugo/anaconda2";
			Py_SetPythonHome(python_home);
			Py_Initialize();

		    auto main_mod = boost::python::import("__main__");
		    auto main_ns  = main_mod.attr("__dict__");

		    boost::python::exec(script_.c_str(), main_ns);
		    main_mod.attr("main_process")();

		    process_time_ = ofGetElapsedTimef() - start;
		    unlock();
		    isTrained_ = true;
		    std::cout << "[Trainer] Finished online training." << std::endl;
		}
		catch (boost::python::error_already_set const &)
		{
			PyErr_Print();
		}
	}

	~Trainer()
	{
		Py_Finalize();
	}

};


#endif /* SRC_TRAINER_H_ */

