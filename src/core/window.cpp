#include <glad/glad.h> 
#include "core/window.hpp"
#include "utils/log.hpp"
#include <cstdio>

bool Window::createWindow(const unsigned int width, const unsigned int height, const char* title) {
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) return error("Window", "createWindow", "Failed to create GLFW window");

	glfwMakeContextCurrent(window); // This does not need to be here, should extract to window manager ~~~
	return true;
}
Window::Window(const unsigned int width, const unsigned int height, const char* title): width(width), height(height) {
	// Should the constructor be responsible for creating the window? ~~~
	// Should the window just be a data holder and the window manager handle everything? ~~~
	debugLog("Window", "Create start time: " + std::to_string(glfwGetTime()), true);

	if (!createWindow(width, height, title)) return; // Constructor should not create window ~~~
	if (!initGLAD()) return; // Constructor should not initialize GLAD ~~~

	setupGLState(); // Constructor should not setup GL state ~~~

	// Needs to be extracted out of window constructor to a more appropriate place ~~~
	debugLog("OpenGL", "OpenGL Info", true); 
	debugLog("OpenGL", "Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))), true);
	debugLog("OpenGL", "Vendor: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR))), true);
	debugLog("OpenGL", "Renderer: " + std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))), true);
	debugLog("Window", "Create finish time: " + std::to_string(glfwGetTime()), true);
}

Window::~Window() {
	// Should window be responsible for destroying itself? ~~~
	// Initial thoughts think it SHOULD be fine, need to consider more after extracting other stuff ~~~
	if (window) {
		glfwDestroyWindow(window);
		window = nullptr;
	}
}

void Window::pollEvents() const { 
	// Should window be responsible for polling events? ~~~
	// Initial thoughts think it SHOULD be fine, need to consider more after extracting other stuff ~~~
	glfwPollEvents(); 
}
void Window::swapBuffers() const { 
	// Should window be responsible for swapping buffers? ~~~
	// Initial thoughts think it SHOULD be fine, need to consider more after extracting other stuff ~~~
	glfwSwapBuffers(window); 
}

bool Window::shouldClose() const { 
	// Should window be responsible for checking if it should close? ~~~
	// Initial thoughts think it SHOULD be GOOD, need to consider more after extracting other stuff ~~~
	return glfwWindowShouldClose(window); 
}
void Window::updateViewport(const unsigned int widthIn, const unsigned int heightIn) {
	// Should window be responsible for updating viewport? ~~~
	// Initial thoughts think it SHOULD be fine, need to consider more after extracting other stuff ~~~
	width = widthIn;
	height = heightIn;
	glViewport(0, 0, width, height);
}

bool Window::initGLAD() {	 	
	// Should window be responsible for initializing GLAD? ~~~
	// Initial thoughts thinks NO, should be extracted elsewhere ~~~
  return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ? true : error("Window", "initGLAD", "Failed to initialize GLAD");
}

void Window::setupGLState() { 	// Need to extract to some form of state in window (probably window) / window manager ~~~
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Setting clear colour here AND elsewhere, need to adjust ~~~
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}