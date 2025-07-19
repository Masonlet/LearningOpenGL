#include "core/engine.hpp"

int main() {
	Engine engine{};
	engine.initialize(1920, 1200, "Test");

	/* 
	* Saving scene currently only works with models and lights
	* I have not introduced enough features with primitives to feel worth refactoring the code to make it work
	*/
	if (!engine.setScene("SceneTest")) 
		return -1;

	engine.run();

	return 0;
}
