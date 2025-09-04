#ifdef NDEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include <cstdlib>
#endif

#include "starletengine/engine.hpp"

int main() {
#ifdef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc();
#endif
	
	Engine engine;
	engine.assetPath = std::string(ASSET_DIR);

	if (!engine.initialize(1920, 1200, "Test")) 
		return -1;
	if (!engine.setScene("SceneTest"))          
		return -1;
	if (!engine.loadSceneAssets())              
		return -1;
	
	engine.run();
}
