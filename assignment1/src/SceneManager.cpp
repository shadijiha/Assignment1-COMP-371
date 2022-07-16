#include "SceneManager.h"
#include <assert.h>
#include "Renderer.h"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "util/UI.h"

static void handleErrors(GLenum source, GLenum type, GLuint id,
						 GLenum severity, GLsizei length, const GLchar* message, const void* userParam);


SceneManager::SceneManager(uint32_t width, uint32_t height)
	: width(width), height(height)
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

		// Draw x, y grid and skybox
		
		Renderer::drawGrid();
		onUpdate(dt);
		
		// skybox cube
		Renderer::drawSkyBox();

		// Draw UI on top of everything
		onUI();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

static void setupImGui(GLFWwindow* win);
void SceneManager::onCreate()
{
	// Default renderer settings
	camera = std::make_shared<Camera>(*this, width, height);
	shader = std::make_shared<Shader>("shaders/shader.glsl");
	light = std::make_shared<Light>();

	Renderer::init();
	Renderer::setCamera(camera);
	Renderer::setDefaultShader(shader);
	Renderer::setLight(light);

	camera->setRotation({ 30.0f, 30.0f, 0 });
	camera->setPosition({ -3, 4, 10 });
	olaf.onCreate(*this);

	constexpr float camSpeed = 0.08f;
	constexpr float camRotSpeed = 5.0f; // Degress

	addKeyEvent(GLFW_KEY_UP, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camRot = camera.getRotation();
		camRot.x += camRotSpeed;
		camera.setRotation(camRot);
	});

	addKeyEvent(GLFW_KEY_DOWN, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		auto camRot = camera.getRotation();
		camRot.x -= camRotSpeed;
		camera.setRotation(camRot);;
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

	addKeyEvent(GLFW_KEY_HOME, [=](SceneManager& scene, WindowUserData& data, KeyAction action) {
		auto& camera = scene.getCamera();
		camera.setRotation({ 30.0f, 30.0f, 0 });
		camera.setPosition({-3, 4, 10});
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

	addKeyEvent(GLFW_KEY_Z, [this](SceneManager& scene, WindowUserData& data, KeyAction action) {
		std::cout << 1 / lastDt << std::endl;
	});

	addKeyEvent(GLFW_KEY_B, [this](SceneManager& scene, WindowUserData& data, KeyAction action) {
		if (action == KeyAction::RELEASE)
			Renderer::textures = !Renderer::textures;
	});

	
	setupImGui(window);
}

void SceneManager::onUpdate(float dt)
{
	lastDt = dt;
	camera->onUpdate(dt);
	olaf.onUpdate(dt);
}

void SceneManager::onUI() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//static bool show = true;
	//ImGui::ShowDemoWindow(&show);

	// UI Code goes here
	ImGui::Begin("Controls");

	if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused()) {
		camera->enableMouseZoom(false);
	}		
	else
		camera->enableMouseZoom(true);

	ImGui::Text("You can use these controls or the Key shortcuts");

	// Olaf settings
	UI::drawTreeNode<Olaf>("Olaf settings", [this]() {
		UI::drawVec3Control("position", olaf.position);
		UI::drawVec3Control("rotation", olaf.rotation);
		UI::drawVec1Control<float>("scale", olaf.scale);

		if (ImGui::Button("random position"))
			olaf.randomPosition();
	});

	// Camera settings
	UI::drawTreeNode<Camera>("Camera settings", [this]() {
		if (UI::drawVec3Control("position", camera->position))
			camera->recalculateMatrix();

		if (UI::drawVec3Control("rotation", camera->rotation))
			camera->recalculateMatrix();
	});

	// Grid size
	UI::drawTreeNode<Renderer>("Grid settings", [this]() {
		UI::drawVec1Control<int>("Grid count ", Renderer::GridSize, 0, 100);

		// Select menu to change Rendering mode
		static std::string currentSelection = "Triangles";
		UI::drawDropDown("Type", {
			{"Triangles", []() { Renderer::setDefaultRenderering(GL_TRIANGLES); }},
			{"Line loop", []() { Renderer::setDefaultRenderering(GL_LINE_LOOP); }},
			{"Points", []() { Renderer::setDefaultRenderering(GL_POINTS); }}
		}, currentSelection);

		static std::string texturesCurrentSelection =  "On";
		UI::drawDropDown("Textures", {
			{"On", []() { Renderer::textures = true; }},
			{"Off", []() { Renderer::textures = false; }},
			}, texturesCurrentSelection);
	});

	// Light settings
	UI::drawTreeNode<Light>("Light settings", [this]() {
		UI::drawVec3Control("position", light->position);
		UI::drawColorControl("colour", light->color);
		UI::drawVec1Control("ambiant strength", light->ambientStrength);
	});

	ImGui::End();


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void SceneManager::onDestroyed()
{
	olaf.onDestroyed();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete (WindowUserData*)glfwGetWindowUserPointer(window);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void SceneManager::addKeyEvent(int key, KeyEvent func)
{
	keyEvents.push_back(std::make_pair(key, func));
}

bool SceneManager::isKeyDown(KeyCode keyCode) const {
	return glfwGetKey(window, (int)keyCode) == GLFW_PRESS;
}

bool SceneManager::isMouseButtonDown(MouseCode buttonCode) const {
	return glfwGetMouseButton(window, (int)buttonCode) == GLFW_PRESS;
}

bool SceneManager::isShiftPressed() const
{
	return isKeyDown(KeyCode::LEFT_SHIFT);
}

glm::vec2 SceneManager::getMousePos() const {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return { xpos, ypos };
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

static void setupImGui(GLFWwindow* win) {
	float xscale, yscale;
	auto* monitor = glfwGetPrimaryMonitor();
	glfwGetMonitorContentScale(monitor, &xscale, &yscale);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	io.Fonts->AddFontFromFileTTF("fonts/Open_Sans/OpenSans-Bold.ttf", 18.0f);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("fonts/Open_Sans/OpenSans-Regular.ttf", 18.0f);
	io.FontGlobalScale = xscale;
	ImGui::GetStyle().ScaleAllSizes(xscale);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Setup Platform/Renderer backends
	const char* glsl_version = "#version 130";
 	ImGui_ImplGlfw_InitForOpenGL(win, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
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

