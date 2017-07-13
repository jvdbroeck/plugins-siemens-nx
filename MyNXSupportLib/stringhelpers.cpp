#include "MyNXSupportLib.h"

#include <cstdarg>
#include <string>
#include <sstream>

using namespace std;

#define BUFFER_SIZE 1024

namespace MyNXSupportLib {

string StringConcat(string a, int b, string c) {
	stringstream ss;

	ss << a;
	ss << b;
	ss << c;

	return ss.str();
}

string StringConcat(string a, int b) {
	stringstream ss;

	ss << a;
	ss << b;

	return ss.str();
}

string vStringf(string fmt, va_list args) {
	char buf[BUFFER_SIZE];
	vsnprintf_s(buf, BUFFER_SIZE, fmt.c_str(), args);

	return string(buf);
}

string Stringf(string fmt, ...) {
	va_list args;
	va_start(args, fmt);
	string result = vStringf(fmt, args);
	va_end(args);

	return result;
}

}
