#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include "Utility.h"


int Utility::Round(float v)
{
	return static_cast<int>(roundf(v));
}

std::vector<std::string> Utility::Split(std::string& line, char delimiter)
{
    std::istringstream stream(line);
    std::string field;
    std::vector<std::string> result;
    while (getline(stream, field, delimiter)) {
        result.push_back(field);
    }
    return result;
}