#include <glad/glad.h>

#include "scene/sceneManager.hpp"
#include "scene/sceneParser.hpp"

#include "utils/fileParser.hpp"
#include "utils/parser.hpp"
#include "utils/log.hpp"

#include "objects/grids.hpp"
#include "objects/primitives.hpp"

#include <fstream>
#include <iomanip>
#include <vector>
#include <cstring>

bool SceneManager::saveTxtScene() {
	const std::string scenePath = std::string(ASSET_DIR) + "/scenes/" + scene.getSceneName() + ".txt";
	std::ofstream file(scenePath);

	if (!file.is_open()) return error("SceneManager", "saveTxtScene", "Failed to open scene file for saving: " + scenePath);
	file << std::fixed << std::setprecision(3);

	file << "comment, name, pos(xyz), rot(yaw pitch), fov, nearPlane farPlane, camSpeed\n";
	const std::map<std::string, Camera>& cameras = scene.getCameras();
	for (std::map<std::string, Camera>::const_iterator camIt = cameras.begin(); camIt != cameras.end(); ++camIt) {
		const std::string& name = camIt->first;
		const Camera& cam = camIt->second;

		const std::string camType = (cam.type == 0) ? "FreeCam" :
			(cam.type == 1) ? "DungeonCam" :
			/*  .getType() == 2)*/ "ModernCam";

		file << "camera, "
			<< name << ", "
			<< camType << ", "
			<< cam.pos.x << " " << cam.pos.y << " " << cam.pos.z << ", "
			<< cam.yaw << " " << cam.pitch << ", "
			<< cam.fov << ", "
			<< cam.nearPlane << " " << cam.farPlane << ", "
			<< cam.moveSpeed;
		if (cam.type != 0) file << ", " << cam.moveDistance;
		file << '\n';
	}

	file << "\ncomment, name, meshPath, pos(xyz), rot(xyz), scale(xyz), colour(Int, Named Coloured, Random, Rainbow, PLY), specular(rgb, power)\n";
	const std::map<std::string, Model>& modelData = scene.getModels();
	for (const std::pair<const std::string, Model>& entry : modelData) {
		const std::string& name = entry.first;
		const Model& instance = entry.second;

		if (name.rfind("triangle_instance", 0) == 0 || name.rfind("cube_instance_", 0) == 0 || name.rfind("square_instance_", 0) == 0)
			continue;

		file << "model, " 
			<< name << ", "
			<< instance.meshPath << ", "
		  << instance.transform.pos.x << " " << instance.transform.pos.y << " " << instance.transform.pos.z << ", "
		  << instance.transform.rot.x << " " << instance.transform.rot.y << " " << instance.transform.rot.z << ", "
			<< instance.transform.size.x << " " << instance.transform.size.y << " " << instance.transform.size.z << ", ";

		switch (instance.colourMode) {
		case ColourMode::Solid: {
			int r = static_cast<int>(instance.colour.x * 255.0f);
			int g = static_cast<int>(instance.colour.y * 255.0f);
			int b = static_cast<int>(instance.colour.z * 255.0f);
			int a = static_cast<int>(instance.colour.w * 255.0f);

			if      (r == 255 && g == 0 && b == 0) file << "Red";
			else if (r == 0 && g == 255 && b == 0) file << "Green";
			else if (r == 0 && g == 0 && b == 255) file << "Blue";
			else                                   file << r << " " << g << " " << b << " " << a;
			break;
		}
		case ColourMode::Random:           file << "Random"; break;
		case ColourMode::VerticalGradient: file << "Rainbow"; break;
		case ColourMode::PLYColour:        file << "PLY"; break;
		default: break;
		}

		file << ", " << instance.specular.x << " " << instance.specular.y << " " << instance.specular.z << " " << instance.specular.w << "\n";
	}


	file << "\ncomment, name, type, pos (xyz), diffuse (rgba), attention (xyzw), direction, param1 (spotlight inner, spotlight outer), param2 (on/off)\n";
	std::map<std::string, Light>& lights = scene.getLights();
	for (std::map<std::string, Light>::iterator it = lights.begin(); it != lights.end(); ++it) {
		Light& light = it->second;
		if (!light.enabled) continue;

		const std::string camType = 
			(light.param1.x == 0) ? "Point" :
			(light.param1.x == 1) ? "Spot" :
			/*     param1.x == 2)*/ "Directional";

		file << "light, " 
			<< light.name << ", " 
			<< camType << ", "
			<< light.pos.x << " " << light.pos.y << " " << light.pos.z << ", "
			<< light.diffuse.x << " " << light.diffuse.y << " " << light.diffuse.z << " " << light.diffuse.w << ", "
			<< light.attenuation.x << " " << light.attenuation.y << " " << light.attenuation.z << " " << light.attenuation.w << ", "
			<< light.direction.x << " " << light.direction.y << " " << light.direction.z << ", "
			<< light.param1.y << " " << "\n";
	}

	return true;
}

