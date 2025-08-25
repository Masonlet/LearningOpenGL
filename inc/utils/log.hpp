#pragma once

#include <string>

bool error(const char* caller, const char* function, std::string msg);
bool debugLog(const char* caller, const char* function, std::string msg, bool debugOnly = false);