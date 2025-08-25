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
	const std::map<std::string, ModelData>& modelData = scene.getModels();
	for (const std::pair<const std::string, ModelData>& entry : modelData) {
		const std::string& name = entry.first;
		const ModelData& instance = entry.second;

		if (name.rfind("triangle_instance", 0) == 0 || name.rfind("cube_instance_", 0) == 0 || name.rfind("square_instance_", 0) == 0 || name.rfind("maze_", 0) == 0)
			continue;

		file << "model, " 
			<< name << ", "
			<< instance.meshPath << ", "
		  << instance.pos.x << " " << instance.pos.y << " " << instance.pos.z << ", "
		  << instance.rot.x << " " << instance.rot.y << " " << instance.rot.z << ", "
			<< instance.size.x << " " << instance.size.y << " " << instance.size.z << ", ";

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
	for (std::pair<std::string, Light> it : lights) {
		Light& light = it.second;
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
	debugLog("SceneManager", "Scene load start time: " + std::to_string(glfwGetTime()), true);

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
	return debugLog("SceneManager", "Scene load finish time:" + std::to_string(glfwGetTime()), true);;
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
	if      (strcmp(nameStr, "model") == 0)       handled = handleModelLine(p);
	else if (strcmp(nameStr, "light") == 0)       handled = handleLightLine(p);
	else if (strcmp(nameStr, "camera") == 0)      handled = handleCameraLine(p);
	else if (strcmp(nameStr, "texture") == 0)     handled = handleTextureLine(p);
	else if (strcmp(nameStr, "textureCube") == 0) handled = handleTextureCubeLine(p);
	else if (strcmp(nameStr, "textureAdd") == 0)  handled = handleTextureConnectionLine(p);
	else if (strcmp(nameStr, "cubeGrid") == 0)    handled = handleCubeGridLine(p);
	else if (strcmp(nameStr, "squareGrid") == 0)  handled = handleSquareGridLine(p);
	else if (strcmp(nameStr, "triangle") == 0)    handled = handleTriangleLine(p);
	else if (strcmp(nameStr, "maze") == 0)        handled = handleMazeLine(p);
	else if (strcmp(nameStr, "mazeData") == 0)    handled = handleMazeData(p);
	return handled ? debugLog("SceneManager", "Added: " + std::string(nameStr), true) 
		             : error("SceneManager", "processSceneLine", "Failed to handle: " + std::string(nameStr));
}
bool SceneManager::handleModelLine(const unsigned char*& p) {
	ModelData model;
	PARSE_OR(return false, parseModel, model, "parse model");
	return scene.addObject(scene.getModels(), model, "model") ? true : error("SceneManager", "handleModelLine", "Unable to add model: " + model.name);
}
bool SceneManager::handleLightLine(const unsigned char*& p) {
	Light light;
	PARSE_OR(return false, parseLight, light, "parse light");
	return scene.addObject(scene.getLights(), light, "light") ? true : error("SceneManager", "handleLightLine", "Unable to add light: " + light.name);
}
bool SceneManager::handleCameraLine(const unsigned char*& p) {
	Camera camera;
	PARSE_OR(return false, parseCamera, camera, "parse camera");
	return scene.addObject(scene.getCameras(), camera, "camera") ? true : error("SceneManager", "handleCameraLine", "Unable to add camera: " + camera.name);
}
bool SceneManager::handleTextureLine(const unsigned char*& p) {
	BMPTexture texture;
	PARSE_OR(return false, parseTexture, texture, "texture line");
	return scene.addTexture(texture) ? true : error("SceneManager", "handleTextureLine", "Unable to add texture: " + texture.name);
}	
bool SceneManager::handleTextureCubeLine(const unsigned char*& p) {
	BMPTexture texture;
	PARSE_OR(return false, parseCubeTexture, texture, "Failed to parse cube texture line");
	return scene.addTexture(texture) ? true : error("SceneManager", "handleTextureLine", "Could not create cube texture");
}
bool SceneManager::handleTextureConnectionLine(const unsigned char*& p) {
	std::string modelName, textureName;
	unsigned slot = 0;
	float mix = 1.0f;
	if (!parseTextureConnection(p, modelName, slot, textureName, mix)) return error("SceneManager", "handleTextureConnectionLine", "Failed to parse texture connection");
	return scene.bindTextureToModel(modelName, slot, textureName, mix) ? true : error("SceneManager", "handleTextureConnectionLine", "Failed to bind " + textureName + " to " + modelName);
}
bool SceneManager::handleSquareGridLine(const unsigned char*& p) {
	Grid grid;
	PARSE_OR(return false, parseGrid, grid, "Failed to parse cubeGrid colour");
	return scene.addObject(scene.getGrids(), grid, "squareGrid") ? true : error("SceneManager", "handleSquareGridLine", "Could not create grid");
}
bool SceneManager::handleCubeGridLine(const unsigned char*& p) {
	Grid grid;
	PARSE_OR(return false, parseGrid, grid, "Failed to parse cubeGrid colour");
	return scene.addObject(scene.getGrids(), grid, "cubeGrid") ? true : error("SceneManager", "handleCubeGridLine", "Could not create grid");
}
bool SceneManager::handleTriangleLine(const unsigned char*& p) {
	Triangle triangle{};
	PARSE_OR(return false, parseTriangle, triangle, "Failed to parse triangle line");
	return scene.addObject(scene.getTriangles(), triangle, "triangle") ? true : error("SceneManager", "handleTriangleLine", "Failed to add triangle instance: " + triangle.name);
}

bool SceneManager::handleMazeLine(const unsigned char*& p) {
	ParsedMaze maze;
	PARSE_OR(return false, parseMaze, maze, "Failed to parse maze");
	return scene.addObject(scene.getMaze(), maze, "maze");
}
bool SceneManager::handleMazeData(const unsigned char*& p) {
	std::string mazeName;
	std::vector<std::vector<bool>> pendingMazeData;
	if (!(parseMazeData(p, mazeName, pendingMazeData)))	return error("SceneManager", "handleMazeData", "Failed to parse mazeData");

	ParsedMaze* maze = scene.getMazeFromName(mazeName);
	if (!maze) return error("SceneManager", "handleMazeData", "Failed to find maze to add data to");
	else maze->layout = pendingMazeData;

	if (!buildMaze(maze)) return error("SceneManager", "handleMazeData", "Failed to build maze from mazeData");
	return true;
}

static bool addFloor(Scene& scene, const std::string& name, const std::string& mesh, const Vec4& worldPos, const Vec3& rot) {
	ModelData d;
	d.name = name;
	d.meshPath = mesh;
	d.pos = Vec3{ worldPos.x, worldPos.y, worldPos.z };
	d.rot = rot;
	d.size = Vec3{ 1.0f, 1.0f, 1.0f };
	d.modelMatrix = Mat4::modelMatrix({ {d.pos, 0.0}, d.rot, d.size });
	d.colour = Vec4{ 1,1,1,1 };
	d.colourMode = ColourMode::Solid;
	d.specular = Vec4{ 1,1,1,32 };
	d.isVisible = true;
	d.isLighted = true;
	d.useTextures = false;
	if (!scene.addObject(scene.getModels(), d, "model")) return error("SceneLoader", "addFloor", ("Failed to add " + name).c_str());
	return true;
}
static bool addWall(Scene& scene, const ParsedMaze* maze,
	const Vec4& worldPos, const Vec4& wallOffset, const Vec3& baseRot,
	const std::string& defaultMesh, const std::string& mesh, const std::string& wallName,
	bool& hasEntrance, bool& hasExit, const bool edgeCondition = false) {
	std::string finalMesh = mesh;

	if (edgeCondition && (!hasEntrance || !hasExit)) {
		if (!hasEntrance) {
			hasEntrance = true;
			finalMesh = maze->entranceType;
		}
		else {
			hasExit = true;
			finalMesh = maze->exitType;
		}
	}

	const Vec4 pos = worldPos - wallOffset;
	const Vec3 rot = baseRot + maze->baseRot;

	for (unsigned int level = 0; level < maze->wallHeight; ++level) {
		Vec4 stackedPos = pos;
		stackedPos.y += static_cast<float>(level) * maze->spacing;

		const std::string instanceName = wallName + "_" + std::to_string(level);
		ModelData d;
		d.name = instanceName;
		d.meshPath = (level == 0) ? finalMesh : (edgeCondition ? maze->exteriorWallType : defaultMesh);
		d.pos = Vec3{ stackedPos.x, stackedPos.y, stackedPos.z };
		d.rot = rot;
		d.size = Vec3{ 1.0f, 1.0f, 1.0f };
		d.modelMatrix = Mat4::modelMatrix({ {d.pos, 0.0}, d.rot, d.size });
		d.colour = Vec4{ 1,1,1,1 };
		d.colourMode = ColourMode::Solid;
		d.specular = Vec4{ 1,1,1,32 };
		d.isVisible = true;
		d.isLighted = true;
		d.useTextures = false;

		if (!scene.addObject(scene.getModels(), d, "model")) return error("SceneLoader", "addWall", ("Failed to add " + instanceName).c_str());
	}

	return true;
}
bool SceneManager::buildMaze(const ParsedMaze* maze) {
	bool hasEntrance{ false }, hasExit{ false };
	const Mat4 mazeMatrix = Mat4::modelMatrix({ {maze->pos, 0.0}, maze->rot, {1.0f, 1.0f, 1.0f} });
	for (size_t row = 0; row < maze->layout.size(); ++row) {
		for (size_t col = 0; col < maze->layout[row].size(); ++col) {
			const std::string iteration = std::to_string(row) + "_" + std::to_string(col);
			const Vec4 localPos = { static_cast<float>(col) * maze->spacing, 0.0f, -static_cast<float>(row) * maze->spacing, 1.0f };
			const Vec4 worldPos = mazeMatrix * localPos;

			const Vec4 northPos = { 0.0f, 0.0f, 0.0f, 0.0f };
			const Vec4 southPos = { maze->spacing, 0.0f, maze->spacing, 0.0f };
			const Vec4 eastPos = { maze->spacing, 0.0f, 0.0f, 0.0f };
			const Vec4 westPos = { 0.0f, 0.0f, maze->spacing, 0.0f };

			const Vec3 northRot = { 0.0f, 0.0f, 0.0f };
			const Vec3 southRot = { 0.0f, 180.0f, 0.0f };
			const Vec3 eastRot = { 0.0f, 90.0f, 0.0f };
			const Vec3 westRot = { 0.0f, -90.0f, 0.0f };

			const std::string wallVariants[] = { maze->wallType1, maze->wallType2, maze->wallType3, maze->wallType4, maze->wallType5, maze->wallType6 };
			const std::string wallMesh = wallVariants[rand() % 6];

			const bool northEdgeCondition = (row == 0);
			const bool southEdgeCondition = (row + 1 >= maze->layout.size());
			const bool eastEdgeCondition = (col == 0);
			const bool westEdgeCondition = (col + 1 >= maze->layout[row].size());

			if (maze->layout[row][col]) {
				const bool northCondition = (northEdgeCondition || !maze->layout[row - 1][col]);
				const bool southCondition = (southEdgeCondition || !maze->layout[row + 1][col]);
				const bool eastCondition = (eastEdgeCondition || !maze->layout[row][col - 1]);
				const bool westCondition = (westEdgeCondition || !maze->layout[row][col + 1]);

				std::string finalMesh = wallMesh;
				bool wallExists{ false };
				if (northCondition)	wallExists |= addWall(scene, maze, worldPos, northPos, northRot, wallMesh, finalMesh, maze->name + "_wall_" + iteration + "_N", hasEntrance, hasExit, northEdgeCondition);
				if (southCondition) wallExists |= addWall(scene, maze, worldPos, southPos, southRot, wallMesh, finalMesh, maze->name + "_wall_" + iteration + "_S", hasEntrance, hasExit, southEdgeCondition);
				if (eastCondition)  wallExists |= addWall(scene, maze, worldPos, eastPos, eastRot, wallMesh, finalMesh, maze->name + "_wall_" + iteration + "_E", hasEntrance, hasExit, eastEdgeCondition);
				if (westCondition)  wallExists |= addWall(scene, maze, worldPos, westPos, westRot, wallMesh, finalMesh, maze->name + "_wall_" + iteration + "_W", hasEntrance, hasExit, westEdgeCondition);

				const std::string floorVariants[] = { maze->floorType1, maze->floorType2, maze->floorType3, maze->floorType4, maze->floorType5, maze->floorType6 };
				const std::string floorMesh = wallExists ? maze->floorWallType : floorVariants[rand() % 6];
				if (!addFloor(scene, maze->name + "_floor_" + iteration, floorMesh, worldPos, maze->rot)) return false;

				if (maze->hasRoof) {
					const Vec3 flippedRot = maze->rot + Vec3{ 180.0f, 0.0f, 0.0f };
					Vec4 roofPos = worldPos;
					roofPos.y += static_cast<float>(maze->wallHeight) * maze->spacing;
					roofPos.z -= maze->spacing;

					if (!addFloor(scene, maze->name + "_roof_" + iteration, maze->floorWallType, roofPos, flippedRot)) return false;
				}
			}

			bool unusedFlag{ true };
			if (northEdgeCondition) addWall(scene, maze, worldPos, northPos, northRot, wallMesh, maze->exteriorWallType, maze->name + "_exterior_wall_" + iteration + "_N", unusedFlag, unusedFlag);
			if (southEdgeCondition) addWall(scene, maze, worldPos, southPos, southRot, wallMesh, maze->exteriorWallType, maze->name + "_exterior_wall_" + iteration + "_S", unusedFlag, unusedFlag);
			if (eastEdgeCondition)  addWall(scene, maze, worldPos, eastPos, eastRot, wallMesh, maze->exteriorWallType, maze->name + "_exterior_wall_" + iteration + "_E", unusedFlag, unusedFlag);
			if (westEdgeCondition)  addWall(scene, maze, worldPos, westPos, westRot, wallMesh, maze->exteriorWallType, maze->name + "_exterior_wall_" + iteration + "_W", unusedFlag, unusedFlag);
		}
	}
	return true;
}