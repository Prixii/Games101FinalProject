#include <string>
#pragma once
bool StringStartsWith(const std::string& str, const std::string& prefix);
void PrintWarn(const char* fmt, ...);
void PrintErr(const char* fmt, ...);
void PrintInfo(const char* fmt, ...);
void PrintSuccess(const char* fmt, ...);
