#pragma once

#include <engine/graphics/Vertex.hpp>
#include <engine/graphics/VertexArray.hpp>

//TODO: Add namespace to other files
namespace graphics
{
	class Rectangle
	{
	public:
		Rectangle(glm::ivec2 position, glm::ivec2 size, glm::vec2 textureCoords, glm::vec2 textureSize, glm::ivec4 color = { 255, 255, 255, 255 });
		Rectangle(glm::ivec2 position, glm::ivec2 size, glm::ivec4 color = { 255, 255, 255, 255 });
		~Rectangle() = default;
		Rectangle(const Rectangle&) = delete;
		Rectangle& operator=(const Rectangle&) = delete;
		Rectangle(Rectangle&& other) noexcept = default;
		Rectangle& operator=(Rectangle&& other) noexcept = default;

		void SetPosition(glm::ivec2 position);
		void SetSize(glm::ivec2 size);
		void SetTextureCoords(glm::vec2 textureCoords, glm::vec2 textureSize);
		void SetColor(glm::ivec4 color);
		void SetColor(glm::ivec3 color) { SetColor(glm::ivec4(color, 255)); };

		void Render() const { m_VAO.Draw(); };
	private:
		VertexArray m_VAO;
		VertexBuffer<Vertex2D>* m_VBO;
		ElementBuffer* m_EBO;

		glm::ivec2 m_Position,
			m_Size;
		glm::vec2 m_TextureCoords,
			m_TextureSize;

		glm::ivec4 m_Color;

		void update();
	};
}