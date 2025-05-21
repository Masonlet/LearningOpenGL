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

	unsigned int height, width, renderType;
	unsigned int program, mvpLocation;

	bool good;

public:
	Engine();
	~Engine();

	void Run(const char* sceneName = "Default");
};