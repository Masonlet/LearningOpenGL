#ifdef NDEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include <cstdlib>
#endif

#include "core/engine.hpp"

int main() {
#ifdef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc();
#endif
	
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
