#pragma once
#include <Camera.h>
#include <memory>
#include <Shader.h>

#include "util/VertexArray.h"

struct Light;

// Unsed to store RendererIDs in Renderer class
struct RendererInfo {
	uint32_t skybox_Text_RendererID;
	uint32_t skybox_VAO_RendererID;
};

class Renderer
{
public:
	static void setCamera(Camera* camera);
	static void setDefaultShader(Shader* shader);
	static void setDefaultRenderering(int mode);
	static void setLight(Light* light);

	static void drawArrays(std::shared_ptr<VertexArray> array,
		const glm::mat4& transform,
		const glm::vec4& color,
		std::shared_ptr<Shader> shader,
		int mode = renderingMode);

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
	inline static Camera* camera = nullptr;
	inline static Light* light = nullptr;
	inline static Shader* skyboxShader = nullptr;		// Skybox shader
	inline static int renderingMode = 0x0004;
	inline static RendererInfo info;
};

