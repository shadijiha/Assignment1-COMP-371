#include "Sphere.h"

#include <iostream>
#include <glm/gtx/transform.hpp>
#include "Camera.h"
#include "Renderer.h"
#include "GL/glew.h"

#define PI 3.14159265359

static double map(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

Sphere::Sphere(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color)
	: position(position), rotation(rotation), scale(scale), color(color)
{
	static int v = init();
	rotationOriginPtr = &this->position;
}

void Sphere::onDraw(float dt) {
	assert(this->shader != nullptr, "You need to set a shader");
	assert(this->vao != nullptr, "You need to call init at least once");

	bool isTriangles = Renderer::getRenderingMode() == RenderingMode::Triangles;

	glm::mat4 rot = glm::translate(glm::mat4(1.0), position)
				* glm::rotate(glm::mat4(1.0), glm::radians(90.0f), { 1, 0, 0 })
				* glm::translate(glm::mat4(1.0), -position);

	glm::mat4 transform = rot * glm::translate(glm::mat4(1.0f), *rotationOriginPtr)
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.x), { 1, 0, 0 })
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.y), { 0, 1, 0 })
		* glm::rotate(glm::mat4(1.0), glm::radians(rotation.z), { 0, 0, 1 })
		* glm::translate(glm::mat4(1.0f), -*rotationOriginPtr)
		* glm::translate(glm::mat4(1.0), position)
		* glm::scale(scale);

	Renderer::getCubeMapTexture()->bind(Renderer::getInfo().skybox_Text_slot);

	shader->bind();
	
	shader->setFloat3("u_Camera.position", camera->getPosition());
	shader->setMat4("u_Camera.viewProjection", camera->getViewProjection());

	shader->setMat4("u_Transform", transform);

	shader->setFloat3("u_Light.position", light->position);
	shader->setFloat4("u_Light.color", light->color);
	shader->setFloat("u_Light.ambientStrength", light->ambientStrength);

	shader->setFloat4("u_Material.color", color);
	shader->setFloat("u_Material.shininess", isTriangles ? 1.0f : 0.0f);

	shader->setInt("ourTexture", 0);
	shader->setInt("u_CubeMap", Renderer::getInfo().skybox_Text_slot);

	

	if (Renderer::textures && texture != nullptr)
		texture->bind();
	else
		Renderer::whiteTexture->bind();

	

	vao->bind();

	
	glDrawArrays(isTriangles ? GL_TRIANGLE_STRIP : Renderer::getGLRenderingMode(), 0, vao->getCount());

	//shader->setFloat4("u_Color", {0, 0, 0, 1});
	//glDrawArrays(GL_LINE_LOOP, 0, vao->getCount());
}

int Sphere::init() {

	std::vector<float> vertices;

	constexpr int resolution = 500;
	constexpr float r = 0.6f;
	for (int i = 0; i <= resolution; i++) {
		double lon = map(i, 0, resolution, -PI, PI);
		double lon1 = map(i + 1, 0, resolution, -PI, PI);

		for(int j = 0; j <= resolution; j++) {
			double lat = map(j, 0, resolution, -PI * 2, PI * 2);

			double x = r * sin(lat) * cos(lon);
			double y = r * sin(lat) * sin(lon);
			double z = r * cos(lat);

			double x2 = r * sin(lat) * cos(lon1);
			double y2 = r * sin(lat) * sin(lon1);
			double z2 = r * cos(lat);

			// First vertex
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// Normal
			glm::vec3 normal = -glm::normalize(glm::cross(glm::vec3{ x, y, z }, {x2, y2, z2}));

			//Texture UV
			float u = asin(normal.x) / PI + 0.5;
			float v = asin(normal.y) / PI + 0.5;
			glm::vec2 uv = { u , v };


			//std::cout << "u " << u << ", " << "v " << v << std::endl;

			// Normal for second vertex
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);

			// Texture coords for 1st vertex
			vertices.push_back(uv.x);
			vertices.push_back(uv.y);

			// Second vertex
			vertices.push_back(x2);
			vertices.push_back(y2);
			vertices.push_back(z2);


			// Normal for second vertex
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);

			// Texture coords for 2st vertex
			vertices.push_back(uv.x);
			vertices.push_back(uv.y);
		}
	}

	vao = VertexArray::create();

	std::shared_ptr<VertexBuffer> buffer = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
	buffer->setLayout({
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float3, "a_Normal"},
		{ShaderDataType::Float2, "a_TexCoord"}
	});

	vao->addVertexBuffer(buffer);
	vao->setCount(vertices.size() / 3);

	return 0;
}
