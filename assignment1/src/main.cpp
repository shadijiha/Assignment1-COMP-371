#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include "Shader.h"
#include <glm/gtx/transform.hpp>
#include <Camera.h>
#include <Renderer.h>
#include <glm/gtx/color_space.hpp>

static void handleErrors(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void listenToEvents(GLFWwindow* window) {

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		WindowUserData* data = (WindowUserData*)glfwGetWindowUserPointer(window);
		Camera& camera = *data->camera;
		glm::vec3 camPos = camera.getPosition();


		switch (key) {
		case GLFW_KEY_D:
			camPos.x += 0.05;
			camera.setPosition(camPos);
			break;

		case GLFW_KEY_A:
			camPos.x -= 0.05;
			camera.setPosition(camPos);
			break;
		case GLFW_KEY_W:
			camPos.z -= 0.05;
			camera.setPosition(camPos);
			break;

		case GLFW_KEY_S:
			camPos.z += 0.05;
			camera.setPosition(camPos);
			break;

		case GLFW_KEY_UP:
			camPos.y += 0.05;
			camera.setPosition(camPos);
			break;
		case GLFW_KEY_DOWN:
			camPos.y -= 0.05;
			camera.setPosition(camPos);
			break;

		// Rendering mode
		case GLFW_KEY_P:
			Renderer::setDefaultRenderering(GL_POINTS);
			break;

		case GLFW_KEY_L:
			Renderer::setDefaultRenderering(GL_LINE_LOOP);
			break;

		case GLFW_KEY_T:
			Renderer::setDefaultRenderering(GL_TRIANGLES);
			break;
		}
	});
}

int main(int argc, const char** argv) {
	const float window_scale = 2.0f;
	const uint32_t window_width = 1024, window_height = 786;

	// Init GLFW and OpenGL
	/* Initialize the library */
	if (!glfwInit())
		assert(false, "Failed to initialize GLFW!");

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(window_width * window_scale, window_height * window_scale, "Hehexd", NULL, NULL);
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


	// Main game stuff
	// Get and compile shader
	Camera camera(window_width, window_height);
	Shader shader("shaders/shader.glsl");

	WindowUserData data;	// Because we need this to get the camera in events callbacks
	data.camera = &camera;
	data.width = window_width;
	data.height = window_height;
	glfwSetWindowUserPointer(window, &data);

	camera.setPosition({ 0, 2, 2.3 });
	// Main loop
	glClearColor(0.05, 0.05, 0.2, 1.0);

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEPTH_TEST);
	glDebugMessageCallback(handleErrors, 0);

	// Default renderer settings
	Renderer::init();
	Renderer::setCamera(&camera);
	Renderer::setDefaultShader(&shader);

	float color = 0.0f;
	float rotation = 90.0f;

	float lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		const float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw x, y grid
		Renderer::drawGrid();

		//Renderer::drawCube({ 0, 0, 0 }, { rotation, 0, 0 }, { 0.5, 0.5, 0.5 }, glm::vec4(1.0), shader);
		//Renderer::drawCube({ 0, 0, 0 }, { rotation, 0, 0 }, { 0.5, 0.5, 0.5 }, glm::vec4(glm::rgbColor(glm::vec3{ color, 1.0f, 1.0f }), 1.0), shader);
		color += 0.1;
		rotation += 30 * dt;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// Collect garbage
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
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