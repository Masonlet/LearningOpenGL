#include "render.hpp"
#include "files.hpp"

#include <cstdio>

Renderer::Renderer(GLFWwindow* window, Model* models, Scene* scene, unsigned int program, unsigned int mvpLocation, unsigned int width, unsigned int height)
	: window(window), models(models), scene(scene), program(program), mvpLocation(mvpLocation), width(width), height(height), aspect(static_cast<float>(width) / static_cast<float>(height)), lastTime(static_cast<float>(glfwGetTime())), deltaTime(1.0f / 60.0f) {

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Renderer::update() {
	const float currentTime = static_cast<float>(glfwGetTime());

	float rawDelta = currentTime - lastTime;
	if (rawDelta > 0.1f) rawDelta = 0.1f;

	lastTime = currentTime;
	deltaTime = 0.9f * deltaTime + 0.1f * rawDelta;

	static float timeAccumulator = 0.0f;
	static int frameCount = 0;
	timeAccumulator += deltaTime;
	frameCount++;

	if (timeAccumulator >= 1.0f) {
		fprintf(stdout, "FPS: %d\n", frameCount);
		timeAccumulator = 0.0f;
		frameCount = 0;
	}

	if (!camera.Paused() && scene && scene->isLoaded()) {
		camera.ProcessInputs(window, models[currentIndex].transform, scene->renderMode(), deltaTime);
	}
}

void Renderer::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr, "glclear error: %x\n", err);
	}

	if (currentProgram != program) {
		glUseProgram(program);
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			fprintf(stderr, "currentProgram error: %x\n", err);
		}
		currentProgram = program;
	}

	for (size_t i = 0; i < scene->objectCount(); i++) {
		if (!models[i].mesh || models[i].mesh->vao == 0 || models[i].mesh->vbo == 0 || models[i].mesh->ebo == 0 || models[i].mesh->numIndicesToDraw <= 0 || !models[i].mesh->vertices || !models[i].mesh->indices)
			continue;

		if (currentVAO != models[i].mesh->vao) {
			glBindVertexArray(models[i].mesh->vao);
			GLenum err;
			while ((err = glGetError()) != GL_NO_ERROR) {
				fprintf(stderr, "VAO %u valid? %s\n", models[i].mesh->vao, models[i].mesh->vao ? "yes" : "NO");
				fprintf(stderr, "vao error: %x\n", err);
			}
			currentVAO = models[i].mesh->vao;
		}

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			fprintf(stderr, "MVP Location: %d\n", mvpLocation);
			fprintf(stderr, "OpenGL error: %x\n", err);
		}

		const Mat4 modelMatrix{Mat4::modelMatrix(models[i].transform)};
		const Mat4 view = camera.LookAt();
		const Mat4 projection = scene->renderMode() == RenderMode::basic3D ? camera.Perspective(aspect) : camera.Orthographic();
		const Mat4 mvp = projection * view * modelMatrix;

		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp.data); 
		glDrawElements(GL_TRIANGLES, models[i].mesh->numIndicesToDraw, GL_UNSIGNED_INT, nullptr);
	}
}

void Renderer::render() {
#ifndef NDEBUG
	fprintf(stderr, "Renderer Start Time: %f\n", glfwGetTime());
#endif

	update(); 
    draw();   

#ifndef NDEBUG
	fprintf(stderr, "Renderer Complete Time: %f\n", glfwGetTime());
#endif
}

void Renderer::updateAspect(unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;
	this->aspect = static_cast<float>(width) / static_cast<float>(height);
}