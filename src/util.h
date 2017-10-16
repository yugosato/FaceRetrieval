#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include "ofMain.h"
#include <vector>
#include <time.h>
#include <sys/stat.h>


inline bool vector_finder(const std::vector<int>& vector, const int number)
{
	if (vector.size() > 0)
	{
		auto itr = std::find(vector.begin(), vector.end(), number);
		size_t index = std::distance(vector.begin(), itr);
		if (index != vector.size())
			return true;	// If the number exists in the vector.
		else
			return false;	// If the number does not exist in the vector.
	}
	else
		return false;
}

inline int vector_index_finder(const std::vector<int>& vector, const int number)
{
	if (vector.size() > 0)
	{
		auto itr = std::find(vector.begin(), vector.end(), number);
		size_t index = std::distance(vector.begin(), itr);
		if (index != vector.size())
			return index;
		else
			return -1;
	}
	else
		return -1;
}

inline void put_time(std::string& time_str)
{
    time_t     current;
    struct tm  *local;

    time(&current);
    local = localtime(&current);

    std::string year = ofToString(local->tm_year + 1900);
    std::string month = ofToString(local->tm_mon);
    std::string day = ofToString(local->tm_mday);
    std::string hour = ofToString(local->tm_hour);
    std::string min = ofToString(local->tm_min);
    std::string sec = ofToString(local->tm_sec);
    std::string delim = "-";

    time_str = year + delim + month + delim + day + delim + hour + delim + min + delim + sec;
}


#endif /* SRC_UTIL_H_ */
