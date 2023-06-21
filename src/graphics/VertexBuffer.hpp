#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <windows.h>
#include <graphics/RenderingContext.hpp>

#include <vector>

#include <util/Debug.hpp>

struct VertexAttribute
{
	GLint size;
	GLenum type;
	VertexAttribute(GLint size, GLenum type);
};

template <typename TVertex>
class VertexBuffer
{
private:
	GLuint vao = 0, vbo, ebo;
	unsigned int indicesCount;

	bool dynamic;

	std::vector<TVertex> vertices;
	std::vector<unsigned int> indices;

	void setupBuffers()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		size_t stride = 0;
		for (unsigned int i = 0; i < TVertex::s_AttributeCount; i++)
		{
			const VertexAttribute& attrib = TVertex::s_Attributes[i];

			if(attrib.type == GL_BYTE)
				glVertexAttribIPointer(i, attrib.size, attrib.type, sizeof(TVertex), (const void*)stride); //TODO
			else
				glVertexAttribPointer(i, attrib.size, attrib.type, GL_FALSE, sizeof(TVertex), (const void*)stride);

			glEnableVertexAttribArray(i);

			switch (attrib.type)
			{
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
				stride += 1 * attrib.size;
				break;
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
			case GL_HALF_FLOAT:
				stride += 2 * attrib.size;
				break;
			case GL_INT:
			case GL_UNSIGNED_INT:
			case GL_FIXED:
			case GL_FLOAT:
				stride += 4 * attrib.size;
				break;
			case GL_DOUBLE:
				stride += 8 * attrib.size;
				break;
			default:
				Debug::GetInstance().Log("Error: Bad type enum");
				return;
			}
		}

		glGenBuffers(1, &ebo);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void clearBuffers()
	{
		if (!vao)
			return;

		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);

		glDeleteVertexArrays(1, &vao);

		vao = 0;
	}
public:
	VertexBuffer(bool dynamic = false) :
		dynamic(dynamic)
	{
		setupBuffers();
	}

	~VertexBuffer()
	{
		clearBuffers();
		ClearVertices();
	}

	void AddVertices(TVertex* vertex, unsigned int size)
	{
		if (vertex == nullptr)
			return;

		for (unsigned int i = 0; i < size; i++)
			vertices.push_back(vertex[i]);
	}

	void AddIndices(unsigned int* index, unsigned int size)
	{
		if (index == nullptr)
			return;

		for (unsigned int i = 0; i < size; i++)
			indices.push_back(index[i]);
	}

	void UpdateBuffers()
	{
		if (!vao)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(TVertex),
			vertices.data(),
			dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(unsigned int),
			indices.data(),
			dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		indicesCount = (unsigned int)indices.size(); //In case of vector clear

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void ClearVertices()
	{
		vertices.clear();
		indices.clear();

		std::vector<TVertex>().swap(vertices);
		std::vector<unsigned int>().swap(indices);
	}

	void Render() const
	{
		if (!indicesCount || !vao)
			return;

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
	}

	inline void AddVertex(TVertex& vertex)
	{
		vertices.push_back(vertex);
	}
	inline void AddIndex(unsigned int index)
	{
		indices.push_back(index);
	}
	inline unsigned int GetCurrentIndex()
	{
		return (unsigned int)vertices.size();
	}
};