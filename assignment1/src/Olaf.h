#pragma once
#include <glm/glm.hpp>

class SceneManager;

class Olaf {
public:
	Olaf();

	void onCreate(SceneManager& manager);
	void onUpdate(float dt);
	void onDestroyed();


	void randomPosition();

private:
	float scale = 1.0;

	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);

	friend class SceneManager;
};