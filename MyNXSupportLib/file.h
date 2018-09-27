#ifndef MYNXSUPPORTLIB_FILE_H
#define MYNXSUPPORTLIB_FILE_H

#include <array>
#include <cstdarg>
#include <string>
#include <vector>
#include <fstream>

namespace MyNXSupportLib {

std::string FullFilePath(std::string filename);

std::vector<int> ReadIntData(std::ifstream& input);
std::vector<double> ReadFloatData(std::ifstream& input);
void ReadRawData(std::string filename, std::vector<double>& time, std::vector<double>& data);
void ReadListData(std::string filename, std::vector<int>& data);
void ReadListData(std::string filename, std::vector<std::string>& data);

/*
1:   time;data -\
2:   time;data  |-> 
...             |
X:   time;data -/
... (N times)
 */
template<int N>
void ReadRawZoneData(std::string filename, std::array<std::vector<double>, N>& time, std::array<std::vector<double>, N>& data, int n_per_zone /* X */) {
	std::ifstream input;
	input.open(filename);

	if (!input.is_open()) {
		debug("could not open raw data file: %s", filename.c_str());
	}
	else {
		/* read lines in groups */
		int group = 0;

		while (!input.eof()) {
			std::string line;
			std::getline(input, line);

			// skip empty lines
			if (line.empty())
				continue;

			std::string str_line = string(line);
			auto pos = str_line.find(';');

			time[group].push_back(stod(str_line.substr(0, pos)));
			data[group].push_back(stod(str_line.substr(pos+1)));
			if (time[group].size() == n_per_zone)
				group++;
		}
	}
}

}

#endif
