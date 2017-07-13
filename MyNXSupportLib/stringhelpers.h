#ifndef MYNXSUPPORTLIB_STRING_H
#define MYNXSUPPORTLIB_STRING_H

#include <cstdarg>
#include <string>

namespace MyNXSupportLib {

std::string StringConcat(std::string a, int b);
std::string StringConcat(std::string a, int b, std::string c);
std::string vStringf(std::string fmt, va_list args);
std::string Stringf(std::string fmt, ...);

}

#endif
