#include "engine.hpp"
#include "init.hpp"
#include "render.hpp"
#include "camera.hpp"
#include "callbacks.hpp"
#include "mat4.hpp"
#include <cstdio>

constexpr int DEFAULT_WIDTH{1920};
constexpr int DEFAULT_HEIGHT{1080};

void Engine::setupShaders() {
#ifndef NDEBUG
	printf("Shader Start Time: %f\n", glfwGetTime());
#endif

	shaderManager->setBasePath("Assets/Shaders/");

	ShaderManager::Shader vert_shader = shaderManager->makeShader("vertex_shader.glsl");
	ShaderManager::Shader frag_shader = shaderManager->makeShader("fragment_shader.glsl");

	if (!shaderManager->createProgramFromFile("shader1", vert_shader, frag_shader)) {
		printf("%s\n", shaderManager->getLastError().c_str());
		return;
	}
#ifndef NDEBUG
	printf("Shaders Create Time: %f\n", glfwGetTime());
#endif

	program = shaderManager->getIDFromFriendlyName("shader1");

	mvpLocation = glGetUniformLocation(program, "MVP");
	if (mvpLocation == -1) {
		fprintf(stderr, "[ERROR] MVP uniform not found in shader program!\n");
	}
	
}
bool Engine::loadModel(const std::string& path, const Mat4& transform) {
	if (modelInfos.find(path) != modelInfos.end()) {
		printf("Model already loaded: %s\n", path.c_str());
		return false;
	}

	ModelDrawInfo meshInfo;
	if (!meshManager->LoadModelIntoVAO(path, meshInfo, program)) {
		printf("Failed to load model into VAO: %s\n", path.c_str());
		return false;
	}	

	modelInfos[path] = meshInfo;
	modelTransforms[path] = transform;
	return true;
}	  
Engine::Engine() : window{nullptr}, shaderManager{new ShaderManager()}, meshManager{new VAOManager()}, mvpLocation{0}, program{0}, vertex_buffer{0}, currentProgram{0}, height { DEFAULT_HEIGHT }, width{DEFAULT_WIDTH}, deltaTime{0.0f}, lastTime{0.0f}, aspect{0.0f}, wireframe{false} {
	window = initGL(width, height);
	if (!window) {
		fprintf(stderr, "Failed to initialize OpenGL");
		return;
	}
	
	aspect = static_cast<float>(width) / static_cast<float>(height);

#ifndef NDEBUG
	printf("initGL Complete Time: %f\n", glfwGetTime());

	printf("OpenGL Info:\n");
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
#endif

	setupShaders();

	glEnable(GL_DEPTH_TEST);
	glfwSetWindowUserPointer(window, this);
}

void Engine::unloadModel(ModelDrawInfo& info) {
	glDeleteVertexArrays(1, &info.VAO_ID);
	glDeleteBuffers(1, &info.VertexBufferID);
	glDeleteBuffers(1, &info.IndexBufferID);

	delete[] info.vertices;
	delete[] info.indices;
}
void Engine::clearModels() {
	for (std::map<std::string, ModelDrawInfo>::iterator it = modelInfos.begin(); it != modelInfos.end(); ++it) {
		ModelDrawInfo& info = it->second;
		unloadModel(info);
	}
	modelInfos.clear();
	modelTransforms.clear();
}
Engine::~Engine() {
	if (program) glDeleteProgram(program);
	delete shaderManager;
	delete meshManager;
	clearModels();
	if(window) cleanupWindow(window);
}

void Engine::setCallbacks() {
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}
void Engine::updateDeltaTime(const float currentTime) {
	constexpr float MAX_DELTA = 0.1f;
	constexpr float SMOOTHING_FACTOR = 0.9f;

	float rawDelta = currentTime - lastTime;

	if (lastTime == 0.0f) {
		lastTime = currentTime;
		deltaTime = 0.0f;
		return;
	}

	if (rawDelta > MAX_DELTA) {
#ifndef NDEBUG
		printf("[WARN] Large delta time clamped: %f\n", rawDelta);
#endif
		rawDelta = MAX_DELTA;
	}

	lastTime = currentTime;
	deltaTime = SMOOTHING_FACTOR * deltaTime + (1.0f - SMOOTHING_FACTOR) * rawDelta;
}
void Engine::handleInputs() {
	camera.ProcessInputs(window, deltaTime);

}
bool Engine::drawModel(const std::string& name, const Mat4& view, const Mat4& projection) {
	std::map<std::string, ModelDrawInfo>::iterator model = modelInfos.find(name);
	if(model == modelInfos.end()) {
		printf("drawModel [ERROR] Failed to find model for '%s'\n", name.c_str());
		return false;
	}
	
	std::map<std::string, Mat4>::iterator transform = modelTransforms.find(name);
	if(transform == modelTransforms.end()) {
		printf("drawModel [ERROR] Failed to find transform for '%s'\n", name.c_str());
		return false;
	}

	const Mat4 mvp = projection * view * transform->second;
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp.data);
	glBindVertexArray(model->second.VAO_ID);
	glDrawElements(GL_TRIANGLES, model->second.numIndices, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
	return true;
}
void Engine::renderFrame() {
	glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Set background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear framebuffer

	const Mat4 view = camera.LookAt();
	const Mat4 projection = camera.Perspective(aspect);
		
	if (program != currentProgram) {
		glUseProgram(program);
		currentProgram = program;
	}
		
	for (std::map<std::string, ModelDrawInfo>::const_iterator it = modelInfos.begin(); it != modelInfos.end(); ++it) {
		const std::string& name = it->first;
		drawModel(name, view, projection);
	}
}
void Engine::run() {
	setCallbacks();

	while (!glfwWindowShouldClose(window)) {	
		const float currentTime = static_cast<float>(glfwGetTime());

		updateDeltaTime(currentTime);
		handleInputs();
		renderFrame();

		glfwSwapBuffers(window); // Present the rendered frame
		glfwPollEvents();        // Process window input/events
	}
}

void Engine::updateAspect(unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;
	this->aspect = static_cast<float>(width) / static_cast<float>(height);
}
void Engine::updateWireframe() {
	wireframe = !wireframe;
}