bool SceneManager::loadTxtScene(const std::string& sceneIn) {
	debugLog("SceneManager", "loadTxtScene", "Start time: " + std::to_string(glfwGetTime()), true);

	std::string src{};
	if (!loadFile(src, std::string(ASSET_DIR) + "/scenes/" + sceneIn + ".txt"))
		return error("SceneManager", "loadTxtScene", "Failed to load scene file: " + sceneIn);

	const unsigned char* p = reinterpret_cast<const unsigned char*>(src.c_str());
	while (*p) {
		const unsigned char* nextLine = skipToNextLine(p);
		const unsigned char* endLine = trimEOL(p, nextLine);

		if (endLine <= p) {
			p = nextLine;
			continue;
		}

		if (!processSceneLine(p)) {
			const std::size_t maxLen = 256;
			std::size_t len = static_cast<std::size_t>(endLine - p);
			if (len > maxLen) len = maxLen;
			std::string errorMsg;
			errorMsg.reserve(len);
			errorMsg.append(reinterpret_cast<const char*>(p), len);
			return error("SceneManager", "loadTxtScene", std::string("Failed to process scene line: \"") + errorMsg + "\"");
		}

		if(p < nextLine) p = nextLine;
	}

	scene.setSceneName(sceneIn);
	return debugLog("SceneManager", "loadTxtScene", "Finish time:" + std::to_string(glfwGetTime()), true);;
}
bool SceneManager::processSceneLine(const unsigned char*& p) {
	if (!p || *p == '\0') return true;

	unsigned char token[64]{};
	parseToken(p, token, sizeof(token));
	
	const char* nameStr = reinterpret_cast<const char*>(token);
	if (!p || strlen(nameStr) == 0) return true;
	if (strcmp(nameStr, "comment") == 0 || nameStr[0] == '#') {
		p = skipToNextLine(p);
		return true;
	}

	bool handled{ false };
	if      (strcmp(nameStr, "model") == 0)       handled = parseAndAddObject<Model>(p, &parseModel, scene.getModels(), "model");
	else if (strcmp(nameStr, "light") == 0)       handled = parseAndAddObject<Light>(p, &parseLight, scene.getLights(), "light");
	else if (strcmp(nameStr, "camera") == 0)      handled = parseAndAddObject<Camera>(p, &parseCamera, scene.getCameras(), "camera");
	else if (strcmp(nameStr, "texture") == 0)     handled = parseAndAddTexture(p, &parseTexture, "texture");
	else if (strcmp(nameStr, "textureCube") == 0) handled = parseAndAddTexture(p, &parseCubeTexture, "cube texture");
	else if (strcmp(nameStr, "textureAdd") == 0)  handled = handleTextureConnectionLine(p);
	else if (strcmp(nameStr, "cubeGrid") == 0)    handled = parseAndAddObject<Grid>(p, &parseGrid, scene.getGrids(), "cubeGrid");
	else if (strcmp(nameStr, "squareGrid") == 0)  handled = parseAndAddObject<Grid>(p, &parseGrid, scene.getGrids(), "squareGrid");
	else if (strcmp(nameStr, "triangle") == 0)    handled = parseAndAddObject<Model>(p, &parseTriangle, scene.getModels(), "triangle");
	return handled ? true : error("SceneManager", "processSceneLine", "Failed to handle: " + std::string(nameStr));
}
bool SceneManager::parseAndAddTexture(const unsigned char*& p, bool (*parseFN)(const unsigned char*&, BMPTexture&), const char* type) {
	BMPTexture t;
	if (!parseFN(p, t)) return false;
	return scene.addTexture(t);
}

bool SceneManager::handleTextureConnectionLine(const unsigned char*& p) {
	std::string modelName, textureName;
	unsigned slot = 0;
	float mix = 1.0f;
	if (!parseTextureConnection(p, modelName, slot, textureName, mix)) return error("SceneManager", "handleTextureConnectionLine", "Failed to parse texture connection");
	return scene.bindTextureToModel(modelName, slot, textureName, mix) ? true : error("SceneManager", "handleTextureConnectionLine", "Failed to bind " + textureName + " to " + modelName);
}