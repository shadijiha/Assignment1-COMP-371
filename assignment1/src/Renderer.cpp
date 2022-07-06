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

void Renderer::drawCube(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale, const glm::vec4& color, Shader& shader)
{
	static VertexArraysInfo vertexArrayInfo = init();

	glm::mat4 transform = glm::mat4(1.0) * glm::translate(glm::mat4(1.0), pos) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.x), { 1, 0, 0 }) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.y), { 0, 1, 0 }) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.z), { 0, 0, 1 })	*
		glm::scale(glm::mat4(1.0), scale);

	shader.bind();
	shader.setFloat4("u_Color", color);
	shader.setMat4("u_ViewProjection", camera->getViewProjection());
	shader.setMat4("u_Transform", transform);
	shader.setFloat3("u_LightPosition", { 1, 0, camera->getPosition().z });
	/* Push each element in buffer_vertices to the vertex shader */
	glBindVertexArray(vertexArrayInfo.rendererID);
	glDrawArrays(GL_TRIANGLES, 0, vertexArrayInfo.count);
}

void Renderer::drawGrid()
{
	// Draw x y yellow grid
	const int slices = 20;
	//for (int i = 0; i < slices; i++) {
	//	for (int j = 0; j < slices; j++) {
	//		float x = (float)i / (float)slices;
	//		float y = 0;
	//		float z = (float)j / (float)slices;
	//		Renderer::drawCube({ x, z, 1 }, { 0, 0, 0 }, { 0.001, 0.001, 0.001 }, {0, 1, 1, 1});
	//	}
	//}

	for (int i = 0; i < slices; i++) {
		float y = (i + 1) / (float)slices;
		Renderer::drawCube({ 0, y, 1 }, { 0, 0, 0 }, { 1, 0.0001, 0.0000001 }, { 0, 1, 1, 1 });
		Renderer::drawCube({ y, 0, 1 }, { 0, 0, 0 }, { 0.0001, 1, 0.0000001 }, { 0, 1, 1, 1 });
	}

}

VertexArraysInfo Renderer::init()
{
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

	VertexArraysInfo info;
	info.size = sizeof(vertices);
	info.rendererID = cubeVAO;
	info.count = sizeof(vertices) / sizeof(GLfloat) / 6;
	info.indexCount = -1;//sizeof(cube_indices) / sizeof(GLuint);
	return info;
}
