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
	struct RotationInfo {
		glm::vec3 rotation;
		glm::vec3 origin;

		RotationInfo(glm::vec3 rotation, glm::vec3 origin): rotation(rotation), origin(origin) {}
	};

	static void setCamera(Camera* camera);
	static void setDefaultShader(Shader* shader);
	static void setDefaultRenderering(int mode);

	static void drawCube(const glm::vec3& pos = ZERO,
						 const glm::vec3& rot = ZERO,
						 const glm::vec3& scale = ONE,
						 const glm::vec4& color = WHITE,
						 Shader& shader = *Renderer::shader,
						int mode = renderingMode);

	static void drawCube(const glm::vec3& pos = ZERO,
						const RotationInfo& rotationData = { ZERO , ZERO },
						const glm::vec3& scale = ONE,
						const glm::vec4& color = WHITE,
						Shader& shader = *Renderer::shader,
						int mode = renderingMode);

	static void drawCube(const glm::mat4& transform,
						const glm::vec4& color = WHITE,
						Shader& shader = *Renderer::shader,
						int mode = renderingMode);

	static void drawGrid();
	static void init();

	inline static int getRenderingMode() { return renderingMode; }

private:
	inline static Camera* camera = nullptr;
	inline static Shader* shader = nullptr;		// Default shader
	inline static int renderingMode = 0x0004;
	inline static RendererInfo info;

	inline static glm::vec3 ZERO = glm::vec3(0);
	inline static glm::vec3 ONE = glm::vec3(1);
	inline static glm::vec4 WHITE = glm::vec4(1);
public:
	inline static int GridSize = 100;
};

