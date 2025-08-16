#include "core/cameraManager.hpp"

bool CameraManager::addCamera(const Camera& camera) {
	if (cameras.size() >= MAX_CAMERAS) {
		fprintf(stderr, "[CameraManager ERROR] Max cameras reached, cannot add camera\n");
		return false;
	}

	const std::string& name = camera.getName();
	if (cameras.find(name) != cameras.end()) {
		fprintf(stderr, "[CameraManager ERROR] Camera with name '%s' already exists\n", name.c_str());
		return false;
	}

	cameras[name] = camera;
	if (cameras.size() == 1) activeCameraName = name;
	return true;
}

void CameraManager::setActiveCamera(unsigned int index) {
	if (index >= cameras.size()) {
		printf("[CameraManager WARN] Invalid camera index: %u\n", index);
		return;
	}

	std::map<std::string, Camera>::iterator it = cameras.begin();
	std::advance(it, index);
	activeCameraName = it->first;
}
Camera* CameraManager::getActiveCamera() {
	std::map<std::string, Camera>::iterator it = cameras.find(activeCameraName);
	if (it != cameras.end()) return &(it->second);
	return nullptr;
}
int CameraManager::getCameraCount() {
	return cameras.size();
}

void CameraManager::updateActiveCamera(const Vec3& position, 
	const float yaw, const float pitch,
	const float fov, const float nearPlane, const float farPlane) {

	Camera* cam = getActiveCamera();
	if (!cam) return;

	cam->setPos(position);
	cam->setYaw(yaw);
	cam->setPitch(pitch);
	cam->setFov(fov);
	cam->setNear(nearPlane);
	cam->setFar(farPlane);
}