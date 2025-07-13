#include "core/engine.hpp"

int main() {
	Engine engine{};
	engine.initialize();

	/* 
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
