#pragma once

#include "files.hpp"
#include "shaders.hpp"
#include "render.hpp"
#include "callbacks.hpp"
#include "generator.hpp"

class Engine {
	GLFWwindow* window;
	Camera camera;
	Model* models;
	RenderMode mode;

	unsigned int height, width;
	unsigned int program, mvpLocation;
	bool good;

public:
	Engine(RenderMode renderMode = RenderMode::basic3D);
	~Engine();

	void Run(const char* sceneName = "Default");
};