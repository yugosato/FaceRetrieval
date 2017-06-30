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
	int iter_;


public:
	SampleWriter(const std::string& samplefile)
	{
		samplefile_ = samplefile;
		iter_ = 0;
	}

	void write_init()
	{
		std::ofstream writer(samplefile_, std::ios::trunc);
		if (!writer)
			std::cerr << "[warning] cannot open the specified file. " << samplefile_ << std::endl;

		writer << "{" << std::endl;
		writer << "  \"iter0\":{\"positive\":[]," << std::endl;
		writer << "           \"negative\":[]}" << std::endl;
		writer << "}" << std::endl;
	}

	void write(const std::vector<int>& positive, const std::vector<int>& negative)
	{
		positives_.push_back(positive);
		negatives_.push_back(negative);
		iter_++;

		std::ofstream writer(samplefile_, std::ios::trunc);
		if (!writer)
			std::cerr << "[warning] Cannot open the specified file. " << samplefile_ << std::endl;

		writer << "{" << std::endl;
		writer << "  \"iter0\":{\"positive\":[]," << std::endl;
		writer << "           \"negative\":[]}," << std::endl;
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
	std::string candidatefile_;
	std::string pysettingfile_;
	std::string npyfile_;
	std::vector<std::vector<int>> candidates_;
	int dim_;
	int iter_;


public:
	void setup(const std::string& candidatefile, const std::string& pysettingfile, const std::string& npyfile, const int dim)
	{
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
			std::cerr << "[warning] cannot open the specified file. " << candidatefile_ << std::endl;

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
			std::cerr << "[warning] cannot open the specified file. " << candidatefile_ << std::endl;

		writer << "{" << std::endl;
		writer << "  \"input_file\":\"" << npyfile_ << "\"," << std::endl;
		writer << "  \"unit\":" << dim_ << std::endl;
		writer << "}" << std::endl;
		writer.close();
	}
};

#endif /* SRC_WRITER_H_ */

