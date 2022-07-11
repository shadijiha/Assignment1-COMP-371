#pragma once
#include <glm/glm.hpp>

#include "objects3d/Cube.h"

class SceneManager;

class Olaf {
public:
	Olaf();

	void onCreate(SceneManager& manager);
	void onUpdate(float dt);
	void onDestroyed();


	void randomPosition();

private:
	void listenToEvent(SceneManager& manager);

private:
	float scale = 1.0;

	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);

	std::shared_ptr<Cube> root;
	std::vector<std::shared_ptr<Cube>> elements;

	friend class SceneManager;
};