#include "engine.hpp"
#include "scene.hpp"
#include <cstdio>

constexpr unsigned int EXIT_FAILURE{1};
constexpr unsigned int EXIT_SUCCESS{0};

constexpr int DEFAULT_WIDTH{1920};
constexpr int DEFAULT_HEIGHT{1080};

Engine::Engine(RenderMode renderMode) : window{nullptr}, models{nullptr}, mode{renderMode}, mvpLocation { 0 }, program{0}, height{DEFAULT_HEIGHT}, width{DEFAULT_WIDTH}, good{false} {
#ifndef NDEBUG
	fprintf(stderr, "initGL Start Time: %f\n", glfwGetTime());
#endif
	window = initGL(width, height);
	if (!window) {
		good = false;
		return;
	}
#ifndef NDEBUG
	fprintf(stderr, "initGL Complete Time: %f\n", glfwGetTime());

	fprintf(stderr, "OpenGL Info:\n");
	fprintf(stderr, "Vendor: {%s}\n", glGetString(GL_VENDOR));
	fprintf(stderr, "Renderer: {%s}\n", glGetString(GL_RENDERER));
	fprintf(stderr, "Version: {%s}\n", glGetString(GL_VERSION));
#endif



#ifndef NDEBUG
	fprintf(stderr, "initializeShaders Start Time: %f\n", glfwGetTime());
#endif
	program = initializeShaders();
	if (program == 0) {
		fprintf(stderr, "Failed to initialize shaders");
		cleanupWindow(window);
		good = false;
	}
#ifndef NDEBUG
	fprintf(stderr,"initializeShaders Complete Time: %f\n", glfwGetTime());
#endif



#ifndef NDEBUG
	fprintf(stderr, "glGetUniformLocation Start Time: %f\n", glfwGetTime());
#endif
	glUseProgram(program);
	mvpLocation = glGetUniformLocation(program, "MVP");
	if (mvpLocation == -1) {
		fprintf(stderr, "MVP uniform failed to load");
		cleanupWindow(window);
		good = false;
	}
#ifndef NDEBUG
	fprintf(stderr,"glGetUniformLocation Complete Time: %f\n", glfwGetTime());
#endif

	good = true;
}



Engine::~Engine() {
	if (program) glDeleteProgram(program);
	cleanupWindow(window);
}



void Engine::Run(const char* sceneName) {
	if (!good) {
		fprintf(stderr, "Engine Initialization Failed\n");
		return;
	}

#ifndef NDEBUG
	fprintf(stderr,"Create Scene Start Time: %f\n", glfwGetTime());
#endif
	Scene* scene = createSceneFromName(reinterpret_cast<const unsigned char*>(sceneName));
	if (!scene) {
		fprintf(stderr, "Scene not found: %s\n", sceneName);
		return;
	}

	models = new Model[scene->objectCount()];
	scene->Load(models);

#ifndef NDEBUG
	fprintf(stderr,"Create Scene Complete Time: %f\n", glfwGetTime());
#endif
	
	Renderer renderer(window, models, scene, mode, program, mvpLocation, width, height);
	glfwSetWindowUserPointer(window, &renderer);

	setCallbacks(window);
	glfwShowWindow(window); 

	while (!glfwWindowShouldClose(window)) {
		if (scene && scene->isLoaded()) {
			renderer.update();
			renderer.draw();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	
	delete scene;
}




