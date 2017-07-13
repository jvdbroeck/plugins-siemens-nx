#include "MyNXSupportLib.h"

#include <cstdarg>

using namespace std;

namespace MyNXSupportLib {
void debug(string fmt, ...) {
	va_list args;

	va_start(args, fmt);
	string str = vStringf(fmt, args);
	va_end(args);

	nx_listingwindow->WriteLine(str.c_str());
}
}
