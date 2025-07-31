#include <glad/glad.h> 

#include "core/window.hpp"

#include <cstdio>

Window::Window(const unsigned int width, const unsigned int height, const char* title): width(width), height(height) {
#ifndef NDEBUG
	printf("[Window] Create start: %f\n", glfwGetTime());
#endif

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		fprintf(stderr, "[Window ERROR] Failed to create GLFW window\n");
		return;
	}

	glfwMakeContextCurrent(window);
	initGLAD();
	setupGLState();

#ifndef NDEBUG
	printf("[Window] Create finish: %f\n", glfwGetTime());
#endif
}

Window::~Window() {
	if (window) {
		glfwDestroyWindow(window);
		window = nullptr;
	}
}

void Window::pollEvents() const { 
	glfwPollEvents(); 
}
void Window::swapBuffers() const { 
	glfwSwapBuffers(window); 
}

bool Window::shouldClose() const { 
	return glfwWindowShouldClose(window); 
}
void Window::updateViewport(const unsigned int widthIn, const unsigned int heightIn) {
	width = widthIn;
	height = heightIn;
	glViewport(0, 0, width, height);
}

bool Window::initGLAD() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "[Window ERROR] Failed to initialize GLAD\n");
		return false;
	}
	
	return true;
}

void Window::setupGLState() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}