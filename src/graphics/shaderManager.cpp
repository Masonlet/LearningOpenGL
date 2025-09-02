#include <glad/glad.h>

#include "graphics/shaderManager.hpp"
#include "parsers/fileParser.hpp"
#include "utils/log.hpp"

#include <sstream>

ShaderManager::~ShaderManager() {
	for (std::map<std::string, Shader>::iterator it = nameToShaders.begin(); it != nameToShaders.end(); ++it) {
		if (it->second.programID  && glIsProgram(it->second.programID)) glDeleteProgram(it->second.programID);
		if (it->second.vertexID   && glIsShader(it->second.vertexID))   glDeleteShader(it->second.vertexID);
		if (it->second.fragmentID && glIsShader(it->second.fragmentID)) glDeleteShader(it->second.fragmentID);
	}
}

bool ShaderManager::useProgram(const std::string& name) const {
	std::map<std::string, Shader>::const_iterator it = nameToShaders.find(name);
	if (it == nameToShaders.end()) 
		return error("ShaderManager", "useProgram", "Shader not found: " + name);
	if (!it->second.linked || it->second.programID == 0) 
		return error("ShaderManager", "useProgram", "Shader not linked, or has invalid id: " + name);

	glUseProgram(it->second.programID);
	return true;
}

unsigned int ShaderManager::getProgramID(const std::string& name) const {
	std::map<std::string, Shader>::const_iterator it = nameToShaders.find(name);
	return (it == nameToShaders.end()) ? 0u : it->second.programID;
}

bool ShaderManager::createProgramFromPaths(const std::string& name, const std::string& vertPath, const std::string& fragPath) {
	std::string vertSource, fragSource;
	if (!loadFile(vertSource, std::string(ASSET_DIR) + "/shaders/" + vertPath)) return false;
	if (!loadFile(fragSource, std::string(ASSET_DIR) + "/shaders/" + fragPath)) return false;

	unsigned int vertID = 0;
	if (!compileShader(vertID, GL_VERTEX_SHADER, vertSource)) return false;
		
	unsigned int fragID = 0;
	if (!compileShader(fragID, GL_FRAGMENT_SHADER, fragSource)) {
		glDeleteShader(vertID);
		return false;
	}

	unsigned int programID = 0;
	if (!linkProgram(programID, vertID, fragID)) {
		glDeleteShader(vertID);
		glDeleteShader(fragID);
		return false;
	}

	std::map<std::string, Shader>::iterator it = nameToShaders.find(name);
	if (it != nameToShaders.end()) {
		if (it->second.programID)  glDeleteProgram(it->second.programID);
		if (it->second.vertexID)   glDeleteShader(it->second.vertexID);
		if (it->second.fragmentID) glDeleteShader(it->second.fragmentID);
	}

	Shader sh;
	sh.programID = programID;
	sh.vertexID = vertID;
	sh.fragmentID = fragID;
	sh.linked = true;

	nameToShaders[name] = std::move(sh);
	return true;
}


bool ShaderManager::findShader(const std::string& name) const {
	return nameToShaders.find(name) != nameToShaders.end();
}
bool ShaderManager::getShader(const std::string& name, Shader*& dataOut) {
	std::map<std::string, Shader>::iterator it = nameToShaders.find(name);
	if (it == nameToShaders.end()) return error("ShaderManager", "getShader", "Shader not found: " + name);
	dataOut = &it->second;
	return true;
}

bool ShaderManager::compileShader(unsigned int& outShaderID, int glShaderType, const std::string& source) {
	outShaderID = glCreateShader(static_cast<GLenum>(glShaderType));
	if (!outShaderID) return error("ShaderManager", "compileShader", "glCreateShader failed.");

	const char* src = source.c_str();
	glShaderSource(outShaderID, 1, &src, nullptr);
	glCompileShader(outShaderID);

	int status = GL_FALSE;
	glGetShaderiv(outShaderID, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		int logLen = 0;
		glGetShaderiv(outShaderID, GL_INFO_LOG_LENGTH, &logLen);

		std::string log;
		if (logLen > 1) {
			log.resize(static_cast<size_t>(logLen));
			int written = 0;
			glGetShaderInfoLog(outShaderID, logLen, &written, log.data());
			if (written > 0 && static_cast<size_t>(written) < log.size())
				log.resize(static_cast<size_t>(written));
		}

		glDeleteShader(outShaderID);
		outShaderID = 0;

		const std::string shaderType = (glShaderType == GL_VERTEX_SHADER   ? "VERTEX" 
			                            : glShaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" 
			                                                                 : "UNKNOWN");
		 
		return error("ShaderManager", "compileShader", "Shader compilation failed (type=" + shaderType + "):\n" + log);
	}

	return true;
}

bool ShaderManager::linkProgram(unsigned int& outProgramID, unsigned int vertID, unsigned int fragID){
	outProgramID = glCreateProgram();
	if (!outProgramID) return error("ShaderManager", "linkProgram", "glCreateProgram failed.");

	glAttachShader(outProgramID, vertID);
	glAttachShader(outProgramID, fragID);
	glLinkProgram(outProgramID);

	int status = GL_FALSE;
	glGetProgramiv(outProgramID, GL_LINK_STATUS, &status);

	glDetachShader(outProgramID, vertID);
	glDetachShader(outProgramID, fragID);

	if (status != GL_TRUE) {
		int logLen = 0;
		glGetProgramiv(outProgramID, GL_INFO_LOG_LENGTH, &logLen);

		std::string log;
		if (logLen > 1) {
			log.resize(static_cast<size_t>(logLen));
			int written = 0;
			glGetProgramInfoLog(outProgramID, logLen, &written, log.data());
			if (written > 0 && static_cast<size_t>(written) < log.size())
				log.resize(static_cast<size_t>(written));
		}

		glDeleteProgram(outProgramID);
		outProgramID = 0;
		return error("ShaderManager", "linkProgram", std::string("Program link failed:\n") + log);
	}

	return true;
}