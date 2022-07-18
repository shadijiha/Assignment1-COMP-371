#pragma once
#include <Camera.h>
#include <Shader.h>
#include "util/Texture.h"
#include "util/VertexArray.h"

struct Light;

// Unsed to store RendererIDs in Renderer class
struct RendererInfo {
	std::shared_ptr<VertexArray> cube_vao;

	std::shared_ptr<Texture> skybox_Texture;
	std::shared_ptr<VertexArray> skybox_VAO;
	uint32_t skybox_Text_slot = 5;

	uint32_t shadow_frameBuffer;
	uint32_t shadow_depth_map;
	uint32_t shadow_width = 1024, shadow_height = 1024;
	std::shared_ptr<Texture> ground_Texture = nullptr;
};

enum class RenderingMode {
	Triangles, LineLoop, Lines, Points, Default, TriangleStrip
};


struct RenderingOptions;
class Renderer
{
public:
	static void setCamera(std::shared_ptr<Camera> camera);
	static void setDefaultShader(std::shared_ptr<Shader> shader);
	static void setDefaultRenderering(RenderingMode mode);
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
	static void drawCube(const RenderingOptions& options);

	static void drawCube(const glm::vec3& pos,
		const glm::vec3& rot,
		const glm::vec3& rotOrigine,
		const glm::vec3& scale = glm::vec3(1.0f), const glm::vec4& color = glm::vec4(1.0f));

	/**
	 * \brief Draw cube using transform matrix
	 * \param transform 
	 */
	static void drawCube(const glm::mat4& transform,
						 const glm::vec4& color = glm::vec4(1),
						 Shader& shader = *Renderer::shader,
						 RenderingMode mode = renderingMode,
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
	inline static RenderingMode getRenderingMode() { return renderingMode; }

	/**
	 * Convert rendering mode enum to OpenGL enum
	 */
	static int getGLRenderingMode(RenderingMode mode = Renderer::renderingMode);

	static std::shared_ptr<Texture> getCubeMapTexture() { return Renderer::info.skybox_Texture; }

	static const RendererInfo& getInfo() { return info; }

private:
	/**
	 * \brief Initialize everything used for the skybox
	 */
	static void initCubeMap();

private:
	inline static std::shared_ptr<Shader> skyboxShader = nullptr;		// Skybox shader
	
	inline static RenderingMode renderingMode = RenderingMode::Triangles;
	inline static RendererInfo info;

	inline static glm::vec3 ZERO = glm::vec3(0);
	inline static glm::vec3 ONE = glm::vec3(1);
	inline static glm::vec4 WHITE = glm::vec4(1);
public:
	inline static std::shared_ptr<Texture> whiteTexture = nullptr;
	inline static std::shared_ptr<Camera> camera = nullptr;
	inline static std::shared_ptr<Light> light = nullptr;
	inline static std::shared_ptr<Shader> shader = nullptr;		// Default shader

	inline static bool textures = true;
	inline static int GridSize = 100;

	friend struct RenderingOptions;
};

struct RenderingOptions {
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
	glm::vec3 rotationOrigin = position;
	glm::vec4 color = glm::vec4(1);
	std::shared_ptr<Shader> shader = Renderer::shader;
	std::shared_ptr<Texture> texture = Renderer::whiteTexture;
	RenderingMode mode = RenderingMode::Default;
};
