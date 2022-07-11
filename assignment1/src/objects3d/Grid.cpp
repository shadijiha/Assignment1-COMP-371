#include "Grid.h"

Grid::Grid(uint32_t count) {
	regenerate(count);
}

void Grid::onCreate() {
}

void Grid::onUpdate(float dt) {
	for (auto& element : elements) {
		element.onUpdate(dt);
	}
}

void Grid::onDestroyed() {
}

void Grid::regenerate(uint32_t count) {
	this->count = count;

	elements.clear();
	elements.reserve(count * count);

	// Draw x y yellow grid
	constexpr float gridDim = 1;
	const int countPerAxis = count;
	for (int i = -countPerAxis / 2; i < countPerAxis / 2; i++) {
		for (int j = -countPerAxis / 2; j < countPerAxis / 2; j++) {
			Cube cube({ i, 0, j }, { 0, 0, 0 }, { gridDim, 0.01, gridDim });
			cube.color = { 1, 1, 0, 1 };
			cube.renderingMode = RenderingMode::Lines;
			elements.push_back(cube);
		}
	}


	// Draw axis lines
	constexpr float lineLength = 5 * gridDim;
	elements.push_back(Cube({ lineLength / 2, 0, 0 }, { 0, 0, 0 }, { lineLength, 0.02, 0.02 }, { 1, 0, 0, 1 }));
	elements.push_back(Cube({ 0, lineLength / 2, 0 }, { 0, 0, 0 }, { 0.02, lineLength, 0.02 }, { 0, 1, 0, 1 }));
	elements.push_back(Cube({ 0, 0, lineLength / 2 }, { 0, 0, 0 }, { 0.02, 0.02, lineLength }, { 0, 0, 1, 1 }));

}
