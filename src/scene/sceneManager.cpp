#include <glad/glad.h>

#include "scene/sceneManager.hpp"
#include "scene/sceneParser.hpp"

#include "utils/fileParser.hpp"
#include "utils/parser.hpp"
#include "utils/log.hpp"

#include "models/grids.hpp"
#include "models/primitives.hpp"

#include <fstream>
#include <iomanip>

SceneManager::SceneManager(MeshManager& meshManager, unsigned int& program, LightManager& lightManager, CameraManager& cameraManager, TextureManager& textureManager)
	: meshManager(meshManager), program(program), lightManager(lightManager), cameraManager(cameraManager), textureManager(textureManager), scene() {
}

bool SceneManager::loadTxtScene(const std::string& sceneIn) {
	debugLog("SceneManager", "Scene load start time: " + std::to_string(glfwGetTime()), true);

	const std::string scenepath = std::string(ASSET_DIR) + "/scenes/" + sceneIn + ".txt";
	std::string src{};
	if (!loadFile(src, scenepath)) return error("SceneManager", "loadTxtScene", "Failed to load scene file: " + scenepath);

	const unsigned char* p = reinterpret_cast<const unsigned char*>(src.c_str());
	while (*p) {
		const unsigned char* nextLine = skipToNextLine(p);
		const unsigned char* endLine = trimEOL(p, nextLine);

		if (endLine == p) {
			p = nextLine;
			continue;
		}

		if (!processSceneLine(p)) 
			return error("SceneManager", "loadTxtScene","Failed to process scene line: " + std::string(reinterpret_cast<const char*>(p), endLine - p));
	}

	scene.setSceneName(sceneIn);
	debugLog("SceneManager", "Scene load finish time:" + std::to_string(glfwGetTime()), true);
	return true;
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
	else if (strcmp(nameStr, "cubeGrid") == 0)    handled = handleCubeGridLine(p);
	else if (strcmp(nameStr, "squareGrid") == 0)  handled = handleSquareGridLine(p);
	else if (strcmp(nameStr, "triangle") == 0)    handled = handleTriangleLine(p);
	else if (strcmp(nameStr, "maze") == 0)        handled = handleMazeLine(p);
	else if (strcmp(nameStr, "mazeData") == 0)    handled = handleMazeData(p);
	return handled;
}

bool SceneManager::saveTxtScene() {
	const std::string scenePath = std::string(ASSET_DIR) + "/scenes/" + scene.getSceneName() + ".txt";
	std::ofstream file(scenePath);

	if (!file.is_open()) return error("SceneManager", "saveTxtScene", "Failed to open scene file for saving: " + scenePath);
	file << std::fixed << std::setprecision(3);

	file << "comment, name, pos(xyz), rot(yaw pitch), fov, nearPlane farPlane, camSpeed\n";
	const std::map<std::string, Camera>& cameras = cameraManager.getAllCameras();
	for (std::map<std::string, Camera>::const_iterator camIt = cameras.begin(); camIt != cameras.end(); ++camIt) {
		const std::string& name = camIt->first;
		const Camera& cam = camIt->second;
		const Vec3& pos = cam.getPos();

		const std::string camType = (cam.getType() == 0) ? "FreeCam" :
			(cam.getType() == 1) ? "DungeonCam" :
			/*  .getType() == 2)*/ "ModernCam";

		file << "camera, "
			<< name << ", "
			<< camType << ", "
			<< pos.x << " " << pos.y << " " << pos.z << ", "
			<< cam.getYaw() << " " << cam.getPitch() << ", "
			<< cam.getFov() << ", "
			<< cam.getNearPlane() << " " << cam.getFarPlane() << ", "
			<< cam.getMoveSpeed();
		if (cam.getType() != 0) file << ", " << cam.getMoveDistance();
		file << '\n';
	}

	file << "\ncomment, name, meshPath, pos(xyz), rot(xyz), scale(xyz), colour(Int, Named Coloured, Random, Rainbow, PLY), specular(rgb, power)\n";
	const std::map<std::string, ModelData>& modelData = scene.getModelData();
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
			<< instance.scale.x << " " << instance.scale.y << " " << instance.scale.z << ", ";

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

	file << "\ncomment, name, type, pos (xyz), diffuse (rgba), atten (xyzw), direction, param1 (spotlight inner, spotight outer), param2 (on/off)\n";
	for (int i = 0; i < LightManager::NUMBEROFLIGHTS; ++i) {
		const Light& light = lightManager.theLights[i];
		if (light.param2.x == 0.0f) continue;

		const std::string camType = 
			(light.param1.x == 0) ? "Point" :
			(light.param1.x == 1) ? "Spot" :
			/*     param1.x == 2)*/ "Directional";

		file << "light, " 
			<< lightManager.getLightName(i) << ", " 
			<< camType << ", "
			<< light.pos.x << " " << light.pos.y << " " << light.pos.z << ", "
			<< light.diffuse.x << " " << light.diffuse.y << " " << light.diffuse.z << " " << light.diffuse.w << ", "
			<< light.atten.x << " " << light.atten.y << " " << light.atten.z << " " << light.atten.w << ", "
			<< light.direction.x << " " << light.direction.y << " " << light.direction.z << " " << light.direction.w << ", "
			<< light.param1.y << " " << light.param1.z << " " << light.param1.w << ", "
			<< light.param2.x << " " << light.param2.y << " " << light.param2.z << " " << light.param2.w << "\n";
	}

	return true;
}

