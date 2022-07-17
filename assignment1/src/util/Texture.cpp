#include "Texture.h"
#include <assert.h>
#include <iostream>

#include "stb_image/stb_image.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>

	static int openGLType(TextureType type) {
		switch (type) {
			case TextureType::Texture2D:
				return GL_TEXTURE_2D;
			case TextureType::CubeMap:
				return GL_TEXTURE_CUBE_MAP;
			default:
				assert(false, "Invalid type");
				return -1;
		}
	}

	static int openGLWrap(TextureWrap type) {
		switch (type) {
		case TextureWrap::Repeat:
			return GL_REPEAT;
		case TextureWrap::ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		default:
			assert(false, "Invalid wrap type");
			return -1;
		}
	}
	
	Texture::Texture(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) {
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(openGLType(TextureType::Texture2D), 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, openGLWrap(TextureWrap::Repeat));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, openGLWrap(TextureWrap::Repeat));
	}

	Texture::Texture(const std::string& path)
		: m_RendererID(0), m_Width(0), m_Height(0), m_FilePath(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		
		stbi_uc* data = nullptr;
		{
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		
		if (data) {
			m_IsLoaded = true;
			m_Width = width;
			m_Height = height;

			GLenum internalFormat = 0, dataFormat = 0;
			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			} else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			m_InternalFormat = internalFormat;
			m_DataFormat = dataFormat;

			assert(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(openGLType(TextureType::Texture2D), 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, openGLWrap(TextureWrap::Repeat));
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, openGLWrap(TextureWrap::Repeat));
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, openGLWrap(TextureWrap::Repeat));

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
	}

	Texture::~Texture() {
		glDeleteTextures(1, &m_RendererID);
	}

	std::shared_ptr<Texture> Texture::cubeMap(const std::vector<std::string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			} else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->m_RendererID = textureID;
		texture->m_Width = width;
		texture->m_Height = height;

		if (nrChannels == 4)
		{
			texture->m_InternalFormat = GL_RGBA8;
			texture->m_DataFormat = GL_RGBA;
		} else if (nrChannels == 3)
		{
			texture->m_InternalFormat = GL_RGB8;
			texture->m_DataFormat = GL_RGB;
		}

		texture->m_IsLoaded = true;

		return texture;
	}

	void Texture::setData(void* data, uint32_t size) {
			uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
			assert(size == m_Width * m_Height * bpp, "Data must be entire texture!");
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
		}

	void Texture::bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_RendererID);
	}

	void Texture::unbind() const {
		//glCall(glBindTexture(GL_TEXTURE_2D, 0));
	}