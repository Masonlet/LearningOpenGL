#include "shaderManager.hpp"
#include "init.hpp"

ShaderManager::Shader::Shader() : sourceIsMultiLine{false} {
	this->ID = 0;
	this->type = Shader::UNKNOWN;
	return;
}

ShaderManager::Shader::~Shader() {
	return;
}

std::string ShaderManager::Shader::getType() {
	switch ( this->type )
	{
	case Shader::VERTEX_SHADER:
		return "VERTEX_SHADER";
		break;
	case Shader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
		break;
	case Shader::UNKNOWN:
	default:
		return "UNKNOWN_SHADER_TYPE";
		break;
	}
	// Should never reach here...
	return "UNKNOWN_SHADER_TYPE";
}