bool SceneManager::handleModelLine(const unsigned char*& p) {
	ParsedModel model{};
	PARSE_OR(return false, parseModel, model, "parse model");

	ModelData data;
	data.name = model.name;
	data.meshPath = model.path;
	data.pos = model.pos;
	data.rot = model.rot;
	data.scale = model.scale;
	data.modelMatrix = Mat4::modelMatrix({ {data.pos, 0.0 }, data.rot, data.scale });
	data.colour = model.colour;
	data.colourMode = model.colourMode;
	data.specular = model.specular;
	data.isVisible = model.isVisible;
	data.isLighted = model.isLighted;
	scene.addInstance(data);
	return true;
}
bool SceneManager::handleLightLine(const unsigned char*& p) {
	ParsedLight lightData{};
	PARSE_OR(return false, parseLight, lightData, "parse light");

	Light* light = lightManager.getLightByName(lightData.name);
	if (!light) return error("SceneManager", "handleLightLine", "Unable to store light: " + lightData.name);

	light->pos = { lightData.pos, 1.0 };
	light->diffuse = lightData.diffuse;
	light->atten = lightData.atten;
	light->direction = lightData.direction;
	light->param1 = { lightData.param1Type, lightData.param1Direction };
	light->param2 = { static_cast<float>(lightData.isEnabled), 0.0f, 0.0f, 0.0f };
	return true;
}
bool SceneManager::handleCameraLine(const unsigned char*& p) {
	ParsedCamera cameraData{};
	PARSE_OR(return false, parseCamera, cameraData, "parse camera");

	Camera cam;
	cam.setName(cameraData.name);
	cam.setYaw(cameraData.yaw);
	cam.setPitch(cameraData.pitch);
	cam.setPos(cameraData.pos);
	cam.setMoveSpeed(cameraData.speed);
	cam.setType(cameraData.type);
	cam.setFov(cameraData.fov);
	cam.setNear(cameraData.nearPlane);
	cam.setFar(cameraData.farPlane);
	if (cam.getType() != 0) cam.setMoveDistance(cameraData.moveDistance);

	if (!cameraManager.addCamera(cam)) return error("SceneManager", "handleCameraLine", "Failed to add camera: " + cameraData.name);
	return true;
}

bool SceneManager::handleTextureLine(const unsigned char*& p) {
	ParsedTexture t{};
	PARSE_OR(return false, parseTexture, t, "Failed to parse texture line");

	std::map<std::string, ModelData>& models = scene.getModelData();
	auto it = models.find(t.modelName);
	if (it == models.end()) return error("SceneManager", "handleTextureLine", "Texture refers to unknown model: " + t.modelName);

	if (t.textureNum >= ModelData::NUM_TEXTURES) 
		return error("SceneManager", "handleTextureLine", "Texture slot " + std::to_string(t.textureNum) + " out of range for " + t.modelName);

	ModelData& m = it->second;
	m.useTextures = true;
	m.textureNames[t.textureNum] = t.textureFile;
	m.textureMixRatio[t.textureNum] = t.mix;
	m.textureTiling = t.tiling;

	if (textureManager.getTextureIDFromName(t.textureFile) == 0) 
		if (!textureManager.Create2DBMPTexture(t.textureFile.c_str(), true)) 
			return error("SceneManager", "handleTextureLine", "Could not create 2D texture: " + t.textureFile);
	
	return true;
}
bool SceneManager::handleTextureCubeLine(const unsigned char*& p) {
	ParsedTextureCube t{};
	PARSE_OR(return false, parseTextureCube, t, "Failed to parse cube texture line");

	std::map<std::string, ModelData>& models = scene.getModelData();
	auto it = models.find(t.modelName);
	if (it == models.end()) return error("SceneManager", "handleTextureCubeLine", "Texture refers to unknown model: " + t.modelName);

	if (textureManager.getTextureIDFromName(t.modelName) == 0) 
		if (!textureManager.createCubeBMPTexture(t.modelName, t.textureFile1, t.textureFile2, t.textureFile3, t.textureFile4, t.textureFile5, t.textureFile6, true, true)) 
			return error("SceneManager", "handleTextureCubeLine", "Could not create cube texture: " + t.modelName);
		
	return true;
}

