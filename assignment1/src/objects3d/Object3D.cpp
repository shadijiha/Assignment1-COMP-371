#include "Object3D.h"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

Object3D::Object3D(const std::string& path)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE* file = fopen(path.c_str(), "r");
	if (file == NULL) {
		assert(false, "Impossible to open the file !\n");
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				assert(false, "File can't be read by our simple parser : ( Try exporting with other options\n");
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	// For each vertex of each triangle
	std::vector<glm::vec3> out_vertices;
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}

	std::vector<glm::vec3> out_normals;
	for (unsigned int i = 0; i < normalIndices.size(); i++) {
		unsigned int vertexIndex = normalIndices[i];
		glm::vec3 vertex = temp_normals[vertexIndex - 1];
		out_normals.push_back(vertex);
	}

	std::vector<glm::vec2> out_uv;
	for (unsigned int i = 0; i < uvIndices.size(); i++) {
		unsigned int vertexIndex = uvIndices[i];
		glm::vec vertex = temp_uvs[vertexIndex - 1];
		out_uv.push_back(vertex);
	}


	std::vector<float> merge;
	for (int i = 0; i < out_vertices.size(); i++) {
		merge.push_back(out_vertices[i].x);
		merge.push_back(out_vertices[i].y);
		merge.push_back(out_vertices[i].z);

		merge.push_back(out_normals[i].x);
		merge.push_back(out_normals[i].y);
		merge.push_back(out_normals[i].z);

		merge.push_back(out_normals[i].x);
		merge.push_back(out_uv[i].y);
	}

	vao = VertexArray::create();

	auto buffer = std::make_shared<VertexBuffer>(merge.data(), merge.size() * sizeof(float));
	buffer->setLayout({
		{ ShaderDataType::Float3, "a_Position"},
		{ ShaderDataType::Float3, "a_Normal"},
		{ ShaderDataType::Float2, "a_TexCoord"}
	});
	vao->addVertexBuffer(buffer);

	vao->setCount(out_normals.size());
}

void Object3D::onDraw(float dt)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), rotationOrigin)
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.x), { 1, 0, 0 })
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.y), { 0, 1, 0 })
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.z), { 0, 0, 1 })
		* glm::translate(glm::mat4(1.0f), -rotationOrigin)
		* glm::translate(glm::mat4(1.0), position)
		* glm::scale(scale);

	Renderer::getCubeMapTexture()->bind(Renderer::getInfo().skybox_Text_slot);

	shader->bind();

	shader->setFloat3("u_Camera.position", camera->getPosition());
	shader->setMat4("u_Camera.viewProjection", camera->getViewProjection());

	shader->setMat4("u_Transform", transform);
	shader->setFloat3("u_LightPos", light->position);

	shader->setFloat3("u_Light.position", light->position);
	shader->setFloat3("u_Light.C_d", light->C_d);
	shader->setFloat3("u_Light.C_a", light->C_a);
	shader->setFloat3("u_Light.C_l", light->C_l);
		  
	shader->setFloat("u_Light.k_d", light->k_d);
	shader->setFloat("u_Light.k_a", light->k_a);
	shader->setFloat("u_Light.k_s", light->k_s);
	shader->setFloat("u_Light.s", light->s);

	shader->setFloat4("u_Material.color", color);
	shader->setFloat("u_Material.shininess", Renderer::textures ? 1.0f : 0.0f);

	shader->setInt("ourTexture", 0);
	shader->setInt("u_CubeMap", Renderer::getInfo().skybox_Text_slot);

	Renderer::whiteTexture->bind();

	vao->bind();
	glDrawArrays(Renderer::getGLRenderingMode(), 0, vao->getCount());
}
