#include "core/callbacks.hpp"
#include "core/engine.hpp"

#include <stdio.h>

void error_callback(const int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	if (!engine) return;
	
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) 
			glfwSetWindowShouldClose(window, GLFW_TRUE);	
		
		if (key == GLFW_KEY_P) {
			engine->updateWireframe();
			glPolygonMode(GL_FRONT_AND_BACK, engine->getWireframe() ? GL_LINE : GL_FILL);
		}

		if (key == GLFW_KEY_C) {
			if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			else 
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

    if(key == GLFW_KEY_L){
			if (!engine->getSceneLoader().saveTxtScene())
				fprintf(stderr, "[KEY_CALLBACK] Failed to save scene\n");
			else {
#ifndef ndebug
				printf("[KEY_CALLBACK] Scene saved successfully!\n");
#endif
			}
    }

		if (key == GLFW_KEY_N) {
			if (mods & GLFW_MOD_SHIFT)
				engine->incrementModel();
			else
				engine->decrementModel();
		}

		if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
			int camIndex = key - GLFW_KEY_0;
			engine->getCameraManager()->setActiveCamera(camIndex);
#ifndef ndebug
			printf("[KEY_CALLBACK] Switched to camera %d\n", camIndex);
#endif
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	static int lastWidth = 0;
	static int lastHeight = 0;

	if (width == lastWidth && height == lastHeight)
		return;

	lastWidth = width;
	lastHeight = height;

	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	if (!engine) return;
	
#ifndef NDEBUG
	printf("[INFO] Framebuffer resized: %dx%d\n", width, height);
#endif

	if (width < 64) width = 64;
	if (height < 64) height = 64;

	engine->updateAspect(width, height);
	glViewport(0, 0, width, height);
}
