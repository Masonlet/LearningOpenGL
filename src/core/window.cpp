#include <glad/glad.h> 
#include "core/window.hpp"
#include "utils/log.hpp"
#include <cstdio>

Window::Window(const unsigned int width, const unsigned int height, const char* title): width(width), height(height) {
	debugLog("Window", "Create start time: " + std::to_string(glfwGetTime()), true);

	if (!createWindow(width, height, title)) return;
	if (!initGLAD()) return;

	setupGLState();

	debugLog("OpenGL", "OpenGL Info", true);
	debugLog("OpenGL", "Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))), true);
	debugLog("OpenGL", "Vendor: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR))), true);
	debugLog("OpenGL", "Renderer: " + std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))), true);
	debugLog("Window", "Create finish time: " + std::to_string(glfwGetTime()), true);
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

bool Window::createWindow(const unsigned int width, const unsigned int height, const char* title) {
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) return error("Window", "createWindow", "Failed to create GLFW window");

	glfwMakeContextCurrent(window);
	return true;
}

bool Window::initGLAD() {	
  return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ? true : error("Window", "initGLAD", "Failed to initialize GLAD");
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