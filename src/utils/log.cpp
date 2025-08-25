#include "utils/log.hpp"
#include <cstdio>

bool error(const char* caller, const char* function, std::string msg) {
	fprintf(stderr, "[%s %s ERROR] %s\n", caller, function, msg.c_str());
	return false;
}
bool debugLog(const char* caller, const char* function, std::string msg, bool debugOnly){
	if(debugOnly) {
#ifndef NDEBUG
		printf("[%s %s LOG] %s\n", caller, function, msg.c_str());
#endif
	}
	else printf("[%s %s LOG] %s\n", caller, function, msg.c_str());
	return true;
}

