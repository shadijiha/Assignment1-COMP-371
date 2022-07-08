#pragma once
#include <Camera.h>
#include <Shader.h>

struct RendererInfo {
	uint32_t cube_rendererID;
	uint32_t cube_size;
	uint32_t cube_count;
	uint32_t cube_indexCount;

	uint32_t quad_rendererID;
	uint32_t quad_size;
	uint32_t quad_count;
	uint32_t quad_indexCount;
};


class Renderer
{
public:
	static void setCamera(Camera* camera);
	static void setDefaultShader(Shader* shader);
	static void setDefaultRenderering(int mode);

	static void drawCube(const glm::vec3& pos = { 0, 0, 0 },
						 const glm::vec3& rot = {0, 0, 0},
						 const glm::vec3& scale = { 1, 1, 1 }, 
						 const glm::vec4& color = {1, 1, 1, 1},
						 Shader& shader = *Renderer::shader,
						int mode = Renderer::renderingMode);

	static void drawGrid();
	static void init();

	inline static int getRenderingMode() { return renderingMode; }

private:
	inline static Camera* camera = nullptr;
	inline static Shader* shader = nullptr;		// Default shader
	inline static int renderingMode = 0x0004;
	inline static RendererInfo info;

public:
	inline static int GridSize = 100;
};

