#pragma once
#include <string>

	enum class TextureType {
		Texture2D, CubeMap
	};

	enum class TextureWrap {
		Repeat, ClampToEdge
	};

	class Texture {
	public:
		Texture(uint32_t width, uint32_t height, TextureType type = TextureType::Texture2D, TextureWrap wrap = TextureWrap::Repeat);
		Texture(const std::string& path, TextureType type = TextureType::Texture2D, TextureWrap wrap = TextureWrap::Repeat);
		~Texture();

		void setData(void* data, uint32_t size);

		void bind(uint32_t slot = 0) const;
		void unbind() const;

		bool isLoaded() const { return m_IsLoaded; }

		int getWidth() const { return m_Width; }
		int getHeight() const { return m_Height; }
		uint32_t getRendererID() const { return m_RendererID; }
		std::string getFilePath() const { return m_FilePath; }

		bool operator==(const Texture& other) const
		{
			return m_RendererID == ((Texture&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID;
		uint32_t m_Width, m_Height;

		unsigned int m_InternalFormat;
		unsigned int m_DataFormat;

		std::string m_FilePath;
		bool m_IsLoaded = false;
	};
