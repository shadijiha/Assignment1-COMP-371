#include "Renderer.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "Light.h"
#include "stb_image/stb_image.h"
#include "util/Texture.h"

void Renderer::setCamera(std::shared_ptr<Camera> camera)
{
	Renderer::camera = camera;
}

void Renderer::setDefaultShader(std::shared_ptr<Shader> shader)
{
	Renderer::shader = shader;
}

void Renderer::setDefaultRenderering(RenderingMode mode)
{
	Renderer::renderingMode = mode;
}

void Renderer::setLight(std::shared_ptr<Light> light) {
	Renderer::light = light;
}

void Renderer::drawCube(const RenderingOptions& options) {

	auto rotation = options.rotation;
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), options.rotationOrigin)
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.x), { 1, 0, 0 })
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.y), { 0, 1, 0 })
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.z), { 0, 0, 1 })
		* glm::translate(glm::mat4(1.0f), -options.rotationOrigin)
		* glm::translate(glm::mat4(1.0), options.position)
		* glm::scale(options.scale);

	drawCube(transform, options.color, *shader, options.mode, *options.texture);
}

void Renderer::drawCube(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& rotOrigine,
		const glm::vec3& scale, const glm::vec4& color)
{
	RenderingOptions options;
	options.position = pos;
	options.rotation = rot;
	options.rotationOrigin = rotOrigine;
	options.color = color;
	options.scale = scale;
	drawCube(options);
}

void Renderer::drawCube(const glm::mat4& transform, const glm::vec4& color, Shader& shader, RenderingMode mode, Texture& texture) {
	shader.bind();
	
	shader.setFloat3("u_Camera.position", camera->getPosition());
	shader.setMat4("u_Camera.viewProjection", camera->getViewProjection());

	shader.setMat4("u_Transform", transform);
	shader.setFloat3("u_LightPos", light->position);

	shader.setFloat3("u_Light.position", light->position);

	shader.setFloat3("u_Light.C_d", light->C_d);
	shader.setFloat3("u_Light.C_a", light->C_a);
	shader.setFloat3("u_Light.C_l", light->C_l);

	shader.setFloat("u_Light.k_d", light->k_d);
	shader.setFloat("u_Light.k_a", light->k_a);
	shader.setFloat("u_Light.k_s", light->k_s);
	shader.setFloat("u_Light.s", light->s);

	shader.setInt("ourTexture", 0);

	shader.setFloat4("u_Material.color", color);
	shader.setFloat("u_Material.shininess", 0);

	if (textures)
		texture.bind();
	else
		Renderer::whiteTexture->bind();

	info.cube_vao->bind();
	glDrawArrays(getGLRenderingMode(mode), 0, info.cube_vao->getCount());
}

void Renderer::drawGrid()
{
	static std::shared_ptr<Texture> snow = std::make_shared<Texture>("shaders/snow.png");

	Renderer::info.ground_Texture = snow;

	// Draw x y yellow grid
	constexpr float gridDim = 1;
	const int countPerAxis = GridSize;

	if (textures) {
		RenderingOptions options;
		options.color = { 1, 1, 1, 1 };
		options.mode = RenderingMode::Triangles;
		options.position = { 0, 0, 0 };
		options.scale = { gridDim * GridSize, 0.01, gridDim * GridSize };
		options.texture = snow;
		Renderer::drawCube(options);
	}
	else
	{
		for (int i = -countPerAxis / 2; i <= countPerAxis / 2; i++) {
			for (int j = -countPerAxis / 2; j <= countPerAxis / 2; j++) {
				RenderingOptions options;
				options.color = glm::vec4{ 1, 1, 0, 1 };
				options.mode = RenderingMode::Lines;
				options.position = { i, 0, j };
				options.scale = { gridDim, 0.01, gridDim };
				options.texture = snow;

				Renderer::drawCube(options);
			}
		}
	}

	// Draw axis lines
	constexpr float lineLength = 5 * gridDim;
	Renderer::drawCube({ lineLength / 2, 0, 0 }, { 0, 0, 0 }, {0, 0, 0}, { lineLength, 0.02, 0.02 }, { 1, 0, 0, 1 });
	Renderer::drawCube({ 0, lineLength / 2, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0.02, lineLength, 0.02 }, { 0, 1, 0, 1 });
	Renderer::drawCube({ 0, 0, lineLength / 2}, { 0, 0, 0 }, { 0, 0, 0 } , { 0.02, 0.02, lineLength }, { 0, 0, 1, 1 });

	// Draw debug light cube
	Renderer::drawCube(light->position, ZERO, ZERO, ONE * 0.5f, {0.7, 1.0, 0.3, 1.0});
}

