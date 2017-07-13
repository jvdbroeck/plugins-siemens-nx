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

template<int N>
void ReadRawZoneData(std::string filename, std::array<std::vector<double>, N>& time, std::array<std::vector<double>, N>& data, int n_per_zone) {
	std::ifstream input;
	input.open(filename);

	if (!input.is_open()) {
		debug("could not open raw data file: %s", filename.c_str());
	}
	else {
		/* read lines in groups */
		int group = 0;

		char *cxt;
		while (!input.eof()) {
			std::string line;
			std::getline(input, line);

			// skip empty lines
			if (line.empty())
				continue;

			char *cline = new char[line.length()+1];
			strcpy_s(cline, line.length(), line.c_str());

			char *tok = strtok_s(cline, ";", &cxt);
			time[group].push_back(stod(string(tok)));

			tok = strtok_s(NULL, ";", &cxt);
			data[group].push_back(stod(string(tok)));

			if (time[group].size() == n_per_zone)
				group++;
		}
	}
}

}

#endif
