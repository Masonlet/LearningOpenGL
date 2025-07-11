#include "graphics/shaderManager.hpp"

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

  return "UNKNOWN_SHADER_TYPE";
}
