#include "Renderer.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <iostream>

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

void Renderer::drawCube(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale, const glm::vec4& color, Shader& shader, int mode)
{
	glm::mat4 transform = glm::mat4(1.0) * glm::translate(glm::mat4(1.0), pos) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.x), { 1, 0, 0 }) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.y), { 0, 1, 0 }) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.z), { 0, 0, 1 })	*
		glm::scale(glm::mat4(1.0), scale);

	shader.bind();
	shader.setFloat4("u_Color", color);
	shader.setMat4("u_ViewProjection", camera->getViewProjection());
	shader.setMat4("u_Transform", transform);
	shader.setFloat3("u_LightPosition", camera->getPosition());
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

void Renderer::init()
{
	//******************** CUBE Stuff ********************
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	RendererInfo info;
	info.cube_size = sizeof(vertices);
	info.cube_rendererID = cubeVAO;
	info.cube_count = sizeof(vertices) / sizeof(GLfloat) / 6;
	info.cube_indexCount = -1;//sizeof(cube_indices) / sizeof(GLuint);


	Renderer::info = info;
}
