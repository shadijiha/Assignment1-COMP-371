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

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), *rotationOriginPtr)
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
	shader->setFloat("u_Material.shininess", shininess);

	shader->setInt("ourTexture", 0);
	shader->setInt("u_CubeMap", Renderer::getInfo().skybox_Text_slot);

	
	if (Renderer::textures && texture != nullptr)
		texture->bind();
	else
		Renderer::whiteTexture->bind();

	vao->bind();	


	glDrawElements(Renderer::getGLRenderingMode(), vao->getCount(), GL_UNSIGNED_INT, nullptr);
	//shader->setFloat4("u_Color", {0, 0, 0, 1});
	//glDrawArrays(GL_LINE_LOOP, 0, vao->getCount());
}

int Sphere::init() {

	constexpr float sectorCount = 100.0f, stackCount = 100.0f;
	constexpr float radius = 0.65f;

	std::vector<float> vertices;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			vertices.push_back(nx);
			vertices.push_back(ny);
			vertices.push_back(nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			vertices.push_back(s);
			vertices.push_back(t);
		}
	}

	// generate CCW index list of sphere triangles
	// k1--k1+1
	// |  / |
	// | /  |
	// k2--k2+1
	std::vector<uint32_t> indices;
	std::vector<uint32_t> lineIndices;
	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}

			// store indices for lines
			// vertical lines for all stacks, k1 => k2
			lineIndices.push_back(k1);
			lineIndices.push_back(k2);
			if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
			{
				lineIndices.push_back(k1);
				lineIndices.push_back(k1 + 1);
			}
		}
	}


	vao = VertexArray::create();
	std::shared_ptr<VertexBuffer> buffer = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
	buffer->setLayout({
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float3, "a_Normal"},
		{ShaderDataType::Float2, "a_TexCoord"}
	});
	std::shared_ptr<IndexBuffer> ibo = std::make_shared<IndexBuffer>(indices.data(), indices.size());

	vao->addVertexBuffer(buffer);
	vao->setIndexBuffer(ibo);
	return 0;
}
