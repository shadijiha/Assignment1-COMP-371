#include "Renderer.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "Light.h"
#include "stb_image/stb_image.h"

void Renderer::setCamera(Camera* camera)
{
	Renderer::camera = camera;
}

void Renderer::setDefaultShader(Shader* shader)
{
	Renderer::shader = shader;
}

void Renderer::setDefaultRenderering(int mode)
{
	Renderer::renderingMode = mode;
}

void Renderer::setLight(Light* light) {
	Renderer::light = light;
}

void Renderer::drawCube(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale, const glm::vec4& color, Shader& shader, int mode, Texture& texture)
{
	glm::mat4 transform = glm::mat4(1.0) * glm::translate(glm::mat4(1.0), pos) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.x), { 1, 0, 0 }) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.y), { 0, 1, 0 }) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.z), { 0, 0, 1 })	*
		glm::scale(glm::mat4(1.0), scale);

	texture.bind();
	drawCube(transform, color, shader, mode);
}

void Renderer::drawCube(const glm::vec3& pos, const RotationInfo& rotationData, const glm::vec3& scale,
	const glm::vec4& color, Shader& shader, int mode) {

	auto rotation = rotationData.rotation;
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), rotationData.origin)
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.x), { 1, 0, 0 })
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.y), { 0, 1, 0 })
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.z), { 0, 0, 1 })
		* glm::translate(glm::mat4(1.0f), -rotationData.origin)
		* glm::translate(glm::mat4(1.0), pos)
		* glm::scale(scale);

	drawCube(transform, color, shader, mode);
}

void Renderer::drawCube(const glm::mat4& transform, const glm::vec4& color, Shader& shader, int mode) {
	shader.bind();
	shader.setFloat4("u_Color", color);
	shader.setMat4("u_ViewProjection", camera->getViewProjection());
	shader.setMat4("u_Transform", transform);

	shader.setFloat3("u_LightPosition", light->position);
	shader.setFloat4("u_LightColor", light->color);
	shader.setFloat("u_AmbientStrength", light->ambientStrength);
	shader.setInt("ourTexture", 0);

	/* Push each element in buffer_vertices to the vertex shader */
	glBindVertexArray(Renderer::info.cube_rendererID);
	glDrawArrays(mode, 0, Renderer::info.cube_count);
}

void Renderer::drawGrid()
{
	// Draw x y yellow grid
	constexpr float gridDim = 1;
	const int countPerAxis = GridSize;
	for (int i = -countPerAxis/2; i < countPerAxis/2; i++) {
		for(int j = -countPerAxis / 2; j < countPerAxis / 2; j++)
			Renderer::drawCube({ i, 0, j}, { 0, 0, 0 }, { gridDim, 0.01, gridDim }, {1, 1, 0, 1}, *Renderer::shader, GL_LINES);
	}


	// Draw axis lines
	constexpr float lineLength = 5 * gridDim;
	Renderer::drawCube({ lineLength / 2, 0, 0 }, { 0, 0, 0 }, { lineLength, 0.02, 0.02 }, { 1, 0, 0, 1 });
	Renderer::drawCube({ 0, lineLength / 2, 0 }, { 0, 0, 0 }, { 0.02, lineLength, 0.02 }, { 0, 1, 0, 1 });
	Renderer::drawCube({ 0, 0, lineLength / 2}, { 0, 0, 0 }, { 0.02, 0.02, lineLength }, { 0, 0, 1, 1 });

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
	uint32_t whiteTextureData = 0xffffffff;
	whiteTexture = new Texture(1, 1);
	whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

	//******************** CUBE Stuff ********************
	float vertices[] = {
		// Position			  // Normals			// Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	 1.0f, 1.0f

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	 1.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	 0.0f, 1.0f, 
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	 1.0f, 1.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	 1.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	 1.0f, 0.0f,
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture coords attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(2);

	RendererInfo info;
	info.cube_size = sizeof(vertices);
	info.cube_rendererID = cubeVAO;
	info.cube_count = sizeof(vertices) / sizeof(GLfloat) / 6;
	info.cube_indexCount = -1;//sizeof(cube_indices) / sizeof(GLuint);


	Renderer::info = info;

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


	skyboxShader = new Shader("shaders/skyboxShader.glsl");

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
