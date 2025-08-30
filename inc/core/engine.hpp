#pragma once

#include "core/windowManager.hpp"
#include "core/renderer.hpp"
#include "graphics/shaderManager.hpp"
#include "graphics/meshManager.hpp"
#include "graphics/textureManager.hpp"
#include "core/inputManager.hpp"
#include "scene/sceneManager.hpp"

struct Engine {
	WindowManager windowManager;

	Renderer renderer;
	ShaderManager shaderManager;
	MeshManager meshManager;
	TextureManager textureManager;

	InputManager inputManager;
	SceneManager sceneManager;

	bool wireframe{ false };
	float deltaTime{ 0.0f }, lastTime{ 0.0f };

	bool initialize(const unsigned int width, const unsigned int height, const char* title);
	bool setScene(const std::string& sceneIn = "Default") { return sceneManager.loadTxtScene(sceneIn); }
	bool loadSceneAssets();

	void run();

private:
	bool setupShaders();
	bool loadSceneMeshes();
	bool loadSceneTextures();
	bool loadSceneTextureConnections();

	void updateTime(const float currentTime);
	void renderFrame();
};
