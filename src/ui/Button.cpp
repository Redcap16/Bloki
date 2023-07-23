#include <ui/Button.hpp>

Button::Button(WidgetParent& parent, glm::ivec2 position, glm::ivec2 size) :
	Widget(parent, position, size),
	m_VBO(false),
	m_EBO(false)
{
	createMesh();
}

void Button::Render(ShaderProgram& shader)
{
	m_VAO.Draw();
}

void Button::handleMouseEvent(const MouseEvent& event)
{
	switch (event.Type)
	{
	case MouseEvent::MouseClick:
		DEBUG_LOG("I was clicked!");
		break;
	case MouseEvent::MouseRelease:
		if(m_OnHover)
			DEBUG_LOG("I was released!");
		break;
	}
}

void Button::createMesh()
{
	m_VAO.AddBuffer(&m_VBO);
	m_VAO.SetElementBuffer(&m_EBO);

	Vertex2D vertex;
	vertex.Color = glm::ivec3(100, 255, 90);

	vertex.Position = glm::ivec2(0);
	m_VBO.AddVertex(vertex);
	vertex.Position = glm::ivec2(m_Size.x, 0);
	m_VBO.AddVertex(vertex);
	vertex.Position = m_Size;
	m_VBO.AddVertex(vertex);
	vertex.Position = glm::ivec2(0, m_Size.x);
	m_VBO.AddVertex(vertex);

	m_EBO.AddIndex(0);
	m_EBO.AddIndex(1);
	m_EBO.AddIndex(2);

	m_EBO.AddIndex(0);
	m_EBO.AddIndex(2);
	m_EBO.AddIndex(3);

	m_VBO.UpdateBuffer();
	m_EBO.UpdateBuffer();
	m_VBO.ClearData();
	m_EBO.ClearData();
}