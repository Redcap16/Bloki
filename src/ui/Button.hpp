#pragma once

#include <ui/Widget.hpp>
#include <ui/Text.hpp>
#include <graphics/VertexArray.hpp>
#include <graphics/Vertex.hpp>
#include <graphics/ImageTexture.hpp>
#include <graphics/ResourceManager.hpp>

//TODO: Change ivec3 to vec<3, uint8_t>
class Button : public Widget
{
public:
	Button(WidgetParent& parent, glm::ivec2 position, glm::ivec2 size);
	~Button() = default;
	Button(const Button&) = delete;
	Button& operator=(const Button&) = delete;

	void Render(ShaderProgram& shader) override;

protected:
	void handleMouseEvent(const MouseEvent& event) override;
	virtual void pressed() = 0;
	virtual void released() = 0;

private:
	class ButtonMesh
	{
	public:
		ButtonMesh(glm::ivec2 size);
		
		void ChangeColor(glm::ivec3 color);
		void Render(ShaderProgram& shader);
	private:
		static const int c_BorderWidth = 10;
		static constexpr float c_BorderTextureWidth = 0.1f;

		glm::ivec2 m_Size;

		VertexBuffer<Vertex2D> m_BorderVBO;
		VertexArray m_BorderVAO;
		ElementBuffer m_BorderEBO;
		VertexBuffer<Vertex2D> m_CenterVBO;
		VertexArray m_CenterVAO;
		ElementBuffer m_CenterEBO;

		ImageTexture* m_Texture;

		void createMesh();
		void addRectangle(VertexBuffer<Vertex2D>& vbo, ElementBuffer& ebo, glm::ivec2 position, glm::ivec2 size, glm::vec2 texturePosition, glm::vec2 textureSize, glm::ivec3 color);
		void addVertex(VertexBuffer<Vertex2D>& vbo, glm::ivec2 position, glm::vec2 textureCoords, glm::ivec3 color);
	};

	static constexpr glm::ivec3 c_HighlightedColor = glm::ivec3(200, 200, 200);
	ButtonMesh m_Mesh;
	std::unique_ptr<Text> m_Text;
};