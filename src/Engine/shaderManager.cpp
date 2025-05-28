#include "shaderManager.hpp"
#include "init.hpp"
#include "files.hpp"
#include "parser.hpp"
#include <cstdio>
#include <sstream>

bool ShaderManager::useShaderProgram(unsigned int ID) {
	//Add lookup to see if we really have that ID
	glUseProgram(ID);
	return true;
}
bool ShaderManager::useShaderProgram(std::string friendlyName) {
	std::map<std::string, unsigned int>::iterator itShad = this->name_to_id.find(friendlyName);

	if (itShad == this->name_to_id.end()) {
		//Possibly set glUseProgram(0)..?
		return false;
	}
	glUseProgram(itShad->second);

	return true;
}

int ShaderManager::getIDFromFriendlyName(std::string friendlyName) {
	std::map<std::string, unsigned int>::iterator itShad = this->name_to_id.find(friendlyName);

	if (itShad == this->name_to_id.end()) {
		return 0;
	}

	return itShad->second;
}

ShaderManager::ShaderProgram* ShaderManager::getShaderProgramFromFriendlyName(std::string friendlyName) {
	unsigned int shaderID = this->getIDFromFriendlyName(friendlyName);

	std::map<unsigned int, ShaderProgram>::iterator itShad = this->id_to_shader.find(shaderID);

	if (itShad == this->id_to_shader.end()) {
		return nullptr;
	}

	ShaderProgram* shader = &(itShad->second);
	return shader;
}

const unsigned int MAXLINELENGTH = 65536; //16x1024
void ShaderManager::setBasePath(const std::string& path) {
	this->basePath = path;
}
ShaderManager::Shader ShaderManager::makeShader(const std::string& path) {
	ShaderManager::Shader shader;
	shader.fileName = path;
	return shader;
}

bool ShaderManager::loadSourceFromFile(Shader& shader) const {
	std::string path = this->basePath + shader.fileName;

	unsigned char* buffer{nullptr};
	size_t bufferSize{0};
	if (!loadFile(buffer, bufferSize, path)) {
		return false;
	}

	shader.vecSource.clear();
	std::string src(reinterpret_cast<char*>(buffer), bufferSize);
	std::istringstream iss(src);
	std::string line;

	while (std::getline(iss, line)) {
		shader.vecSource.push_back(line);
	}

	return true;
}

bool ShaderManager::wasThereACompileError(unsigned int shaderID, std::string& errorText) {
	errorText = "";

	int isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == 0) {
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->lastError.append("\n");
		this->lastError.append( errorText );

		delete [] pLogText;	// Oops
		return true;	// There WAS an error
	}

	return false; // There WASN'T an error
}
bool ShaderManager::wasThereALinkError(unsigned int programID, std::string& errorText) {
	errorText = "";	// No error

	int wasError = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &wasError);
	if(wasError == GL_FALSE) {
		int maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetProgramInfoLog(programID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->lastError.append("\n");
		this->lastError.append( errorText );

		delete [] pLogText;	
		return true; // There WAS an error
	}
	
	return false; // There WASN'T an error
}

std::string ShaderManager::getLastError() {
	std::string lastErrorTemp = this->lastError;
	this->lastError = "";
	return lastErrorTemp;
}

bool ShaderManager::compileShaderFromSource(ShaderManager::Shader& shader, std::string& error) {
	error = "";

	const unsigned int MAXLINESIZE = 8 * 1024;	// About 8K PER LINE, which seems excessive
	unsigned int numberOfLines = static_cast<unsigned int>(shader.vecSource.size());

	// This is an array of pointers to strings. aka the lines of source
	char** arraySource = new char*[numberOfLines];
	// Clear array to all zeros (i.e. '\0' or null terminator)
	memset( arraySource, 0, numberOfLines );	

	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++) {
		unsigned int numCharacters = (unsigned int)shader.vecSource[indexLine].length();
		// Create an array of chars for each line
		arraySource[indexLine] = new char[numCharacters + 2]; // For the '\n' and '\0' at end
		memset(arraySource[indexLine], 0, static_cast<size_t>(numCharacters + 2));

		// Copy line of source into array
		for (unsigned int indexChar = 0; indexChar != shader.vecSource[indexLine].length(); indexChar++) {
			arraySource[indexLine][indexChar] = shader.vecSource[indexLine][indexChar];
		}

		// At a '\0' at end (just in case)
		arraySource[indexLine][numCharacters + 0] = '\n';
		arraySource[indexLine][numCharacters + 1] = '\0';
	}

	glShaderSource(shader.ID, numberOfLines, arraySource, NULL);
	glCompileShader(shader.ID);

	// Get rid of the temp source "c" style array
	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++) {	// Delete this line
		delete[] arraySource[indexLine];
	}
	// And delete the original char** array
	delete[] arraySource;

	// Did it work? 
	std::string errorText = "";
	if (this->wasThereACompileError(shader.ID, errorText)) {
		std::stringstream ssError;
		ssError << shader.getType();
		ssError << " compile error: ";
		ssError << errorText;
		error = ssError.str();
		return false;
	}

	return true;
}

bool ShaderManager::createProgramFromFile(std::string friendlyName, Shader & vertexShader, Shader & fragShader) {
	std::string errorText = "";

	// Shader loading happening before vertex buffer array
	vertexShader.ID = glCreateShader(GL_VERTEX_SHADER);
	vertexShader.type = Shader::VERTEX_SHADER;
	
	// Load some text from a file...
	if ( ! this->loadSourceFromFile( vertexShader ) )
		return false;

	errorText = "";
	if ( ! this->compileShaderFromSource( vertexShader, errorText ) ){
		this->lastError = errorText;
		return false;
	}


    fragShader.ID = glCreateShader(GL_FRAGMENT_SHADER);
	fragShader.type = Shader::FRAGMENT_SHADER;
	if ( ! this->loadSourceFromFile( fragShader ) )
		return false;

	if ( ! this->compileShaderFromSource( fragShader, errorText ) ){
		this->lastError = errorText;
		return false;
	}

	ShaderProgram curProgram;
    curProgram.ID = glCreateProgram();

    glAttachShader(curProgram.ID, vertexShader.ID);
    glAttachShader(curProgram.ID, fragShader.ID);
    glLinkProgram(curProgram.ID);

	// Was there a link error? 
	errorText = "";
	if ( this->wasThereALinkError( curProgram.ID, errorText ) ){
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->lastError = ssError.str();
		return false;
	}

	// At this point, shaders are compiled and linked into a program

	curProgram.friendlyName = friendlyName;

	// Add the shader to the map
	this->id_to_shader[curProgram.ID] = curProgram;
	// Save to other map, too
	this->name_to_id[curProgram.friendlyName] = curProgram.ID;

	return true;
}