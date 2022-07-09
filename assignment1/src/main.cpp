#include <iostream>
#include "SceneManager.h"

int main(int argc, const char** argv) {
	const float window_scale = 2.0f;
	const uint32_t window_width = 1024, window_height = 786;

	// Init scene
	SceneManager scene(window_width * window_scale, window_height * window_scale);

	// Main game stuff
	// Get and compile shader
	scene.onCreate();

	// Game loop
	scene.run();
	
	// Cleanup
	scene.onDestroyed();
	return 0;
}
