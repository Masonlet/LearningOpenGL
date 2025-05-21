#include "callbacks.hpp"
#include "render.hpp"
#include <cstdio>

void error_callback(const int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	if (!renderer) return;
	
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);	
		
		if (key == GLFW_KEY_P) {
			renderer->wireframe = !renderer->wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, renderer->wireframe ? GL_LINE : GL_FILL);
		}

		if (key == GLFW_KEY_SPACE) renderer->paused = !renderer->paused;

		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
			renderer->mouseLocked = !renderer->mouseLocked;

			int currentMode = glfwGetInputMode(window, GLFW_CURSOR);
			glfwSetInputMode(window, GLFW_CURSOR, currentMode == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) renderer->currentIndex++;
	if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) renderer->currentIndex = 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	if (!renderer) return;
	
	glViewport(0, 0, width, height);

	renderer->updateAspect(width, height);
}

void setCallbacks(GLFWwindow* window) {
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}
