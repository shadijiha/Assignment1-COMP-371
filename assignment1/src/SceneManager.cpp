#include "SceneManager.h"
#include <assert.h>
#include "Renderer.h"

static void handleErrors(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam);


SceneManager::SceneManager(uint32_t width, uint32_t height)
	: camera(width, height)
{
	// Init GLFW and OpenGL
	/* Initialize the library */
	if (!glfwInit())
		assert(false, "Failed to initialize GLFW!");

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Hehexd", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		assert(false, "Failed to create window");
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	listenToEvents(window);

	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		assert(false, "Failed to create GLEW context");
	}

	WindowUserData* data = new WindowUserData;	// Because we need this to get the camera in events callbacks
	data->sceneManager = this;
	data->width = width;
	data->height = height;

	glfwSetWindowUserPointer(window, data);

	// Default renderer settings
	shader = new Shader("shaders/shader.glsl");
	Renderer::init();
	Renderer::setCamera(&camera);
	Renderer::setDefaultShader(shader);
}

SceneManager::~SceneManager()
{
}

void SceneManager::run()
{
	// During init, enable buffers and debug output
	glClearColor(0.05, 0.05, 0.2, 1.0);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDebugMessageCallback(handleErrors, 0);

	float lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		const float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw x, y grid
		Renderer::drawGrid();
		onUpdate(dt);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void SceneManager::onCreate()
{
	camera.setPosition({ 0, 2, 2.3 });
	olaf.onCreate(*this);

	constexpr float camSpeed = 0.08f;
	constexpr float camRotSpeed = 5.0f; // Degress

	addKeyEvent(GLFW_KEY_D, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camPos = camera.getPosition();
		camPos.x += camSpeed;
		camera.setPosition(camPos);
	});

	addKeyEvent(GLFW_KEY_A, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camPos = camera.getPosition();
		camPos.x -= camSpeed;
		camera.setPosition(camPos);
	});

	addKeyEvent(GLFW_KEY_W, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camPos = camera.getPosition();
		camPos.z -= camSpeed;
		camera.setPosition(camPos);
		});

	addKeyEvent(GLFW_KEY_S, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camPos = camera.getPosition();
		camPos.z += camSpeed;
		camera.setPosition(camPos);
		});

	addKeyEvent(GLFW_KEY_UP, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camPos = camera.getPosition();
		camPos.y += camSpeed;
		camera.setPosition(camPos);
		});

	addKeyEvent(GLFW_KEY_DOWN, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camPos = camera.getPosition();
		camPos.y -= camSpeed;
		camera.setPosition(camPos);
		});

	// Camera rotation
	addKeyEvent(GLFW_KEY_RIGHT, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camRot = camera.getRotation();
		camRot.y += camRotSpeed;
		camera.setRotation(camRot);
		});

	addKeyEvent(GLFW_KEY_LEFT, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camRot = camera.getRotation();
		camRot.y -= camRotSpeed;
		camera.setRotation(camRot);
		});

	addKeyEvent(GLFW_KEY_P, [](SceneManager& scene, WindowUserData& data, KeyAction action) {
		Renderer::setDefaultRenderering(GL_POINTS);
	});

	addKeyEvent(GLFW_KEY_L, [](SceneManager& scene, WindowUserData& data, KeyAction action) {
		Renderer::setDefaultRenderering(GL_LINE_LOOP);
		});

	addKeyEvent(GLFW_KEY_T, [](SceneManager& scene, WindowUserData& data, KeyAction action) {
		Renderer::setDefaultRenderering(GL_TRIANGLES);
	});

}

void SceneManager::onUpdate(float dt)
{
	olaf.onUpdate(dt);
}

void SceneManager::onDestroyed()
{
	olaf.onDestroyed();

	delete shader;
	delete (GLFWwindow*)glfwGetWindowUserPointer(window);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void SceneManager::addKeyEvent(int key, KeyEvent func)
{
	keyEvents.push_back(std::make_pair(key, func));
}

void SceneManager::listenToEvents(GLFWwindow* window) {

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		WindowUserData& data = *(WindowUserData*)glfwGetWindowUserPointer(window);
		data.width = width;
		data.height = height;

		glViewport(0, 0, width, height);
		data.sceneManager->getCamera().setWindowSize(width, height);
	});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		WindowUserData* data = (WindowUserData*)glfwGetWindowUserPointer(window);
		auto& keyEvents = data->sceneManager->keyEvents;

		for (std::pair<int, KeyEvent>& pair : keyEvents)
		{
			if (key == pair.first)
			{
				pair.second(*data->sceneManager, *data, (KeyAction)action);
				break;
			}
		}
	});
}

static void handleErrors(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