bool SceneManager::handleSquareGridLine(const unsigned char*& p) {
	ParsedGrid grid;
	PARSE_OR(return false, parseGrid, grid, "Failed to parse cubeGrid colour");

	if (!createSquareGrid(&meshManager, program, "cube", 0, grid.layout.count, { grid.layout.spacing, grid.layout.spacing }, grid.layout.rot, { grid.layout.scale.x, grid.layout.scale.y })) 
		return error("SceneManager", "handleSquareGridLine", "Failed to create squareGrid");

	std::map<std::string, ModelData>& modelData = scene.getModelData();
	std::map<std::string, ModelData>::iterator it = modelData.begin();
	for (it; it != modelData.end(); ++it) {
		const std::string& instanceName = it->first;
		ModelData& instance = it->second;

		if (instanceName.rfind("cube_instance_", 0) == 0) {
			instance.colour = grid.colour;
			instance.colourMode = grid.colourMode;
		}
	}

	return true;
}
bool SceneManager::handleCubeGridLine(const unsigned char*& p) {
	ParsedGrid grid;
	PARSE_OR(return false, parseGrid, grid, "Failed to parse cubeGrid colour");

	if (!createCubeGrid(&meshManager, program, "cube", 0, grid.layout.count, { grid.layout.spacing, grid.layout.spacing }, grid.layout.rot, grid.layout.scale)) 
		return error("SceneManager", "handleCubeGridLine", "Failed to create cubeGrid");

	std::map<std::string, ModelData>& modelData = scene.getModelData();
	std::map<std::string, ModelData>::iterator it = modelData.begin();
	for (it; it != modelData.end(); ++it) {
		const std::string& instanceName = it->first;
		ModelData& instance = it->second;

		if (instanceName.rfind("cube_instance_", 0) == 0) {
			instance.colour = grid.colour;
			instance.colourMode = grid.colourMode;
		}
	}

	return true;
}
bool SceneManager::handleTriangleLine(const unsigned char*& p) {
	ParsedTriangle triangle{};
	PARSE_OR(return false, parseTriangle, triangle, "Failed to parse triangle line");

	std::string sharedName;
	bool skipCache = false;
	if (triangle.colourMode == ColourMode::Solid) {
		int r = static_cast<int>(triangle.colour.x * 255.0f);
		int g = static_cast<int>(triangle.colour.y * 255.0f);
		int b = static_cast<int>(triangle.colour.z * 255.0f);
		sharedName = std::string(triangle.name) + "_solid_" + std::to_string(r) + "_" + std::to_string(g) + "_" + std::to_string(b);
		skipCache = true;
	}
	else sharedName = "triangle_shared";

	const MeshData* info{};
	bool meshExists = !skipCache && meshManager.findMesh(sharedName, info);

	if (!meshExists) {
		Vec4 bakedVertexColour = { triangle.colour.x, triangle.colour.y, triangle.colour.z, 1.0f };
		if (!createTriangle(&meshManager, sharedName, program, { triangle.transform.scale.x, triangle.transform.scale.y }, bakedVertexColour)) 
			return error("SceneManager", "handleTriangleLine", "Failed to create triangle mesh: " + sharedName);

		if (!meshManager.findMesh(sharedName, info)) 
			return error("SceneManager", "handleTriangleLine", "Mesh still not found after creation: " + sharedName);
	}

	std::string instanceName = std::string(triangle.name) + "_instance";
	ModelData data;
	data.name = instanceName;
	data.meshPath = sharedName;
	data.pos = { triangle.transform.pos.x, triangle.transform.pos.y, triangle.transform.pos.z };
	data.rot = triangle.transform.rot;
	data.scale = triangle.transform.scale;
	data.modelMatrix = Mat4::modelMatrix({ {data.pos, 0.0}, data.rot, data.scale });
	data.colour = triangle.colour;
	data.colourMode = triangle.colourMode;
	data.specular = Vec4{ 1.0f, 1.0f, 1.0f, 32.0f };
	data.isVisible = true;
	data.isLighted = true;
	data.useTextures = false;
	if (!scene.addInstance(data)) return error("SceneManager", "handleTriangleLine", "Failed to add triangle instance: " + instanceName);

	return true;
}

