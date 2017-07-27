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

vector<int> ReadIntData(ifstream& input) {
	string line;
	getline(input, line);

	/* create C-string to be used by strtok */
	char *cline = new char[line.length()+1];
	copy(line.begin(), line.end(), cline);
	cline[line.length()] = '\0';

	vector<int> data;

	char *cxt;
	char *tok = strtok_s(cline, ",", &cxt);
	while (tok != NULL) {
		/* try to convert x to string */
		int x;
		try {
			x = stoi(string(tok));
		} catch(invalid_argument ex) {
			x = 0;
		}

		/* add the new value to the result array */
		data.push_back(x);

		/* go to the next token */
		tok = strtok_s(NULL, ",", &cxt);
	}

	delete[] cline;
	return data;

}

}