void Renderer::drawSkyBox() {
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->bind();

	const auto&  view = glm::mat4(glm::mat3(camera->getView())); // remove translation from the view matrix
	skyboxShader->setMat4("view", view);
	skyboxShader->setMat4("proj", camera->getProjection());
	skyboxShader->setInt("skybox", 0);

	info.cube_vao->bind();
	info.skybox_Texture->bind(info.skybox_Text_slot);

	glDrawArrays(GL_TRIANGLES, 0, info.cube_vao->getCount());
	info.cube_vao->unBind();
	glDepthFunc(GL_LESS); // set depth function back to default
}

int Renderer::getGLRenderingMode(RenderingMode mode) {
	mode = mode == RenderingMode::Default ? Renderer::renderingMode : mode;

	switch (mode)
	{
		[[Likely]]
		case RenderingMode::Triangles:
			return GL_TRIANGLES;
		case RenderingMode::LineLoop:
			return GL_LINE_LOOP;
		case RenderingMode::Lines:
			return GL_LINES;
		case RenderingMode::Points:
			return GL_POINTS;
		case RenderingMode::TriangleStrip:
			return GL_TRIANGLE_STRIP;
	}
	return GL_TRIANGLES;
}

/**
 * *****************************
 * ******** Init Stuff *********
 * *****************************
 */
void Renderer::init()
{
	RendererInfo i;
	Renderer::info = i;

	uint32_t whiteTextureData = 0xffffffff;
	whiteTexture = std::make_shared<Texture>(1, 1);
	whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

	//******************** CUBE Stuff ********************
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	// first, configure the cube's VAO (and VBO)
	info.cube_vao = std::make_shared<VertexArray>();
	info.cube_vao->setCount(36);

	std::shared_ptr<VertexBuffer> buffer = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
	buffer->setLayout({
		{ ShaderDataType::Float3, "a_Position"},
		{ ShaderDataType::Float3, "a_Normal"},
		{ ShaderDataType::Float2, "a_TexCoord"},
	});

	info.cube_vao->addVertexBuffer(buffer);	

	Renderer::initCubeMap();


	/**
	 * Initialize shadow texture
	 */
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	info.shadow_frameBuffer = depthMapFBO;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		info.shadow_width, info.shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColour[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	info.shadow_depth_map = depthMap;
}

void Renderer::initCubeMap() {
	std::vector<std::string> faces{
			"shaders/skybox/right.jpg",
			"shaders/skybox/left.jpg",
			"shaders/skybox/top.jpg",
			"shaders/skybox/bottom.jpg",
			"shaders/skybox/front.jpg",
			"shaders/skybox/back.jpg"
	};

	std::shared_ptr<Texture> texture = Texture::cubeMap(faces);
	
	skyboxShader = std::make_shared<Shader>("shaders/skyboxShader.glsl");

	// Init VAO skybox
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// skybox VAO
	std::shared_ptr<VertexArray> vao = VertexArray::create();

	auto buffer = std::make_shared<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices));
	buffer->setLayout({
		{ShaderDataType::Float3, "a_Position"}
	});

	vao->addVertexBuffer(buffer);
	vao->setCount(sizeof(skyboxVertices) / sizeof(float) / 3);

	Renderer::info.skybox_Texture = texture;
	Renderer::info.skybox_VAO = vao;
}
