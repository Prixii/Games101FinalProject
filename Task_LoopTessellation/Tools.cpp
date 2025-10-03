#include "Tools.h"
#include <stdarg.h>

bool StringStartsWith(const std::string& str, const std::string& prefix) {
if (str.size() < prefix.size()) {
		return false;
	}

	for (size_t i = 0; i < prefix.size(); i++) {
		if (str[i] != prefix[i]) {
			return false;
		}
	}
	return true;
}


void PrintWarn(const char *fmt, ...) {
  printf("\033[1;33m[Warning]:\033[0m ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void PrintErr(const char *fmt, ...) {
  printf("\033[1;31m[Error]:\033[0m ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void PrintInfo(const char *fmt, ...) {
  printf("\033[1;34m[Info]:\033[0m ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void PrintSuccess(const char *fmt, ...) {
  printf("\033[1;32m[Success]:\033[0m ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}
