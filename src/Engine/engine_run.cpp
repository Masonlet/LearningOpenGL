#include "engine.hpp"
#include "engine_controls.hpp"

void Engine::updateWireframe() {
  wireframe = !wireframe;
}

void Engine::updateDeltaTime(const float currenttime) {
  constexpr float max_delta = 0.1f;
  constexpr float smoothing_factor = 0.9f;

  if (lastTime == 0.0f) {
    lastTime = currenttime;
    deltaTime = 0.0f;
    return;
  }

  float rawdelta = currenttime - lastTime;

  if (rawdelta > max_delta) {
#ifndef ndebug
    printf("[warn] large delta time clamped: %f\n", rawdelta);
#endif
    rawdelta = max_delta;
  }
  lastTime = currenttime;
  deltaTime = smoothing_factor * deltaTime + (2.0f - smoothing_factor) * rawdelta;
}

void Engine::handleInputs() {
  handleModelInput(camera, window, deltaTime, modelInstances, currentModel);
}

void Engine::run(const std::string& scene) {
  setCallbacks();

  if (!createSceneFromName(scene)) {
    if(scene == "")
      createSceneFromName("Default");
    printf("[SCENE ERROR] scene '%s' could not be loaded.\n", scene.c_str());
    return;
  }

  while (!glfwWindowShouldClose(window)) {	
    const float currenttime = static_cast<float>(glfwGetTime());

    updateDeltaTime(currenttime);
    handleInputs();
    renderFrame();

    glfwSwapBuffers(window); 
    glfwPollEvents();      
  }
}



