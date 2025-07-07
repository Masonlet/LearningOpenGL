#include "utils/files.hpp"
#include "utils/parser.hpp"

#include <fstream>
#include <cstdio>

constexpr size_t MAX_SIZE = static_cast<size_t>(200 * 1024) * 1024; //200MB Limit

static bool getFileSize(FILE* file, size_t& sizeOut) {
	// Seek to end 
	if (fseek(file, 0, SEEK_END) != 0) {
		fprintf(stderr, "[getFileSize] Failed to seek end of binary file\n");
		return false;
	}

	// Get the file size
	long size = ftell(file);
	if (size < 0 || static_cast<size_t>(size) > MAX_SIZE) {
		fprintf(stderr, "[getFileSize] Invalid binary file, or it is too large\n");
		return false;
	}

	// Rewind to beginning
	rewind(file);
	sizeOut = static_cast<size_t>(size);
	return true;
}

bool loadFile(std::string& out, const std::string& path) {
	// Open File
	FILE* file = fopen(path.c_str(), "rb");
	if (file == NULL) {
		fprintf(stderr, "[loadFile] Failed to open file: %s\n", path.c_str());
		return false;
	}	

	size_t fileSize;
	if (!getFileSize(file, fileSize)) {
		fprintf(stderr, "[loadFile] Failed to get file size"); 
		fclose(file); 
		return false;
	}

	// Read file into buffer
	char* buffer = new char[fileSize + 1];
	size_t bytesRead = fread(buffer, 1, fileSize, file);
	fclose(file);

	if (bytesRead != fileSize) {
		fprintf(stderr, "[loadFile] fread failed. Expected %zu bytes, got %zu\n", fileSize, bytesRead);
		delete[] buffer;
		return false;
	}

	buffer[fileSize] = '\0'; 
	out.assign(buffer);   
	delete[] buffer;
	return true;
}

bool loadBinaryFile(unsigned char*& out, const std::string& path) {
	// Open File
	FILE* file = fopen(path.c_str(), "rb");
	if (!file) {
		fprintf(stderr, "[loadBinaryFile] Failed to open file: %s\n", path.c_str());
		return false;
	}

	size_t fileSize;
	if (!getFileSize(file, fileSize)) {
		fprintf(stderr, "[loadBinaryFile] Failed to get file size"); 
		fclose(file); 
		return false;
	}

	// Read file into buffer
	unsigned char* buffer = new unsigned char[fileSize];
	if (fread(buffer, 1, fileSize, file) != fileSize) {
		fprintf(stderr, "[loadBinaryFile] Failed to read binary data\n");
		delete[] buffer;
		fclose(file);
		out = nullptr;
		return false;
	}

	fclose(file);
	if (out) delete[] out;
	out = buffer;
	return true;
}
