#ifndef SRC_WRITER_H_
#define SRC_WRITER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class SampleWriter
{
public:
	std::string samplefile_;
	std::vector<std::vector<int>> positives_;
	std::vector<std::vector<int>> negatives_;
	std::vector<std::vector<int>> neighbors_;
	int iter_;


public:
	SampleWriter(const std::string& samplefile)
	{
		samplefile_ = samplefile;
		iter_ = 0;
	}

	void init_write()
	{
		std::ofstream writer(samplefile_, std::ios::trunc);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << samplefile_ << std::endl;

		writer << "{" << std::endl;
		writer << "  \"iter0\":{\"positive\":[]," << std::endl;
		writer << "           \"negative\":[]," << std::endl;
		writer << "           \"neighbor\":[]}" << std::endl;
		writer << "}" << std::endl;
	}

	void write(const std::vector<int>& positive, const std::vector<int>& negative, const std::vector<int>& neighbor)
	{
		positives_.push_back(positive);
		negatives_.push_back(negative);
		neighbors_.push_back(neighbor);
		iter_++;

		std::ofstream writer(samplefile_, std::ios::trunc);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << samplefile_ << std::endl;

		writer << "{" << std::endl;
		writer << "  \"iter0\":{\"positive\":[]," << std::endl;
		writer << "           \"negative\":[]," << std::endl;
		writer << "           \"neighbor\":[]}," << std::endl;
		for (int i = 0; i < iter_; ++i)
		{
			writer << "  \"iter" << i + 1 << "\":{";
			writer << "\"positive\":[";
			for (int j = 0; j < (int)positives_[i].size(); ++j)
			{
				writer << positives_[i][j];
				if (j < (int)positives_[i].size() - 1)
					writer << ", ";
			}
			writer << "]," << std::endl;

			writer << "           \"negative\":[";
			for (int j = 0; j < (int) negatives_[i].size(); ++j)
			{
				writer << negatives_[i][j];
				if (j < (int) negatives_[i].size() - 1)
					writer << ", ";
			}
			writer << "]," << std::endl;

			writer << "           \"neighbor\":[";
			for (int j = 0; j < (int) neighbors_[i].size(); ++j)
			{
				writer << neighbors_[i][j];
				if (j < (int) neighbors_[i].size() - 1)
					writer << ", ";
			}
			writer << "]}";

			if (i < iter_ - 1)
				writer << ",";
			writer << std::endl;
		}
		writer << "}" << std::endl;
		writer.close();
	}
};


class Logger
{
public:
	std::string feedbackfile_;
	std::string candidatefile_;
	std::string pysettingfile_;
	std::string npyfile_;
	std::vector<std::vector<int>> candidates_;
	int dim_;
	int iter_;


public:
	void setup(const std::string& feedbackfile, const std::string& candidatefile, const std::string& pysettingfile, const std::string& npyfile, const int dim)
	{
		feedbackfile_ = feedbackfile;
		candidatefile_ = candidatefile;
		pysettingfile_ = pysettingfile;
		npyfile_ = npyfile;
		dim_ = dim;
		iter_ = 0;
	}

	void writeCandidate(const std::vector<int>& candidate)
	{
		candidates_.push_back(candidate);
		iter_++;

		std::ofstream writer(candidatefile_, std::ios::trunc);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << candidatefile_ << std::endl;

		writer << "{" << std::endl;
		for (int i = 0; i < iter_; ++i)
		{
			writer << "  \"iter" << i << "\":[";
			for (int j = 0; j < (int)candidates_[i].size(); ++j)
			{
				writer << candidates_[i][j];
				if (j < (int)candidates_[i].size() - 1)
					writer << ", ";
			}
			writer << "]";

			if (i < iter_ - 1)
				writer << ",";
			writer << std::endl;
		}
		writer << "}" << std::endl;
		writer.close();
	}

	void writePySetting()
	{
		std::ofstream writer(pysettingfile_, std::ios::trunc);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << candidatefile_ << std::endl;

		writer << "{" << std::endl;
		writer << "  \"feedback_file\":\"" << feedbackfile_ << "\"," << std::endl;
		writer << "  \"input_file\":\"" << npyfile_ << "\"," << std::endl;
		writer << "  \"unit\":" << dim_ << std::endl;
		writer << "}" << std::endl;
		writer.close();
	}

};


class TestWriter
{
public:
	std::string filename_;


public:
	void setup(const std::string filename)
	{
		filename_ = filename;
	}

	void settings(const int searchTarget, const std::string selection_method)
	{
		std::ofstream writer(filename_, std::ios::trunc);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << filename_ << std::endl;

		writer << "Search target: " << searchTarget << std::endl;
		writer << "Selection method: " << selection_method << std::endl;
	}

	void target_found(const int epoch, const float time, const int selection_count)
	{
		std::ofstream writer(filename_, std::ios::app);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << filename_ << std::endl;

		writer << "Total Search: " << epoch << std::endl;
		writer << "Total Search Time: " << time << std::endl;
		writer << "Total Selection Count: " << selection_count << std::endl;
	}

};


class ActiveLog
{
public:
	std::string filename_;


public:
	void setup(const std::string filename)
	{
		filename_ = filename;
	}


	void Write(const int epoch, const std::string msg)
	{
		std::ofstream writer(filename_, std::ios::app);
		if (!writer)
			std::cerr << "[Warning] Cannot open the specified file. " << filename_ << std::endl;

		writer << "[iter" << epoch << "] " << msg << std::endl;;
	}

};

#endif /* SRC_WRITER_H_ */
