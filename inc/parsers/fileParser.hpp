#pragma once

#include <string>

bool loadFile(std::string& out, const std::string& path);
bool loadBinaryFile(const unsigned char*& dataOut, size_t& sizeOut, const std::string& path);