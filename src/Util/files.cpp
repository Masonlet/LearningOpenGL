#include "files.hpp"
#include "parser.hpp"
#include <fstream>

bool loadFile(unsigned char*& bufferOut, size_t& sizeOut, const std::string& path) {
	constexpr size_t MAX_SIZE = static_cast<size_t>(100.0 * 1024 * 1024); //100mb

	// Open File
	FILE* file {nullptr};
	if (fopen_s(&file, path.c_str(), "rb") != 0 || file == nullptr) {
		fprintf(stderr, "Failed to open file ");
		printf("%s\n", path.c_str());
		return false;
	}

	// Seek to end 
	if (fseek(file, 0, SEEK_END) != 0) {
		fprintf(stderr, "Failed to seek end of file\n");
		fclose(file); 
		return false;
	}

	// Get the file size
	size_t fileSize = ftell(file);
	if (fileSize < 0 || fileSize > MAX_SIZE) {
		fprintf(stderr, "Invalid file, or it is too large\n");
		fclose(file);
		return false;
	} 

	// Rewind to beginning
	rewind(file);

	// Read file into buffer
	unsigned char* buffer = new unsigned char [fileSize + 1];
	if (fread(buffer, 1, fileSize, file) != fileSize) {
		fprintf(stderr, "File read failed ");
		printf("%s\n", path.c_str());
		delete[] buffer;
		fclose(file);
		bufferOut = nullptr;
		return false;
	}

	//End buffer with a eof
	buffer[fileSize] = '\0';

	//Set out variables
	bufferOut = buffer;
	sizeOut = fileSize;

	fclose(file);
	return true;
}