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
	shader.setFloat4("u_Color", color);
	shader.setMat4("u_ViewProjection", camera->getViewProjection());
	shader.setMat4("u_Transform", transform);

	shader.setFloat3("u_LightPosition", light->position);
	shader.setFloat4("u_LightColor", light->color);
	shader.setFloat("u_AmbientStrength", light->ambientStrength);
	shader.setInt("ourTexture", 0);

	/* Push each element in buffer_vertices to the vertex shader */
	if (textures)
		texture.bind();
	else
		Renderer::whiteTexture->bind();


	int m = GL_TRIANGLES;
	mode = mode == RenderingMode::Default ? Renderer::renderingMode : mode;
	switch (mode)
	{
		[[Likely]]
		case RenderingMode::Triangles:
			m = GL_TRIANGLES;
			break;
		case RenderingMode::LineLoop:
			m = GL_LINE_LOOP;
			break;
		case RenderingMode::Lines:
			m = GL_LINES;
			break;
		case RenderingMode::Points:
			m = GL_POINTS;
			break;	
	}

	info.cube_vao->bind();
	glDrawArrays(m, 0, info.cube_vao->getCount());
}

void Renderer::drawGrid()
{
	static std::shared_ptr<Texture> snow = std::make_shared<Texture>("shaders/snow.png");

	// Draw x y yellow grid
	constexpr float gridDim = 1;
	const int countPerAxis = GridSize;
	for (int i = -countPerAxis/2; i <= countPerAxis/2; i++) {
		for(int j = -countPerAxis / 2; j <= countPerAxis / 2; j++) {
			RenderingMode mode = textures ? RenderingMode::Triangles : RenderingMode::Lines;
			glm::vec4 color = textures ? glm::vec4{ 1, 1, 1, 1 } : glm::vec4{ 1, 1, 0, 1 };

			RenderingOptions options;
			options.color = color;
			options.mode = mode;
			options.position = { i, 0, j };
			options.scale = { gridDim, 0.01, gridDim };
			options.texture = snow;
			
			Renderer::drawCube(options);
		}
			
	}


	// Draw axis lines
	constexpr float lineLength = 5 * gridDim;
	Renderer::drawCube({ lineLength / 2, 0, 0 }, { 0, 0, 0 }, {0, 0, 0}, { lineLength, 0.02, 0.02 }, { 1, 0, 0, 1 });
	Renderer::drawCube({ 0, lineLength / 2, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0.02, lineLength, 0.02 }, { 0, 1, 0, 1 });
	Renderer::drawCube({ 0, 0, lineLength / 2}, { 0, 0, 0 }, { 0, 0, 0 } , { 0.02, 0.02, lineLength }, { 0, 0, 1, 1 });

}

void Renderer::drawSkyBox() {
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->bind();

	const auto&  view = glm::mat4(glm::mat3(camera->getView())); // remove translation from the view matrix
	skyboxShader->setMat4("view", view);
	skyboxShader->setMat4("proj", camera->getProjection());
	skyboxShader->setInt("skybox", 0);

	glBindVertexArray(info.skybox_VAO_RendererID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, info.skybox_Text_RendererID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
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

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		} else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


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
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	Renderer::info.skybox_Text_RendererID = textureID;
	Renderer::info.skybox_VAO_RendererID = skyboxVAO;
}
