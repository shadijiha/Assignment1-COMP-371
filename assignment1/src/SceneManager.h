#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <functional>
#include <Camera.h>
#include <Shader.h>
#include <Olaf.h>
#include "KeyCodes.h"
#include "Light.h"

class SceneManager;

struct WindowUserData {
	uint32_t width;
	uint32_t height;
	SceneManager* sceneManager;
};



class SceneManager
{
public:
	using KeyEvent = std::function<void(SceneManager&, WindowUserData&, KeyAction action)>;

	SceneManager(uint32_t width, uint32_t height);
	~SceneManager();

	void run();
	void onCreate();
	void onUpdate(float dt);
	void onUI();
	void onDestroyed();

	void addKeyEvent(int key, KeyEvent func);

	Camera& getCamera() { return camera; }
	GLFWwindow* getWindow() { return window; }

	bool isKeyDown(KeyCode keyCode) const;
	bool isMouseButtonDown(MouseCode buttonCode) const;
	bool isShiftPressed() const;
	glm::vec2 getMousePos() const;

private:
	void listenToEvents(GLFWwindow* window);

private:
	std::vector<std::pair<int, KeyEvent>> keyEvents;

	GLFWwindow* window;
	Camera camera;
	Shader* shader;
	Light light;
	Olaf olaf;

	float lastDt = 0.0f;	
};

