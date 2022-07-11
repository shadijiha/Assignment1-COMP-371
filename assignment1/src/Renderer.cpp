#include "Renderer.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "Light.h"
#include "objects3d/Cube.h"
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

void Renderer::drawArrays(std::shared_ptr<VertexArray> array, const glm::mat4& transform, const glm::vec4& color,
	std::shared_ptr<Shader> shader, int mode) {

	shader->bind();
	shader->setFloat4("u_Color", color);
	shader->setMat4("u_ViewProjection", camera->getViewProjection());
	shader->setMat4("u_Transform", transform);
		  
	shader->setFloat3("u_LightPosition", light->position);
	shader->setFloat4("u_LightColor", light->color);
	shader->setFloat("u_AmbientStrength", light->ambientStrength);


	array->bind();
	glDrawArrays(mode, 0, array->getCount());
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
	Cube::initVAO();
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
