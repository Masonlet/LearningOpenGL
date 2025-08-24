#include <glad/glad.h>
#include "scene/scene.hpp"
#include "math/constants.hpp"

Camera* Scene::getActiveCamera() {
	std::map<std::string, Camera>::iterator it = cameras.find(activeCam);
	if (it == cameras.end()) return nullptr;
	return &it->second;
}
void Scene::setActiveCamera(unsigned int camIndex) {
	if (cameras.empty()) return;
	if (camIndex >= cameras.size()) camIndex = cameras.size() - 1;
	std::map<std::string, Camera>::const_iterator cam = cameras.begin();
	std::advance(cam, camIndex);
	activeCam = cam->first;
}

Light* Scene::getLightByName(std::string name) {
	std::map<std::string, Light>& m = getLights();
	std::map<std::string, Light>::iterator it = m.find(name);
	if (it == m.end()) {
		debugLog("getLightByName", "Light " + name + " not found.");
		return nullptr; 
	}
	return &it->second;
}
void Scene::updateLights(int shaderProgram) {
	std::map<std::string, Light>& lights = getLights();

	unsigned int i{ 0 };
	for (std::map<std::string, Light>::iterator it = lights.begin(); it != lights.end() && i < NUMBEROFLIGHTS; ++it, ++i) {
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
void Scene::updateLightUniforms(int shaderProgram) {
	std::map<std::string, Light>& lights = getLights();

	for (std::pair<std::string, Light> it: lights) {
		Light& light = it.second;
		if (light.position_UL != -1)    glUniform4f(light.position_UL, light.pos.x, light.pos.y, light.pos.z, 1.0f);
		if (light.diffuse_UL != -1)     glUniform4f(light.diffuse_UL, light.diffuse.r, light.diffuse.g, light.diffuse.b, light.diffuse.a);
		if (light.attenuation_UL != -1) glUniform4f(light.attenuation_UL, light.attenuation.r, light.attenuation.g, light.attenuation.b, light.attenuation.a);
		if (light.direction_UL != -1)   glUniform4f(light.direction_UL, light.direction.r, light.direction.g, light.direction.b, { 1.0f });
		if (light.param1_UL != -1)      glUniform4f(light.param1_UL, static_cast<float>(light.type), light.param1.x, light.param1.y, 0.0f);
		if (light.param2_UL != -1)      glUniform4f(light.param2_UL, light.enabled, 0.0f, 0.0f, 0.0f);
	}
}

bool Scene::addTexture(const BMPTexture& data) {	
	if (data.name.empty())                    return error("Scene", "addTexture", "texture name is empty");
	if (data.index == 0)                      return error("Scene", "addTexture", "texture GL id is 0");
	if (data.slot >= ModelData::NUM_TEXTURES) return error("Scene", "addTexture", "texture slot " + std::to_string(data.slot) + " out of range");
	
	const std::string name = data.name;
 	if (textures.find(name) != textures.end()) return error("Scene", "addTexture", "texture file already exists: " + name);

	textures.emplace(name, data);
	return true;	
}

unsigned int Scene::getTextureIDFromName(const std::string& textureFileName) {
	std::map<std::string, BMPTexture>::iterator it = textures.find(textureFileName);
	return (it == textures.end()) ? 0 : static_cast<unsigned>(it->second.index);
}
bool Scene::bindTextureToModel(const std::string& modelName, unsigned int slot, const std::string& textureName, float mix) {
	if (slot >= ModelData::NUM_TEXTURES) return error("Scene", "bindTextureToModel", "slot out of range: " + std::to_string(slot));

	std::map<std::string, ModelData>::iterator  mIt = models.find(modelName);
	if (mIt == models.end()) return error("Scene", "bindTextureToModel", "model not found: " + modelName);

	ModelData& data = mIt->second;
	if (textureName.empty() || mix <= 0.0f) {
		data.textureNames[slot].clear();
		data.textureMixRatio[slot] = 0.0f;

		bool any = false;
		for (unsigned i = 0; i < ModelData::NUM_TEXTURES; ++i)
			if (!data.textureNames[i].empty()) { 
				any = true; 
				break; 
			}
		data.useTextures = any;

		debugLog("Scene", "unbind texture: " + modelName + "[slot " + std::to_string(slot) + "]", true);
		return true;
	}

	std::map<std::string, BMPTexture>::iterator tIt = textures.find(textureName);
	if (tIt == textures.end()) return error("Scene", "bindTextureToModel", "texture not found: " + textureName);

	if (mix < 0.0f) mix = 0.0f;
	if (mix > 1.0f) mix = 1.0f;

	data.useTextures = true;
	data.textureNames[slot] = textureName;
	data.textureMixRatio[slot] = mix;

	debugLog("Scene", "bind texture: " + textureName + " to " + modelName +" [slot " + std::to_string(slot) + "], mix=" + std::to_string(mix), true);
	return true;
}

ParsedMaze* Scene::getMazeFromName(const std::string& name) {
	std::map<std::string, ParsedMaze>::iterator it = mazes.find(name);
	return (it == mazes.end()) ? nullptr : &it->second;
}