#include "engine.hpp"

void Engine::renderFrame() {
  glViewport(0, 0, width, height);
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (program != currentProgram) {
    glUseProgram(program);
    currentProgram = program;
  }

  const Mat4 view = camera.LookAt();
  const Mat4 projection = camera.Perspective(aspect);

  glUniformMatrix4fv(modelProjectionLocation, 1, GL_FALSE, projection.ptr());
  glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, view.ptr());

  for (std::map<std::string, ModelInstance>::const_iterator it = modelInstances.begin(); it != modelInstances.end(); ++it) {
    drawModel(it->second, view, projection);
  }

  glBindVertexArray(0);
}

bool Engine::drawModel(const ModelInstance& instance, const Mat4& view, const Mat4& projection) {
  std::map<std::string, ModelDrawInfo>::const_iterator mesh = modelInfos.find(instance.path);
  if(mesh == modelInfos.end()) {
    printf("[DRAWMODEL ERROR] Failed to find model for '%s'\n", instance.path.c_str());
    return false;
  }

  glUniformMatrix4fv(modelLocation, 1, GL_FALSE, instance.modelMatrix.ptr());

  glBindVertexArray(mesh->second.VAO_ID);
  glDrawElements(GL_TRIANGLES, mesh->second.numIndices, GL_UNSIGNED_INT, (void*)0);

  return true;
}


