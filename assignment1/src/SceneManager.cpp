#include "SceneManager.h"
#include <assert.h>
#include "Renderer.h"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

static void handleErrors(GLenum source, GLenum type, GLuint id,
						 GLenum severity, GLsizei length, const GLchar* message, const void* userParam);


SceneManager::SceneManager(uint32_t width, uint32_t height)
	: camera(*this, width, height)
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
		onUI();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void SceneManager::onCreate()
{
	camera.setRotation({ 30.0f, 30.0f, 0 });
	camera.setPosition({ -3, 4, 10 });
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

	// Init ImGui
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		const char* glsl_version = "#version 130";
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
}

void SceneManager::onUpdate(float dt)
{
	lastDt = dt;
	camera.onUpdate(dt);
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
	ImGui::Text("You can use these controls or the Key shortcuts");

	// Draw sections
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap
		| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding
		| ImGuiTreeNodeFlags_AllowItemOverlap;

	// Olaf settings
	bool openOlaf = ImGui::TreeNodeEx((void*)typeid(Olaf).hash_code(), treeNodeFlags, "Olaf settings");
	if (openOlaf) {
		ImGui::DragFloat3("position (Shift + w,a,s,d) ", (float*)&olaf.position);
		ImGui::DragFloat3("rotation (a, d): ", (float*)&olaf.rotation);
		ImGui::DragFloat("scale: (u, j)", &olaf.scale);
		
		if (ImGui::Button("random position"))
			olaf.randomPosition();

		ImGui::TreePop();
	}

	// Camera settings
	bool openCam = ImGui::TreeNodeEx((void*)typeid(Camera).hash_code(), treeNodeFlags, "Camera settings");
	if (openCam) {
		if (ImGui::DragFloat3("position ", (float*)&camera.position))
			camera.recalculateMatrix();

		if (ImGui::DragFloat3("rotation (arrow keys)", (float*)&camera.rotation))
			camera.recalculateMatrix();
		ImGui::TreePop();
	}

	// Grid size
	bool openGrid = ImGui::TreeNodeEx((void*)typeid(Renderer).hash_code(), treeNodeFlags, "Grid settings");
	if (openGrid) {
		ImGui::DragInt("Grid count ", &Renderer::GridSize);

		// Select menu to change Rendering mode
		const char* types[] = { "Triangles", "Lines", "Points"};
		int index[] = {GL_TRIANGLES, GL_LINE_LOOP, GL_POINTS};
		int currentIndex = Renderer::getRenderingMode() == GL_TRIANGLES ? 0 : Renderer::getRenderingMode() == GL_LINE_LOOP ? 1 : 2;

		const char* currentType = types[currentIndex];
		if (ImGui::BeginCombo("Rendering mode", currentType)) {

			for (int i = 0; i < 3; i++) {
				bool isSelected = currentType == types[i];

				if (ImGui::Selectable(types[i], isSelected)) {
					currentType = types[i];

					// Change camera type
					Renderer::setDefaultRenderering(index[i]);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::TreePop();
	}
	
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

	delete shader;
	delete (GLFWwindow*)glfwGetWindowUserPointer(window);
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

