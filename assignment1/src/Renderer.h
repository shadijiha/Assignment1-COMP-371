#pragma once
#include <Camera.h>
#include <Shader.h>

struct VertexArraysInfo {
	uint32_t rendererID;
	uint32_t size;
	uint32_t count;

	uint32_t indexCount;
};

class Renderer
{
public:
	static void setCamera(Camera* camera);
	static void setDefaultShader(Shader* shader);

	static void drawCube(const glm::vec3& pos = { 0, 0, 0 },
						 const glm::vec3& rot = {0, 0, 0},
						 const glm::vec3& scale = { 1, 1, 1 }, 
						 const glm::vec4& color = {1, 1, 1, 1},
						 Shader& shader = *Renderer::shader,
						int mode = 0x0004);

	static void drawGrid();

private:
	static VertexArraysInfo init();
	inline static Camera* camera = nullptr;
	inline static Shader* shader = nullptr;		// Default shader
};

