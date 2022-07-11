#include "Olaf.h"
#include <glm/gtx/transform.hpp>
#include "Renderer.h"
#include "SceneManager.h"

Olaf::Olaf()
{
}

void Olaf::onCreate(SceneManager& manager)
{
	// Movement and rotation
	constexpr float speed = 0.1;
	manager.addKeyEvent(GLFW_KEY_D, [this, speed](SceneManager& scene, WindowUserData& data, KeyAction action) {
		if (scene.isShiftPressed()) {
			this->position.x += speed;
		}
		else
		{
			this->rotation.y += 5;
		}
	});

	manager.addKeyEvent(GLFW_KEY_A, [this, speed](SceneManager& scene, WindowUserData& data, KeyAction action) {
		if (scene.isShiftPressed()) {
			this->position.x -= speed;
		}
		else
		{
			this->rotation.y += -5;
		}
	});

	manager.addKeyEvent(GLFW_KEY_W, [this, speed](SceneManager& scene, WindowUserData& data, KeyAction action) {
		if (scene.isShiftPressed()) {
			this->position.z -= speed;
		}
	});
	manager.addKeyEvent(GLFW_KEY_S, [this, speed](SceneManager& scene, WindowUserData& data, KeyAction action) {
		if (scene.isShiftPressed()) {
			this->position.z += speed;
		}
	});


	// Scalling
	manager.addKeyEvent(GLFW_KEY_U,  [this](SceneManager& manager, WindowUserData& data, KeyAction action) {
		if (action == RELEASE)
			this->scale += 0.5f;
	});

	manager.addKeyEvent(GLFW_KEY_J, [this](SceneManager& manager, WindowUserData& data, KeyAction action) {
		if (action == RELEASE)
			this->scale -= 0.5f;
	});

	// Random position
	manager.addKeyEvent(GLFW_KEY_SPACE, [this](SceneManager& manager, WindowUserData& data, KeyAction action) {
		if (action == RELEASE)
			randomPosition();
	});

	// Create the Olaf
	glm::vec3 rootPos = position;
	glm::vec3 rootScale = glm::vec3{ 3, 3, 1 } * scale;
	glm::vec3 feetScale = glm::vec3{ 1, 1, 1 } * scale;
	root = std::make_shared<Cube>(position, rotation, rootScale);
	rootPos.y += rootPos.y + rootScale.y / 2 + feetScale.y;

	// Feet
	{
		Cube leftFeet, rightFeet;
		leftFeet.setParent(root);
		rightFeet.setParent(root);

		leftFeet.position = { rootScale.x * 0.25,  -1 * (rootScale.y / 2 + feetScale.y / 2) , 0};
		leftFeet.scale = feetScale;

		rightFeet.position = { -1 * (rootScale.x * 0.25), -1 * (rootScale.y / 2 + feetScale.y / 2) , 0};
		rightFeet.scale = feetScale;

		elements.push_back(leftFeet);
		elements.push_back(rightFeet);
	}

}

void Olaf::onUpdate(float dt)
{
	glm::vec3 rootPos = position;
	glm::vec3 rootScale = glm::vec3{ 3, 3, 1 } * scale;
	glm::vec3 feetScale = glm::vec3{ 1, 1, 1 } * scale;
	rootPos.y += rootPos.y + rootScale.y / 2 + feetScale.y;

	// Root
	root->position = rootPos;
	root->rotation = rotation;
	root->scaleFactor = glm::vec3(scale);
	root->onUpdate(dt);

	for (auto element : elements) {
		element.onUpdate(dt);
	}

	
	/*// Feet
	{
		glm::vec3 feetPos = rootPos;
		feetPos.y -= rootScale.y / 2 + feetScale.y / 2;
		feetPos.x += rootScale.x * 0.25;
		Renderer::drawCube(feetPos, { rotation, rootPos }, feetScale);
		feetPos.x -= rootScale.x * 0.5;
		Renderer::drawCube(feetPos, { rotation, rootPos }, feetScale);
	}

	// Chest
	glm::vec3 chestPos = rootPos;
	auto chestScale = rootScale * 0.7f;
	chestScale.y *= 0.6;
	chestPos.y += rootScale.y / 2 + chestScale.y / 2;
	Renderer::drawCube(chestPos, { rotation, rootPos }, chestScale);

	// Head
	glm::vec3 headPos = chestPos;
	auto headScale = chestScale * 0.8f;
	headPos.y += chestScale.y / 2 + headScale.y / 2;
	Renderer::drawCube(headPos, { rotation, rootPos }, headScale);

	// Nose
	{
		glm::vec3 nosePos = headPos;
		nosePos.z += headScale.z / 2;
		auto noseScale = glm::vec3{0.1, 0.1, 0.5} * scale;

		Renderer::drawCube(nosePos, { rotation, rootPos },
				noseScale,
				glm::vec4{ 235.0f / 255.0f, 119.0f / 255.0f, 52.0f / 255.0f, 1.0f });
	}

	// Eyes
	{
		glm::vec3 eyesPos = headPos;
		eyesPos.z += headScale.z / 2;
		eyesPos.x += headScale.x * 0.25;
		eyesPos.y += headScale.y * 0.25;

		auto eyesScale = glm::vec3{ 0.2, 0.2, 0.2 } *scale;

		Renderer::drawCube(eyesPos, { rotation, rootPos },
			eyesScale, { 0, 0, 0, 1 });


		eyesPos = headPos;
		eyesPos.z += headScale.z / 2;
		eyesPos.x -= headScale.x * 0.25;
		eyesPos.y += headScale.y * 0.25;
		Renderer::drawCube(eyesPos, { rotation, rootPos },
			eyesScale, { 0, 0, 0, 1 });
	}

	// Arms
	{
		glm::vec3 armsPos = chestPos;
		armsPos.x += chestScale.x;
		glm::vec3 armsScale = glm::vec3{ 3, 0.5, .6 } *scale;

		Renderer::drawCube(armsPos, { rotation, rootPos }, armsScale);

		armsPos.x -= chestScale.x * 2;
		Renderer::drawCube(armsPos, { rotation, rootPos }, -armsScale);
	}*/
}

void Olaf::onDestroyed()
{
}

void Olaf::randomPosition() {
	/*const int half = Renderer::GridSize / 2;
	this->position = { rand() % Renderer::GridSize - half,
			0, rand() % Renderer::GridSize - half };*/
}
