#include "utils/log.hpp"
#include <cstdio>

bool error(const char* caller, const char* function, std::string msg) {
	fprintf(stderr, "[%s %s ERROR] %s\n", caller, function, msg.c_str());
	return false;
}
void debugLog(const char* caller, std::string msg, bool debugOnly){
	if(debugOnly) {
#ifndef NDEBUG
		fprintf(stderr, "[%s LOG] %s\n", caller, msg.c_str());
#endif
	}
	else fprintf(stderr, "[%s LOG] %s\n", caller, msg.c_str());
}

