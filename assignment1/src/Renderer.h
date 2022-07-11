#pragma once
#include <Camera.h>
#include <Shader.h>
#include "util/Texture.h"
#include "util/VertexArray.h"

struct Light;

// Unsed to store RendererIDs in Renderer class
struct RendererInfo {
	std::shared_ptr<VertexArray> cube_vao;

	uint32_t skybox_Text_RendererID;
	uint32_t skybox_VAO_RendererID;
};

class Renderer
{
public:
	struct RotationInfo {
		glm::vec3 rotation;
		glm::vec3 origin;

		RotationInfo(glm::vec3 rotation, glm::vec3 origin): rotation(rotation), origin(origin) {}
	};

	static void setCamera(std::shared_ptr<Camera> camera);
	static void setDefaultShader(std::shared_ptr<Shader> shader);
	static void setDefaultRenderering(int mode);
	static void setLight(std::shared_ptr<Light> light);

	/**
	 * \brief Draw a cube at selected location, rotation, scale, colour
	 * \param pos Object Position
	 * \param rot Object Rotation
	 * \param scale Object scale
	 * \param color Object colour
	 * \param shader Loaded shader
	 * \param mode OpenGL rendering mode (triangles, lines, etc.)
	 */
	static void drawCube(const glm::vec3& pos = ZERO,
						 const glm::vec3& rot = ZERO,
						 const glm::vec3& scale = ONE,
						 const glm::vec4& color = WHITE,
						 Shader& shader = *Renderer::shader,
						 int mode = renderingMode,
						 Texture& texture = *Renderer::whiteTexture);

	/**
	 * \brief Draws cube at selected cube. You can specify the rotaion Origin with this function
	 * \param rotationData Rotaion angles and origine
	 */
	static void drawCube(const glm::vec3& pos = ZERO,
						 const RotationInfo& rotationData = { ZERO , ZERO },
						 const glm::vec3& scale = ONE,
						 const glm::vec4& color = WHITE,
						 Shader& shader = *Renderer::shader,
						 int mode = renderingMode);

	/**
	 * \brief Draw cube using transform matrix
	 * \param transform 
	 */
	static void drawCube(const glm::mat4& transform,
						 const glm::vec4& color = WHITE,
						 Shader& shader = *Renderer::shader,
						 int mode = renderingMode,
						 Texture& texture = *Renderer::whiteTexture);

	/**
	 * \brief Draws a grid depending on the Renderer::GridSize
	 */
	static void drawGrid();

	/**
	 * \brief Draws the Cube map skybox
	 */
	static void drawSkyBox();

	/**
	 * \brief Intializes the renderer. This function should be called once OpenGL contex is ready
	 */
	static void init();

	/**
	 * \return current rendering mode (Triangles, Lines, etc.)
	 */
	inline static int getRenderingMode() { return renderingMode; }

private:
	/**
	 * \brief Initialize everything used for the skybox
	 */
	static void initCubeMap();

private:
	inline static std::shared_ptr<Camera> camera = nullptr;
	inline static std::shared_ptr<Light> light = nullptr;
	inline static std::shared_ptr<Shader> shader = nullptr;		// Default shader
	inline static std::shared_ptr<Shader> skyboxShader = nullptr;		// Skybox shader
	inline static std::shared_ptr<Texture> whiteTexture = nullptr;
	inline static int renderingMode = 0x0004;
	inline static RendererInfo info;

	inline static glm::vec3 ZERO = glm::vec3(0);
	inline static glm::vec3 ONE = glm::vec3(1);
	inline static glm::vec4 WHITE = glm::vec4(1);
public:
	inline static int GridSize = 100;
};

