#pragma once

#include <ui/Widget.hpp>
#include <graphics/VertexArray.hpp>
#include <graphics/Vertex.hpp>

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

private:
	VertexBuffer<Vertex2D> m_VBO;
	VertexArray m_VAO;
	ElementBuffer m_EBO;

	void createMesh();
};