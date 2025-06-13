#include "files.hpp"
#include "parser.hpp"
#include <fstream>
#include <cstdio>

bool loadFile(std::string& out, const std::string& path) {
	constexpr size_t MAX_SIZE = static_cast<size_t>(100.0 * 1024 * 1024); //100mb

	// Open File
	FILE* file = fopen(path.c_str(), "r");
	if (!file) {
		fprintf(stderr, "Failed to open file: %s\n ", path.c_str());
		return false;
	}	

	// Seek to end 
	if (fseek(file, 0, SEEK_END) != 0) {
		fprintf(stderr, "Failed to seek end of file\n");
		fclose(file); 
		return false;
	}

	// Get the file size
	long fileSize = ftell(file);
	if (fileSize < 0 || static_cast<size_t>(fileSize) > MAX_SIZE) {
		fprintf(stderr, "Invalid file, or it is too large%s\n", path.c_str());
		fclose(file);
		return false;
	} 

	// Rewind to beginning
	rewind(file);

	// Read file into buffer
  out.resize(static_cast<size_t>(fileSize) + 1);
	if (fread(&out[0], 1, static_cast<size_t>(fileSize), file) != static_cast<size_t>(fileSize)) {
		fprintf(stderr, "File read failed: %s\n", path.c_str());
		out.clear();
		fclose(file);
		return false;
	}

  out[fileSize] = '\0'; 

	fclose(file);
	return true;
}
