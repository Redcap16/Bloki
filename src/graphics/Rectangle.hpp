#pragma once

#include <graphics/Vertex.hpp>
#include <graphics/VertexArray.hpp>

//TODO: Add namespace to other files
namespace graphics
{
	class Rectangle
	{
	public:
		Rectangle(glm::ivec2 position, glm::ivec2 size, glm::vec2 textureCoords, glm::vec2 textureSize, glm::ivec3 color = { 255, 255, 255 });
		Rectangle(glm::ivec2 position, glm::ivec2 size, glm::ivec3 color = { 255, 255, 255 });
		~Rectangle() = default;
		Rectangle(const Rectangle&) = delete;
		Rectangle& operator=(const Rectangle&) = delete;
		Rectangle(Rectangle&& other) noexcept = default;
		Rectangle& operator=(Rectangle&& other) noexcept = default;

		void SetPosition(glm::ivec2 position);
		void SetSize(glm::ivec2 size);
		void SetTextureCoords(glm::vec2 textureCoords, glm::vec2 textureSize);
		void SetColor(glm::ivec3 color);

		void Render() const { m_VAO->Draw(); };
	private:
		std::shared_ptr<VertexBuffer<Vertex2D>> m_VBO;
		std::shared_ptr<ElementBuffer> m_EBO;
		std::shared_ptr<VertexArray> m_VAO;

		glm::ivec2 m_Position,
			m_Size;
		glm::vec2 m_TextureCoords,
			m_TextureSize;

		glm::ivec3 m_Color;

		void update();
	};
}