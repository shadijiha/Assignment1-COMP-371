#include "Buffer.h"
#include "GL/glew.h"

	VertexBuffer::VertexBuffer(uint32_t size) {
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::VertexBuffer(float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	void VertexBuffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void VertexBuffer::unBind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size) {
		return std::make_shared<VertexBuffer>(vertices, size);
	}

	void VertexBuffer::setData(const void* data, size_t size) {
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::create(uint32_t size) {
		return std::make_shared<VertexBuffer>(size);
	}

	// ========================================
	std::shared_ptr<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count) {
		return std::make_shared<IndexBuffer>(indices, count);
	}

	IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
		:m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);

		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndexBuffer::bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::unBind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
