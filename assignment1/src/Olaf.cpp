#include "Olaf.h"
#include <glm/gtx/transform.hpp>
#include "Renderer.h"
#include "SceneManager.h"

Olaf::Olaf()
{
	
}

void Olaf::onCreate(SceneManager& manager)
{
	carrot = std::make_shared<Texture>("shaders/carrot.jpg");

	body = std::make_shared<Sphere>();
	body->setShader(manager.shader);
	body->setCamera(manager.camera);
	body->setLight(manager.light);

	chest = std::make_shared<Sphere>();
	chest->setShader(manager.shader);
	chest->setCamera(manager.camera);
	chest->setLight(manager.light);

	head = std::make_shared<Sphere>();
	head->setShader(manager.shader);
	head->setCamera(manager.camera);
	head->setLight(manager.light);

	hat = std::make_shared<Object3D>("shaders/hat.obj");
	hat->setShader(manager.shader);
	hat->setCamera(manager.camera);
	hat->setLight(manager.light);
	

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
}

void Olaf::onUpdate(float dt)
{
	glm::vec3 rootPos = position;
	glm::vec3 rootScale = glm::vec3{ 3, 3, 1 } * scale;
	glm::vec3 feetScale = glm::vec3{ 1, 1, 1 } * scale;
	rootPos.y += rootPos.y + rootScale.y / 2 + feetScale.y;

	// Root
	//Renderer::drawCube(rootPos, rotation, rootPos, rootScale);
	body->position = rootPos;
	body->rotation = rotation;
	body->scale = { rootScale.x, rootScale.y , rootScale.x};
	body->shininess = shininess;
	body->onDraw(dt);
	
	// Feet
	{
		glm::vec3 feetPos = rootPos;
		feetPos.y -= rootScale.y / 2 + feetScale.y / 2;
		feetPos.x += rootScale.x * 0.25;
		Renderer::drawCube(feetPos, rotation, rootPos , feetScale);
		feetPos.x -= rootScale.x * 0.5;
		Renderer::drawCube(feetPos, rotation, rootPos, feetScale);
	}

	// Chest
	glm::vec3 chestPos = rootPos;
	auto chestScale = rootScale * 0.6f;
	//chestScale.y *= 0.6;
	chestPos.y += rootScale.y / 2 + chestScale.y / 2;
	//Renderer::drawCube(chestPos, rotation, rootPos, chestScale);
	chest->position = chestPos;
	chest->rotation = rotation;
	chest->setRotationOrigin(rootPos);
	chest->scale = { chestScale.x, chestScale.y, chestScale.x};
	chest->shininess = shininess;
	chest->onDraw(dt);

	// Head
	glm::vec3 headPos = chestPos;
	auto headScale = chestScale * 0.8f;
	headPos.y += chestScale.y / 2 + headScale.y / 2;
	//Renderer::drawCube(headPos,  rotation, rootPos , headScale);
	head->position = headPos;
	head->rotation = rotation;
	head->setRotationOrigin(rootPos);
	head->scale = { headScale.x, headScale.y, headScale.x };
	head->shininess = shininess;
	head->onDraw(dt);

	// Nose
	{
		glm::vec3 nosePos = headPos;
		nosePos.z += headScale.z + 0.6;
		auto noseScale = glm::vec3{0.2, 0.2, 0.5} * scale;

		glm::vec4 color = Renderer::textures ? glm::vec4{1, 1, 1, 1} : glm::vec4{ 235.0f / 255.0f, 119.0f / 255.0f, 52.0f / 255.0f, 1.0f };
		RenderingOptions options;
		options.position = nosePos;
		options.rotation = rotation;
		options.rotationOrigin = rootPos;
		options.scale = noseScale;
		options.color = color;
		options.texture = carrot;
		Renderer::drawCube(options);
	}

	// Eyes
	{
		glm::vec3 eyesPos = headPos;
		eyesPos.z += headScale.z + 0.3f;
		eyesPos.x += headScale.x * 0.15;
		eyesPos.y += headScale.y * 0.25;

		auto eyesScale = glm::vec3{ 0.2, 0.2, 0.2 } *scale;

		Renderer::drawCube(eyesPos, rotation, rootPos,
			eyesScale, { 0, 0, 0, 1 });


		eyesPos = headPos;
		eyesPos.z += headScale.z + 0.3f;
		eyesPos.x -= headScale.x * 0.25;
		eyesPos.y += headScale.y * 0.25;
		Renderer::drawCube(eyesPos, rotation, rootPos,
			eyesScale, { 0, 0, 0, 1 });
	}

	// Arms
	{
		glm::vec3 armsPos = chestPos;
		armsPos.x += chestScale.x;
		glm::vec3 armsScale = glm::vec3{ 3, 0.5, .6 } *scale;

		Renderer::drawCube(armsPos, rotation, rootPos, armsScale);

		armsPos.x -= chestScale.x * 2;
		Renderer::drawCube(armsPos, rotation, rootPos, -armsScale);
	}


	hat->setPosition(headPos + glm::vec3{ 0, 0, headScale.z });
	//hat->scale = { 0.5, 0.5, 0.5 };
	hat->rotationOrigin = rootPos;
	hat->rotation = rotation;
	hat->onDraw(dt);
	
}

void Olaf::onDestroyed()
{
}

void Olaf::randomPosition() {
	const int half = Renderer::GridSize / 2;
	this->position = { rand() % Renderer::GridSize - half,
			0, rand() % Renderer::GridSize - half };
}
