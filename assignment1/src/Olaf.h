#pragma once
#include <glm/glm.hpp>
#include "util/Texture.h"
#include <memory>

#include "objects3d/Sphere.h"
#include <objects3d/Object3D.h>

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
	float shininess = 0.0;

	std::shared_ptr<Texture> carrot;
	//std::shared_ptr<Sphere> sphere;

	std::shared_ptr<Sphere> head;
	std::shared_ptr<Sphere> chest;
	std::shared_ptr<Sphere> body;
	std::shared_ptr<Object3D> hat;

	SceneManager* scene;	// This shoudn't be here but whatever

	friend class SceneManager;
};