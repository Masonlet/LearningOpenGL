#include <glad/glad.h>

#include "graphics/shaderManager.hpp"
#include "parsers/fileParser.hpp"

#include <sstream>

std::string ShaderManager::Shader::getType() {
	switch (this->type) {
	case Shader::VERTEX_SHADER:
		return "VERTEX_SHADER";
	case Shader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
	default:
		break;
	}

	return "UNKNOWN_SHADER_TYPE";
}

bool ShaderManager::useShaderProgram(unsigned int ID) {
	glUseProgram(ID); 
	return true;
}
bool ShaderManager::useShaderProgram(std::string friendlyName) {
	std::map<std::string, unsigned int>::iterator itShad = this->name_to_id.find(friendlyName);
	if (itShad == this->name_to_id.end()) return false;

	glUseProgram(itShad->second);
	return true;
}

int ShaderManager::getIDFromFriendlyName(std::string friendlyName) {
	std::map<std::string, unsigned int>::iterator itShad = this->name_to_id.find(friendlyName);
	if (itShad == this->name_to_id.end())	return 0;
	else																	return itShad->second;
}

ShaderManager::ShaderProgram* ShaderManager::getShaderProgramFromFriendlyName(std::string friendlyName) {
	std::map<unsigned int, ShaderProgram>::iterator itShad = this->id_to_shader.find(this->getIDFromFriendlyName(friendlyName));
	if (itShad == this->id_to_shader.end())	return nullptr;
	else																		return &(itShad->second);
}

const unsigned int MAXLINELENGTH = 65536; //16x1024
bool ShaderManager::loadSourceFromFile(Shader& shader) const {
	std::string path = std::string(ASSET_DIR) + "/shaders/" + shader.fileName;
  
	std::string src{};
	if (!loadFile(src, path)) {
		fprintf(stderr, "[loadSourceFromFile ERROR] Failed to load file: %s\n", path.c_str());
		return false;
	}

	shader.vecSource.clear();
	std::istringstream iss(src);
	std::string line;

	while (std::getline(iss, line)) shader.vecSource.push_back(line);
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
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);
		errorText.append(pLogText);
		this->lastError.append("\n");
		this->lastError.append( errorText );

		delete [] pLogText;	
		return true;	
	}
	return false; 
}
bool ShaderManager::wasThereALinkError(unsigned int programID, std::string& errorText) {
	errorText = "";	

	int wasError = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &wasError);
	if(wasError == GL_FALSE) {
		int maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		glGetProgramInfoLog(programID, maxLength, &maxLength, pLogText);
		errorText.append(pLogText);
		this->lastError.append("\n");
		this->lastError.append( errorText );

		delete [] pLogText;	
		return true;
	}
	return false;
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

	char** arraySource = new char*[numberOfLines];
	memset(arraySource, 0, numberOfLines);	

	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++) {
		unsigned int numCharacters = (unsigned int)shader.vecSource[indexLine].length();
		arraySource[indexLine] = new char[numCharacters + 2]; // For the '\n' and '\0' at end
		memset(arraySource[indexLine], 0, static_cast<size_t>(numCharacters + 2));

		for (unsigned int indexChar = 0; indexChar != shader.vecSource[indexLine].length(); indexChar++) 
			arraySource[indexLine][indexChar] = shader.vecSource[indexLine][indexChar];

		arraySource[indexLine][numCharacters + 0] = '\n';
		arraySource[indexLine][numCharacters + 1] = '\0';
	}

	glShaderSource(shader.ID, numberOfLines, arraySource, NULL);
	glCompileShader(shader.ID);

	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++) 
		delete[] arraySource[indexLine];
	delete[] arraySource;

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
	vertexShader.ID = glCreateShader(GL_VERTEX_SHADER);
	vertexShader.type = Shader::VERTEX_SHADER;
	if (!this->loadSourceFromFile(vertexShader)) return false;
	if (!this->compileShaderFromSource(vertexShader, errorText)) {
		this->lastError = errorText;
		return false;
	}

	errorText = "";
  fragShader.ID = glCreateShader(GL_FRAGMENT_SHADER);
	fragShader.type = Shader::FRAGMENT_SHADER;
	if (!this->loadSourceFromFile(fragShader)) return false;
	if (!this->compileShaderFromSource(fragShader, errorText)){
		this->lastError = errorText;
		return false;
	}

	ShaderProgram curProgram;
  curProgram.ID = glCreateProgram();
  glAttachShader(curProgram.ID, vertexShader.ID);
  glAttachShader(curProgram.ID, fragShader.ID);
  glLinkProgram(curProgram.ID);

	errorText = "";
	if (this->wasThereALinkError(curProgram.ID, errorText)) {
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->lastError = ssError.str();
		return false;
	}

	curProgram.friendlyName = friendlyName;
	this->id_to_shader[curProgram.ID] = curProgram;
	this->name_to_id[curProgram.friendlyName] = curProgram.ID;
	return true;
}
