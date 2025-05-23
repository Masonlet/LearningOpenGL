#include "files.hpp"
#include <cstdio>

bool loadFile(unsigned char*& outBuffer, const char* path, size_t& sizeOut) {
	// Open File
	FILE* file;
	if (fopen_s(&file, path, "rb") != 0) {
		fprintf(stderr, "Failed to open file\n");
		return false;
	}

	// Seek to end and get size
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	constexpr size_t BUFFER_SIZE = static_cast<size_t>(100.0 * 1024 * 1024); //100mb
	if (fileSize == -1 || fileSize > BUFFER_SIZE) {
		fprintf(stderr, "Invalid file, or it is too large\n");
		fclose(file);
		return false;
	} 
	rewind(file);


	// Read file into buffer
	unsigned char* buffer = new unsigned char [fileSize + 1];
	if (fread(buffer, 1, fileSize, file) != fileSize) {
		fprintf(stderr, "File read failed\n");
		fclose(file);
		outBuffer = nullptr;
		return false;
	}

	fclose(file);
	buffer[fileSize] = '\0';
	outBuffer = buffer;
	sizeOut = fileSize;
	return true;
}

