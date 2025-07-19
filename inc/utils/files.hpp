#pragma once

#include <string>

bool loadFile(std::string& out, const std::string& path);
bool loadBinaryFile(unsigned char*& out, const std::string& path);
