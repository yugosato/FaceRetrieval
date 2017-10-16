#ifndef SRC_DISTANCE_H_
#define SRC_DISTANCE_H_

#include <iostream>
#include <vector>
#include <assert.h>


inline float mycosine(const std::vector<double>& a, const std::vector<double>& b, const std::string mode)
{
	// Calculate the norm of A and B (the supplied vector).
	double normA = 0.0F;
	double normB = 0.0F;
	double sum = 0.0F;

	int dim = a.size();
	int loc = 0;
	while (loc < dim)
	{
		normA += (double) a[loc] * (double) a[loc];
		normB += (double) b[loc] * (double) b[loc];
		sum += (double) a[loc] * (double) b[loc];
		++loc;
	}

	assert(normA > 0.0f);
	assert(normB > 0.0f);

	// Compute the dot product of the two vectors.
	double cosine = sum / (sqrt(normA) * sqrt(normB));

	if (mode == "distance")
		return 1.0 - cosine;
	else if (mode == "similarity")
		return cosine;
	else
	{
		std::cout << "[ofApp] Cannot compute specified metric: " << mode << std::endl;
		std::abort();
		return -1;
	}
}



#endif /* SRC_DISTANCE_H_ */
