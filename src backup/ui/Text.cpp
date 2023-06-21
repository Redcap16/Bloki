#include <ui/Text.hpp>

void Text::setupBuffers()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
}

void Text::updateBuffers()
{
	if (!vao)
		setupBuffers();

	std::vector<Vertex2D> vertices;
	std::vector<unsigned int> indices;

	int positionX = 0;
	for (unsigned int i = 0; i < text.length(); i++)
	{
		FontCharacter& character = font.GetCharacter(text[i]);
		Vertex2D newVertex;

		//Left top
		newVertex.x = positionX + character.bearing.x;
		newVertex.y = -character.bearing.y;
		newVertex.u = character.uvLeftTop.x;
		newVertex.v = character.uvLeftTop.y;
		vertices.push_back(newVertex);
		//Right top
		newVertex.x = positionX + character.bearing.x + character.size.x;
		newVertex.y = -character.bearing.y;
		newVertex.u = character.uvLeftTop.x + character.uvSize.x;
		newVertex.v = character.uvLeftTop.y;
		vertices.push_back(newVertex);
		//Right bottom
		newVertex.x = positionX + character.bearing.x + character.size.x;
		newVertex.y = -character.bearing.y + character.size.y;
		newVertex.u = character.uvLeftTop.x + character.uvSize.x;
		newVertex.v = character.uvLeftTop.y + character.uvSize.y;
		vertices.push_back(newVertex);
		//Left bottom
		newVertex.x = positionX + character.bearing.x;
		newVertex.y = -character.bearing.y + character.size.y;
		newVertex.u = character.uvLeftTop.x;
		newVertex.v = character.uvLeftTop.y + character.uvSize.y;
		vertices.push_back(newVertex);

		//First face LT RT RB    0 1 2
		//Second face LT RB LB   0 2 3
		indices.push_back(vertices.size() - 4 + 0);
		indices.push_back(vertices.size() - 4 + 1);
		indices.push_back(vertices.size() - 4 + 2);

		indices.push_back(vertices.size() - 4 + 0);
		indices.push_back(vertices.size() - 4 + 2);
		indices.push_back(vertices.size() - 4 + 3);

		positionX += (int)character.advance;
	}

	indicesCount = indices.size();

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Text::Text(Font& font, std::string text, glm::ivec2 position) :
	Element(position),
	font(font),
	text(text)
{
	if (text != "")
		updateBuffers();
}

void Text::SetText(std::string text)
{
	this->text = text;
	updateBuffers();
}

void Text::Render()
{
	if (!vao)
		return;

	font.BindTexture(UIManager::GetInstance().GetShaderProgram(), "textureAlias", 0);
	UIManager::GetInstance().GetShaderProgram().SetUniform("textColor", textColor);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
}