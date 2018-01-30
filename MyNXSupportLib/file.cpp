#include "MyNXSupportLib.h"

#include <string>
#include <fstream>

using namespace NXOpen;
using namespace std;

#define BUFFER_SIZE 1024

namespace MyNXSupportLib {

string FullFilePath(string filename) {
	string path = string(nx_part->FullPath().GetText());
	path = path.substr(0, path.find_last_of("\\/")+1).c_str();

	return path + filename;
}

static
vector<string> ReadDataTokens(ifstream& input) {
	string line;
	getline(input, line);

	/* create C-string to be used by strtok */
	char *cline = new char[line.length()+1];
	copy(line.begin(), line.end(), cline);
	cline[line.length()] = '\0';

	vector<string> data;

	char *cxt;
	char *tok = strtok_s(cline, ",", &cxt);
	while (tok != NULL) {
		/* try to convert x to string */
		string x;
		try {
			x = string(tok);
		} catch(invalid_argument ex) {
			x = "0";
		}

		/* add the new value to the result array */
		data.push_back(x);

		/* go to the next token */
		tok = strtok_s(NULL, ",", &cxt);
	}

	delete[] cline;
	return data;
}

vector<int> ReadIntData(ifstream& input) {
	vector<int> result;
	for (auto s : ReadDataTokens(input))
		result.push_back(stoi(s));

	return result;
}

vector<double> ReadFloatData(ifstream& input) {
	vector<double> result;
	for (auto s : ReadDataTokens(input))
		result.push_back(stod(s));

	return result;
}

void ReadRawData(std::string filename, std::vector<double>& time, std::vector<double>& data) {
	array<vector<double>, 1> _time;
	array<vector<double>, 1> _data;
	ReadRawZoneData<1>(filename, _time, _data, 0);

	time = _time[0];
	data = _data[0];
}

void ReadListData(std::string filename, std::vector<std::string>& data) {
	ifstream input;
	input.open(filename);

	if (!input.is_open()) {
		debug("could not open raw data file: %s", filename.c_str());
	}
	else {
		while (!input.eof()) {
			std::string line;
			std::getline(input, line);

			// skip empty lines
			if (line.empty())
				continue;

			data.push_back(line);
		}
	}
}

void ReadListData(std::string filename, std::vector<int>& data) {
	vector<string> _data;
	ReadListData(filename, _data);

	for (string s : _data)
		data.push_back(stoi(s));
}

}
