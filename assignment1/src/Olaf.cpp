#include "Olaf.h"
#include "Renderer.h"
#include "SceneManager.h"

Olaf::Olaf()
{
}

void Olaf::onCreate(SceneManager& manager)
{
	manager.addKeyEvent(85,  [this](SceneManager& manager, WindowUserData& data, KeyAction action) {
		if (action == RELEASE)
			this->scale += 0.5f;
	});

	manager.addKeyEvent(74, [this](SceneManager& manager, WindowUserData& data, KeyAction action) {
		if (action == RELEASE)
			this->scale -= 0.5f;
	});
}

void Olaf::onUpdate(float dt)
{
	glm::vec3 rootPos = position;
	glm::vec3 rootScale = glm::vec3{ 3, 3, 1 } * scale;
	glm::vec3 feetScale = glm::vec3{ 1, 1, 1 } * scale;
	rootPos.y += rootPos.y + rootScale.y / 2 + feetScale.y;

	// Root
	Renderer::drawCube(rootPos, rotation, rootScale);
	
	// Feet
	glm::vec3 feetPos = rootPos;
	feetPos.y = feetScale.y / 2;
	feetPos.x += rootScale.x * 0.25;
	Renderer::drawCube(feetPos, rotation, feetScale);
	feetPos.x -= rootScale.x * 0.5;
	Renderer::drawCube(feetPos, rotation, feetScale);

	// Chest
	glm::vec3 chestPos = rootPos;
	auto chestScale = rootScale * 0.7f;
	chestScale.y *= 0.6;
	chestPos.y += rootScale.y / 2 + chestScale.y / 2;
	Renderer::drawCube(chestPos, rotation, chestScale);

	// Head
	glm::vec3 headPos = chestPos;
	auto headScale = chestScale * 0.8f;
	headPos.y += chestScale.y / 2 + headScale.y / 2;
	Renderer::drawCube(headPos, rotation, headScale);

	// Arms
	glm::vec3 armsPos = chestPos;
	armsPos.x += chestScale.x;
	glm::vec3 armsScale = glm::vec3{3, 0.5, .6} * scale;
	Renderer::drawCube(armsPos, rotation, armsScale);
	armsPos.x *= -1.0f;
	Renderer::drawCube(armsPos, rotation, armsScale * -1.0f);
}

void Olaf::onDestroyed()
{
}
