#pragma once

#include <string>

bool error(const char* caller, const char* function, std::string msg);
void debugLog(const char* caller, std::string msg, bool debugOnly = false);
