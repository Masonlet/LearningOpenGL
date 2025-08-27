#include "controllers/lightController.hpp"
#include <glad/glad.h>

void LightController::updateLightLocations(std::map<std::string, Light>& lights, int shaderProgram) {
	size_t i{ 0 };
	for (std::map<std::string, Light>::iterator it = lights.begin(); it != lights.end() && i < lights.size(); ++it, ++i) {
		Light& light = it->second;
		std::string base = "theLights[" + std::to_string(i) + "].";
		light.position_UL    = glGetUniformLocation(shaderProgram, (base + "position").c_str());
		light.diffuse_UL     = glGetUniformLocation(shaderProgram, (base + "diffuse").c_str());
		light.attenuation_UL = glGetUniformLocation(shaderProgram, (base + "attenuation").c_str());
		light.direction_UL   = glGetUniformLocation(shaderProgram, (base + "direction").c_str());
		light.param1_UL      = glGetUniformLocation(shaderProgram, (base + "param1").c_str());
		light.param2_UL      = glGetUniformLocation(shaderProgram, (base + "param2").c_str());
	}
}
void LightController::updateLightUniforms(std::map<std::string, Light>& lights, int shaderProgram) {
	for (std::pair<std::string, Light> it : lights) {
		Light& light = it.second;
		if (light.position_UL != -1)    glUniform4f(light.position_UL, light.pos.x, light.pos.y, light.pos.z, 1.0f);
		if (light.diffuse_UL != -1)     glUniform4f(light.diffuse_UL, light.diffuse.r, light.diffuse.g, light.diffuse.b, light.diffuse.a);
		if (light.attenuation_UL != -1) glUniform4f(light.attenuation_UL, light.attenuation.r, light.attenuation.g, light.attenuation.b, light.attenuation.a);
		if (light.direction_UL != -1)   glUniform4f(light.direction_UL, light.direction.r, light.direction.g, light.direction.b, 1.0f);
		if (light.param1_UL != -1)      glUniform4f(light.param1_UL, static_cast<float>(light.type), light.param1.x, light.param1.y, 0.0f);
		if (light.param2_UL != -1)      glUniform4f(light.param2_UL, light.enabled, 0.0f, 0.0f, 0.0f);
	}
}
