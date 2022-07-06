#include "Renderer.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

void Renderer::setCamera(Camera* camera)
{
	Renderer::camera = camera;
}

void Renderer::setDefaultShader(Shader* shader)
{
	Renderer::shader = shader;
}

void Renderer::drawCube(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale, const glm::vec4& color, Shader& shader, int mode)
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

	/* Push each element in buffer_vertices to the vertex shader */
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayInfo.rendererID);

	glDrawElements(mode, vertexArrayInfo.indexCount, GL_UNSIGNED_INT, nullptr);
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

	for (int i = 0; i < 10; i++) {

		Renderer::drawCube({ i, 0, 0, }, { 0, 0, 0 }, { 1, 1, 1 }, { 1, 1, 1, 1 }, *Renderer::shader, GL_LINE_LOOP);
	}

}

VertexArraysInfo Renderer::init()
{
	
	GLfloat verticies[] = {
		// front
		-1.0, -1.0,  1.0, //0.0f,  0.0f,  1.0f,
		 1.0, -1.0,  1.0, //0.0f,  0.0f,  1.0f,
		 1.0,  1.0,  1.0, //0.0f,  0.0f,  1.0f,
		-1.0,  1.0,  1.0, //0.0f,  0.0f,  1.0f
		// back
		-1.0, -1.0, -1.0, //0.0f,  0.0f, -1.0f,
		 1.0, -1.0, -1.0, //0.0f,  0.0f, -1.0f,
		 1.0,  1.0, -1.0, //0.0f,  0.0f, -1.0f,
		-1.0,  1.0, -1.0, //0.0f,  0.0f, -1.0f

		/*-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		 -0.5f, 0.5f, 0.0f*/
	};

	GLuint vbo_cube_vertices;
	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3 /*6*/, 0);

	// normal attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	GLuint cube_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	VertexArraysInfo info;
	info.size = sizeof(verticies);
	info.rendererID = vbo_cube_vertices;
	info.count = sizeof(verticies) / sizeof(GLfloat) / 3;
	info.indexCount = sizeof(cube_indices) / sizeof(GLuint);
	return info;
}
