#include "core/engine.hpp"

int main() {
	Engine engine{};
	engine.initialize();

	/* 
	* Saving scene currently only works with models and lights
	* I have not introduced enough features with primitives to feel worth refactoring the code to make it work
	* 
	* Available Scenes
	* - "" (Empty/Default)
  * - Teacher, Teacher2
  * - Warehouse
	* - Maze (W.I.P)
  *
  * Primitive Scenes
	* - Triangle
	* - Square 
  * - Cube
	*/
	engine.run("Warehouse");
	return 0;
}
