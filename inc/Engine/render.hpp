#pragma once

#include "init.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "scene.hpp"

struct Renderer {
	GLFWwindow* window;
	Camera camera;
	Model* models;
	Scene* scene;
	RenderMode mode;

	bool paused, wireframe, mouseLocked;
	unsigned int width, height, program, mvpLocation;
	float aspect, deltaTime, lastTime, lastYaw, lastPitch;

	Mat4 lastModel, lastMVP;
	Vec3 LastPOS;

	unsigned int currentProgram, currentVAO, currentModel, currentIndex{0};

	//models(nullptr)
	Renderer();
	Renderer(GLFWwindow* window, Model* models, Scene* scene, RenderMode& mode, unsigned int program, unsigned int mvpLocation, unsigned int width, unsigned int height);

	void update();
	void draw();

	void render();
	void updateAspect(unsigned int width, unsigned int height);
};





