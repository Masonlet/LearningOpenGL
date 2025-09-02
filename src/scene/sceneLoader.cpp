#include "scene/scene.hpp"
#include "scene/sceneLoader.hpp"
#include "starletparsers/scene/modelParser.hpp"
#include "starletparsers/scene/lightParser.hpp"
#include "starletparsers/scene/cameraParser.hpp"
#include "starletparsers/scene/gridParser.hpp"
#include "starletparsers/scene/textureParser.hpp"
#include "starletparsers/scene/primitiveParser.hpp"
#include "starletparsers/common/fileParser.hpp"
#include "starletparsers/common/parserUtils.hpp"
#include "starletparsers/utils/log.hpp"
#include <string>
#include <fstream>
#include <iomanip>

bool SceneLoader::loadScene(Scene& scene, const char* name) {
	std::string src{};
	if (!loadFile(src, std::string(ASSET_DIR) + "/scenes/" + name + ".txt"))
		return error("SceneLoader", "loadScene", "Failed to load scene file: " + std::string(name));
		
	const unsigned char* p = reinterpret_cast<const unsigned char*>(src.c_str());
	while (*p) {
		const unsigned char* nextLine = skipToNextLine(p);
		const unsigned char* endLine = trimEOL(p, nextLine);

		if (endLine <= p) {
			p = nextLine;
			continue;
		}

		if (!processSceneLine(scene, p)) {
			const std::size_t maxLen = 256;
			std::size_t len = static_cast<std::size_t>(endLine - p);
			if (len > maxLen) len = maxLen;
			std::string errorMsg;
			errorMsg.reserve(len);
			errorMsg.append(reinterpret_cast<const char*>(p), len);
			return error("SceneManager", "loadTxtScene", std::string("Failed to process scene line: \"") + errorMsg + "\"");
		}

		if (p < nextLine) p = nextLine;
	}
	return true;
}

bool SceneLoader::processSceneLine(Scene& scene, const unsigned char*& p) {
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
	if      (strcmp(nameStr, "model") == 0)       handled = parseAndAddObject<Model>(scene, p, &parseModel, "model");
	else if (strcmp(nameStr, "light") == 0)       handled = parseAndAddObject<Light>(scene, p, &parseLight, "light");
	else if (strcmp(nameStr, "camera") == 0)      handled = parseAndAddObject<Camera>(scene, p, &parseCamera, "camera");
	else if (strcmp(nameStr, "cubeGrid") == 0)    handled = parseAndAddObject<Grid>(scene, p, &parseGrid, "cubeGrid");
	else if (strcmp(nameStr, "squareGrid") == 0)  handled = parseAndAddObject<Grid>(scene, p, &parseGrid, "squareGrid");
	else if (strcmp(nameStr, "triangle") == 0)    handled = parseAndAddObject<Primitive>(scene, p, &parseTriangle, "triangle");
	else if (strcmp(nameStr, "texture") == 0)     handled = parseAndAddObject<TextureData>(scene, p, &parseTexture, "texture");
	else if (strcmp(nameStr, "textureCube") == 0) handled = parseAndAddObject<TextureData>(scene, p, &parseCubeTexture, "cube texture");
	else if (strcmp(nameStr, "textureAdd") == 0)  handled = parseAndAddObject<TextureConnection>(scene, p, &parseTextureConnection, "texture connection");
	return handled ? true : error("SceneManager", "processSceneLine", "Failed to handle: " + std::string(nameStr));
}

bool SceneLoader::saveScene(const Scene& scene) {
	const std::string scenePath = std::string(ASSET_DIR) + "/scenes/" + scene.getSceneName() + ".txt";
	std::ofstream file(scenePath);

	if (!file.is_open()) return error("SceneManager", "saveTxtScene", "Failed to open scene file for saving: " + scenePath);
	file << std::fixed << std::setprecision(3);

	file << "comment, name, pos(xyz), rot(yaw pitch), fov, nearPlane farPlane, camSpeed\n";
	const std::map<std::string, Camera>& cameras = scene.getObjects<Camera>();
	for (std::map<std::string, Camera>::const_iterator camIt = cameras.begin(); camIt != cameras.end(); ++camIt) {
		const std::string& name = camIt->first;
		const Camera& cam = camIt->second;

		file << "camera, "
			<< name << ", "
			<< cam.pos.x << " " << cam.pos.y << " " << cam.pos.z << ", "
			<< cam.yaw << " " << cam.pitch << ", "
			<< cam.fov << ", "
			<< cam.nearPlane << " " << cam.farPlane << ", "
			<< cam.moveSpeed;
		file << '\n';
	}

	file << "\ncomment, name, meshPath, pos(xyz), rot(xyz), scale(xyz), colour(Int, Named Coloured, Random, Rainbow, PLY), specular(rgb, power)\n";
	const std::map<std::string, Model>& modelData = scene.getObjects<Model>();
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

			if (r == 255 && g == 0 && b == 0) file << "Red";
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
	const std::map<std::string, Light>& lights = scene.getObjects<Light>();
	for (std::map<std::string, Light>::const_iterator it = lights.begin(); it != lights.end(); ++it) {
		const Light& light = it->second;
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