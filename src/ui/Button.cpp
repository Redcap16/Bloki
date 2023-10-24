#include <ui/Button.hpp>

Button::Button(WidgetParent& parent, glm::ivec2 position, glm::ivec2 size) :
	Widget(parent, position, size),
	m_Mesh(size)
{
}

void Button::handleMouseEvent(const MouseEvent& event)
{
	switch (event.Type)
	{
	case MouseEvent::MouseEnter:
		m_Mesh.ChangeColor(c_HighlightedColor);
		break;
	case MouseEvent::MouseExit:
		m_Mesh.ChangeColor(glm::ivec3(255, 255, 255));
		break;
	case MouseEvent::MouseClick:
		pressed();
		break;
	case MouseEvent::MouseRelease:
		if (m_OnHover)
			released();
		break;
	}
}

void Button::render(RenderingParams& params)
{
	m_Mesh.Render(params);
}

Button::ButtonMesh::ButtonMesh(glm::ivec2 size) :
	m_BorderVBO(m_BorderVAO.CreateVertexBuffer<Vertex2D>(false)),
	m_BorderEBO(m_BorderVAO.GetElementBuffer()),
	m_CenterVBO(m_CenterVAO.CreateVertexBuffer<Vertex2D>(false)),
	m_CenterEBO(m_CenterVAO.GetElementBuffer()),
	m_CenterVAO(false),
	m_BorderVAO(false),
	m_Size(size),
	m_Texture(c_TextureFilename, false)
{
	createMesh();
}

void Button::ButtonMesh::ChangeColor(glm::ivec3 color)
{
	addRectangle(m_CenterVBO, m_CenterEBO, glm::ivec2(c_BorderWidth, c_BorderWidth), glm::ivec2(m_Size.x - 2 * c_BorderWidth, m_Size.y - 2 * c_BorderWidth), glm::vec2(c_BorderTextureWidth, c_BorderTextureWidth), glm::vec2(1 - 2 * c_BorderTextureWidth, 1 - 2 * c_BorderTextureWidth), color);

	m_CenterVBO.UpdateBuffer();
	m_CenterEBO.UpdateBuffer();
	m_CenterVBO.ClearData();
	m_CenterEBO.ClearData();
}

void Button::ButtonMesh::Render(RenderingParams& params)
{
	params.SetColoringType(ColoringType::ColorTexture);
	m_Texture->Bind(0);

	m_BorderVAO.Draw();
	m_CenterVAO.Draw();
}

void Button::ButtonMesh::createMesh()
{
	const glm::ivec3 color = glm::ivec3(255, 255, 255);

	addRectangle(m_BorderVBO, m_BorderEBO, glm::ivec2(c_BorderWidth, 0), glm::ivec2(m_Size.x - 2 * c_BorderWidth, c_BorderWidth), glm::vec2(c_BorderTextureWidth, 0), glm::vec2(1 - 2 * c_BorderTextureWidth, c_BorderTextureWidth), color);
	addRectangle(m_BorderVBO, m_BorderEBO, glm::ivec2(0, c_BorderWidth), glm::ivec2(c_BorderWidth, m_Size.y - 2 * c_BorderWidth), glm::vec2(0, c_BorderTextureWidth), glm::vec2(c_BorderTextureWidth, 1 - 2 * c_BorderTextureWidth), color);
	addRectangle(m_BorderVBO, m_BorderEBO, glm::ivec2(c_BorderWidth, m_Size.y - c_BorderWidth), glm::ivec2(m_Size.x - 2 * c_BorderWidth, c_BorderWidth), glm::vec2(c_BorderTextureWidth, 1 - c_BorderTextureWidth), glm::vec2(1 - 2 * c_BorderTextureWidth, c_BorderTextureWidth), color);
	addRectangle(m_BorderVBO, m_BorderEBO, glm::ivec2(m_Size.x - c_BorderWidth, c_BorderWidth), glm::ivec2(c_BorderWidth, m_Size.y - 2 * c_BorderWidth), glm::vec2(1 - c_BorderTextureWidth, c_BorderTextureWidth), glm::vec2(c_BorderTextureWidth, 1 - 2 * c_BorderTextureWidth), color);

	addRectangle(m_BorderVBO, m_BorderEBO, glm::ivec2(0, 0), glm::ivec2(c_BorderWidth, c_BorderWidth), glm::vec2(0, 0), glm::vec2(c_BorderTextureWidth, c_BorderTextureWidth), color);
	addRectangle(m_BorderVBO, m_BorderEBO, glm::ivec2(0, m_Size.y - c_BorderWidth), glm::ivec2(c_BorderWidth, c_BorderWidth), glm::vec2(0, 1 - c_BorderTextureWidth), glm::vec2(c_BorderTextureWidth, c_BorderTextureWidth), color);
	addRectangle(m_BorderVBO, m_BorderEBO, glm::ivec2(m_Size.x - c_BorderWidth, m_Size.y - c_BorderWidth), glm::ivec2(c_BorderWidth, c_BorderWidth), glm::vec2(1 - c_BorderTextureWidth, 1 - c_BorderTextureWidth), glm::vec2(c_BorderTextureWidth, c_BorderTextureWidth), color);
	addRectangle(m_BorderVBO, m_BorderEBO, glm::ivec2(m_Size.x - c_BorderWidth, 0), glm::ivec2(c_BorderWidth, c_BorderWidth), glm::vec2(1 - c_BorderTextureWidth, 0), glm::vec2(c_BorderTextureWidth, c_BorderTextureWidth), color);

	m_BorderVBO.UpdateBuffer();
	m_BorderEBO.UpdateBuffer();
	m_BorderVBO.ClearData();
	m_BorderEBO.ClearData();

	ChangeColor(color);
}

void Button::ButtonMesh::addRectangle(VertexBuffer<Vertex2D>& vbo, ElementBuffer& ebo, glm::ivec2 position, glm::ivec2 size, glm::vec2 textureCoords, glm::vec2 textureSize, glm::ivec3 color) //TODO: Use graphics/Rectangle
{
	addVertex(vbo, position, textureCoords, color);
	addVertex(vbo, position + glm::ivec2(size.x, 0), textureCoords + glm::vec2(textureSize.x, 0), color);
	addVertex(vbo, position + size, textureCoords + textureSize, color);
	addVertex(vbo, position + glm::ivec2(0, size.y), textureCoords + glm::vec2(0, textureSize.y), color);

	unsigned int index = vbo.GetCurrentIndex();
	ebo.AddIndex(index - 3);
	ebo.AddIndex(index - 2);
	ebo.AddIndex(index - 1);

	ebo.AddIndex(index - 3);
	ebo.AddIndex(index - 1);
	ebo.AddIndex(index);
}

void Button::ButtonMesh::addVertex(VertexBuffer<Vertex2D>& vbo, glm::ivec2 position, glm::vec2 textureCoords, glm::ivec3 color)
{
	Vertex2D vertex;
	vertex.Color = color;
	vertex.Position = position;
	vertex.TextureCoords = textureCoords;
	vbo.AddVertex(vertex);
}