bool SceneManager::handleMazeLine(const unsigned char*& p) {
	ParsedMaze maze;
	PARSE_OR(return false, parseMaze, maze, "Failed to parse maze");

	pendingMaze = maze;
	return true;
}
bool SceneManager::handleMazeData(const unsigned char*& p) {
	if (!pendingMaze.has_value()) 
		return error("SceneManager", "handleMazeData", "No pending maze to apply mazeData to");

	if (!(parseMazeData(p, *pendingMaze))) 
		return error("SceneManager", "handleMazeData", "Failed to parse mazeData");

	if (!buildMaze(*pendingMaze)) 
		return error("SceneManager", "handleMazeData", "Failed to build maze from mazeData");

	pendingMaze.reset();
	return true;
}

static bool addFloor(Scene& scene, const std::string& name, const std::string& mesh, const Vec4& worldPos, const Vec3& rot) {
	ModelData d;
	d.name = name;
	d.meshPath = mesh;
	d.pos = Vec3{ worldPos.x, worldPos.y, worldPos.z };
	d.rot = rot;
	d.scale = Vec3{ 1.0f, 1.0f, 1.0f };
	d.modelMatrix = Mat4::modelMatrix({ {d.pos, 0.0}, d.rot, d.scale });
	d.colour = Vec4{ 1,1,1,1 };
	d.colourMode = ColourMode::Solid;
	d.specular = Vec4{ 1,1,1,32 };
	d.isVisible = true;
	d.isLighted = true;
	d.useTextures = false;
	if (!scene.addInstance(d)) return error("SceneLoader", "addFloor", ("Failed to add " + name).c_str());

	return true;
}
static bool addWall(Scene& scene, const ParsedMaze& maze,
	const Vec4& worldPos, const Vec4& wallOffset, const Vec3& baseRot,
	const std::string& defaultMesh, const std::string& mesh, const std::string& wallName,
	bool& hasEntrance, bool& hasExit, const bool edgeCondition = false) {
	std::string finalMesh = mesh;

	if (edgeCondition && (!hasEntrance || !hasExit)) {
		if (!hasEntrance) {
			hasEntrance = true;
			finalMesh = maze.entranceType;
		}
		else {
			hasExit = true;
			finalMesh = maze.exitType;
		}
	}

	const Vec4 pos = worldPos - wallOffset;
	const Vec3 rot = baseRot + maze.baseRot;

	for (unsigned int level = 0; level < maze.wallHeight; ++level) {
		Vec4 stackedPos = pos;
		stackedPos.y += static_cast<float>(level) * maze.spacing;

		const std::string instanceName = wallName + "_" + std::to_string(level);
		ModelData d;
		d.name = instanceName;
		d.meshPath = (level == 0) ? finalMesh : (edgeCondition ? maze.exteriorWallType : defaultMesh);
		d.pos = Vec3{ stackedPos.x, stackedPos.y, stackedPos.z };
		d.rot = rot;
		d.scale = Vec3{ 1.0f, 1.0f, 1.0f };
		d.modelMatrix = Mat4::modelMatrix({ {d.pos, 0.0}, d.rot, d.scale });
		d.colour = Vec4{ 1,1,1,1 };
		d.colourMode = ColourMode::Solid;
		d.specular = Vec4{ 1,1,1,32 };
		d.isVisible = true;
		d.isLighted = true;
		d.useTextures = false;

		if (!scene.addInstance(d)) return error("SceneLoader", "addWall", ("Failed to add " + instanceName).c_str());
	}

	return true;
}
bool SceneManager::buildMaze(const ParsedMaze& maze) {
	bool hasEntrance{ false }, hasExit{ false };
	const Mat4 mazeMatrix = Mat4::modelMatrix({ {maze.pos, 0.0}, maze.rot, {1.0f, 1.0f, 1.0f} });
	for (size_t row = 0; row < maze.layout.size(); ++row) {
		for (size_t col = 0; col < maze.layout[row].size(); ++col) {
			const std::string iteration = std::to_string(row) + "_" + std::to_string(col);
			const Vec4 localPos = { static_cast<float>(col) * maze.spacing, 0.0f, -static_cast<float>(row) * maze.spacing, 1.0f };
			const Vec4 worldPos = mazeMatrix * localPos;

			const Vec4 northPos = { 0.0f, 0.0f, 0.0f, 0.0f };
			const Vec4 southPos = { maze.spacing, 0.0f, maze.spacing, 0.0f };
			const Vec4 eastPos = { maze.spacing, 0.0f, 0.0f, 0.0f };
			const Vec4 westPos = { 0.0f, 0.0f, maze.spacing, 0.0f };

			const Vec3 northRot = { 0.0f, 0.0f, 0.0f };
			const Vec3 southRot = { 0.0f, 180.0f, 0.0f };
			const Vec3 eastRot = { 0.0f, 90.0f, 0.0f };
			const Vec3 westRot = { 0.0f, -90.0f, 0.0f };

			const std::string wallVariants[] = { maze.wallType1, maze.wallType2, maze.wallType3, maze.wallType4, maze.wallType5, maze.wallType6 };
			const std::string wallMesh = wallVariants[rand() % 6];

			const bool northEdgeCondition = (row == 0);
			const bool southEdgeCondition = (row + 1 >= maze.layout.size());
			const bool eastEdgeCondition = (col == 0);
			const bool westEdgeCondition = (col + 1 >= maze.layout[row].size());

			if (maze.layout[row][col]) {
				const bool northCondition = (northEdgeCondition || !maze.layout[row - 1][col]);
				const bool southCondition = (southEdgeCondition || !maze.layout[row + 1][col]);
				const bool eastCondition = (eastEdgeCondition || !maze.layout[row][col - 1]);
				const bool westCondition = (westEdgeCondition || !maze.layout[row][col + 1]);

				std::string finalMesh = wallMesh;
				bool wallExists{ false };
				if (northCondition)	wallExists |= addWall(scene, maze, worldPos, northPos, northRot, wallMesh, finalMesh, maze.mazeName + "_wall_" + iteration + "_N", hasEntrance, hasExit, northEdgeCondition);
				if (southCondition) wallExists |= addWall(scene, maze, worldPos, southPos, southRot, wallMesh, finalMesh, maze.mazeName + "_wall_" + iteration + "_S", hasEntrance, hasExit, southEdgeCondition);
				if (eastCondition)  wallExists |= addWall(scene, maze, worldPos, eastPos, eastRot, wallMesh, finalMesh, maze.mazeName + "_wall_" + iteration + "_E", hasEntrance, hasExit, eastEdgeCondition);
				if (westCondition)  wallExists |= addWall(scene, maze, worldPos, westPos, westRot, wallMesh, finalMesh, maze.mazeName + "_wall_" + iteration + "_W", hasEntrance, hasExit, westEdgeCondition);

				const std::string floorVariants[] = { maze.floorType1, maze.floorType2, maze.floorType3, maze.floorType4, maze.floorType5, maze.floorType6 };
				const std::string floorMesh = wallExists ? maze.floorWallType : floorVariants[rand() % 6];
				if (!addFloor(scene, maze.mazeName + "_floor_" + iteration, floorMesh, worldPos, maze.rot)) return false;

				if (maze.hasRoof) {
					const Vec3 flippedRot = maze.rot + Vec3{ 180.0f, 0.0f, 0.0f };
					Vec4 roofPos = worldPos;
					roofPos.y += static_cast<float>(maze.wallHeight) * maze.spacing;
					roofPos.z -= maze.spacing;

					if (!addFloor(scene, maze.mazeName + "_roof_" + iteration, maze.floorWallType, roofPos, flippedRot)) return false;
				}
			}

			bool unusedFlag{ true };
			if (northEdgeCondition) addWall(scene, maze, worldPos, northPos, northRot, wallMesh, maze.exteriorWallType, maze.mazeName + "_exteriorwall_" + iteration + "_N", unusedFlag, unusedFlag);
			if (southEdgeCondition) addWall(scene, maze, worldPos, southPos, southRot, wallMesh, maze.exteriorWallType, maze.mazeName + "_exteriorwall_" + iteration + "_S", unusedFlag, unusedFlag);
			if (eastEdgeCondition)  addWall(scene, maze, worldPos, eastPos, eastRot, wallMesh, maze.exteriorWallType, maze.mazeName + "_exteriorwall_" + iteration + "_E", unusedFlag, unusedFlag);
			if (westEdgeCondition)  addWall(scene, maze, worldPos, westPos, westRot, wallMesh, maze.exteriorWallType, maze.mazeName + "_exteriorwall_" + iteration + "_W", unusedFlag, unusedFlag);
		}
	}
	return true;
}