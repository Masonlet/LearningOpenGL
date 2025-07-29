#include "lights/lightManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Light::Light() {
    this->position = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    this->diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->specular = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->atten = Vec4(0.0f, 0.01f, 0.01f, 1.0f);
    this->direction = Vec4(0.0f, 0.0f, 0.0f, 1.0f); // For spot lights
    this->param1.x = 0.0f;	// Point light 
    this->param1.y = 0.0f;	// Inner angle
    this->param1.z = 0.0f;	// outer angle
    this->param2.x = 0.0f;	// Off 
}

void LightManager::GetUniformLocations(int shaderProgram) {
  for (int i = 0; i < NUMBEROFLIGHTS; ++i) {
    std::string base = "theLights[" + std::to_string(i) + "].";

    this->theLights[i].position_UL = glGetUniformLocation(shaderProgram, (base + "position").c_str());
    this->theLights[i].diffuse_UL = glGetUniformLocation(shaderProgram, (base + "diffuse").c_str());
    this->theLights[i].specular_UL = glGetUniformLocation(shaderProgram, (base + "specular").c_str());
    this->theLights[i].atten_UL = glGetUniformLocation(shaderProgram, (base + "atten").c_str());
    this->theLights[i].direction_UL = glGetUniformLocation(shaderProgram, (base + "direction").c_str());
    this->theLights[i].param1_UL = glGetUniformLocation(shaderProgram, (base + "param1").c_str());
    this->theLights[i].param2_UL = glGetUniformLocation(shaderProgram, (base + "param2").c_str());
  }
}

void LightManager::UpdateShaderUniforms(int shaderProgram) const {
    for (unsigned int index = 0; index != LightManager::NUMBEROFLIGHTS; index++) {
        glUniform4f(this->theLights[index].position_UL, 
          this->theLights[index].position.x, this->theLights[index].position.y, this->theLights[index].position.z, this->theLights[index].position.w);
        glUniform4f(this->theLights[index].diffuse_UL, 
          this->theLights[index].diffuse.r, this->theLights[index].diffuse.g, this->theLights[index].diffuse.b, this->theLights[index].diffuse.a);
        glUniform4f(this->theLights[index].specular_UL,	
          this->theLights[index].specular.r, this->theLights[index].specular.g, this->theLights[index].specular.b, this->theLights[index].specular.a);
        glUniform4f(this->theLights[index].atten_UL, 
          this->theLights[index].atten.r, this->theLights[index].atten.g, this->theLights[index].atten.b, this->theLights[index].atten.a);
        glUniform4f(this->theLights[index].direction_UL,
            this->theLights[index].direction.r, this->theLights[index].direction.g, this->theLights[index].direction.b, this->theLights[index].direction.a);
        glUniform4f(this->theLights[index].param1_UL,
            this->theLights[index].param1.x, this->theLights[index].param1.y, this->theLights[index].param1.z, this->theLights[index].param1.w);
        glUniform4f(this->theLights[index].param2_UL,
            this->theLights[index].param2.x, this->theLights[index].param2.y, this->theLights[index].param2.z, this->theLights[index].param2.w);
    }
}

Light* LightManager::getLightByName(const std::string& name) {
  for (int i = 0; i < NUMBEROFLIGHTS; ++i) 
    if (lightNames[i] == name) 
      return &theLights[i];
    
  for (int i = 0; i < NUMBEROFLIGHTS; ++i) {
    if (lightNames[i].empty()) {
      lightNames[i] = name;
      return &theLights[i];
    }
  }

  return nullptr;
}
