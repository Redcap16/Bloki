#include <world/Chunk.hpp>

/*bool Chunk::blockVisible(Block::BlockType type, Block::BlockType neighbor)
{
	static const Block::BlockType airID = Block::GetIdByName("Air"),
		waterID = Block::GetIdByName("Water");

	if (type == airID)
		return false;
	if (neighbor == airID)
		return true;

	if (type != waterID && neighbor != waterID)
		return false;
	if (type == waterID && neighbor == waterID)
		return false;
	if (type != waterID && neighbor == waterID)
		return true;
	if (type == waterID && neighbor != waterID)
		return false;
}

void Chunk::pushFace(const unsigned int vertexIndices[4],
	glm::ivec3 position,
	Block::BlockType type,
	bool highlighted)
{
	const float verticesc[5 * 14] = {
	   0, 0, 0, 0, 0.333333333f,
	   0, 1, 0, 0.25f, 0.333333f,
	   1, 1, 0, 0.25f, 0.66666f,
	   1, 0, 0, 0, 0.666666f,
	   0, 0, 1, 0.75f, 0.33333f,
	   0, 1, 1, 0.5f, 0.33333f,
	   1, 1, 1, 0.5f, 0.66666f,
	   1, 0, 1, 0.75, 0.66666f,
	   0, 0, 0, 1, 0.333333f,
	   1, 0, 0, 1, 0.666666f,
	   0, 0, 0, 0.25f, 0,
	   0, 0, 1, 0.5f, 0,
	   1, 0, 0, 0.25f, 1,
	   1, 0, 1, 0.5f, 1 };


	AtlasTexture::SubTexture& st = Block::GetSubTexture(type);

	static const Block::BlockType waterID = Block::GetIdByName("Water");

	if (type != waterID)
	{
		Vertex3D vertex;
		for (int i = 0; i < 4; i++)
		{
			vertex.x = (verticesc[5 * vertexIndices[i] + 0] + position.x);
			vertex.y = (verticesc[5 * vertexIndices[i] + 1] + position.y);
			vertex.z = (verticesc[5 * vertexIndices[i] + 2] + position.z);

			vertex.u = st.uv.x;
			vertex.v = st.uv.y;

			vertex.u += (verticesc[5 * vertexIndices[i] + 3] * st.size.x);
			vertex.v += (verticesc[5 * vertexIndices[i] + 4] * st.size.y);

			vertex.light = highlighted ? highlightedLight : 0.0f;
			vboSolid.AddVertex(vertex);
		}

		const unsigned int index = vboSolid.GetCurrentIndex();

		vboSolid.AddIndex(index - 4);
		vboSolid.AddIndex(index - 2);
		vboSolid.AddIndex(index - 3);

		vboSolid.AddIndex(index - 2);
		vboSolid.AddIndex(index - 1);
		vboSolid.AddIndex(index - 3);
	}
	else
	{
		Vertex3DT vertex;
		for (int i = 0; i < 4; i++)
		{
			vertex.x = (verticesc[5 * vertexIndices[i] + 0] + position.x);
			vertex.y = (verticesc[5 * vertexIndices[i] + 1] + position.y);
			vertex.z = (verticesc[5 * vertexIndices[i] + 2] + position.z);

			vertex.u = st.uv.x;
			vertex.v = st.uv.y;

			vertex.u += (verticesc[5 * vertexIndices[i] + 3] * st.size.x);
			vertex.v += (verticesc[5 * vertexIndices[i] + 4] * st.size.y);

			vertex.light = highlighted ? highlightedLight : 0.0f;

			vertex.transparency = Block::blockData[waterID].transparency;
			vboTransparent.AddVertex(vertex);
		}
		const unsigned int index = vboTransparent.GetCurrentIndex();

		vboTransparent.AddIndex(index - 4);
		vboTransparent.AddIndex(index - 2);
		vboTransparent.AddIndex(index - 3);

		vboTransparent.AddIndex(index - 2);
		vboTransparent.AddIndex(index - 1);
		vboTransparent.AddIndex(index - 3);
	}
}

void Chunk::pushBlock(glm::ivec3 position, Block& current, Chunk* neighborhood[6])
{
	if (!Math::IsBetween(position.x, 0, chunkSizeXZ - 1) ||
		!Math::IsBetween(position.y, 0, chunkSizeY - 1) ||
		!Math::IsBetween(position.z, 0, chunkSizeXZ - 1))
		return;

	static const Block::BlockType airID = Block::GetIdByName("Air"),
		waterID = Block::GetIdByName("Water");
	const bool highlighted = this->highlighted && (position == highlightedBlock);

	if (current.type == airID)
		return;

	if ((position.x == chunkSizeXZ - 1 && (neighborhood[0] == nullptr ||
		blockVisible(block[position.x][position.y][position.z].type, neighborhood[0]->GetBlock(glm::ivec3(0, position.y, position.z)).first.type))) ||
		(position.x < chunkSizeXZ - 1 &&
			blockVisible(block[position.x][position.y][position.z].type, block[position.x + 1][position.y][position.z].type)))
	{
		const unsigned int vertexIndices[4] = { 2, 6, 12, 13 };

		pushFace(vertexIndices, position, current.type, highlighted);
	}
	if ((position.x == 0 && (neighborhood[1] == nullptr ||
		blockVisible(block[position.x][position.y][position.z].type, neighborhood[1]->GetBlock(glm::ivec3(chunkSizeXZ - 1, position.y, position.z)).first.type))) ||
		(position.x > 0 &&
			blockVisible(block[position.x][position.y][position.z].type, block[position.x - 1][position.y][position.z].type)))
	{
		const unsigned int vertexIndices[4] = { 5, 1, 11, 10 };

		pushFace(vertexIndices, position, current.type, highlighted);
	}
	if ((position.y == chunkSizeY - 1 && (neighborhood[2] == nullptr ||
		blockVisible(block[position.x][position.y][position.z].type, neighborhood[2]->GetBlock(glm::ivec3(position.x, 0, position.z)).first.type))) ||
		(position.y < chunkSizeY - 1 &&
			blockVisible(block[position.x][position.y][position.z].type, block[position.x][position.y + 1][position.z].type)))
	{
		const unsigned int vertexIndices[4] = { 5, 6, 1, 2 };

		pushFace(vertexIndices, position, current.type, highlighted);
	}
	if ((position.y == 0 && (neighborhood[3] == nullptr ||
		blockVisible(block[position.x][position.y][position.z].type, neighborhood[3]->GetBlock(glm::ivec3(position.x, chunkSizeY - 1, position.z)).first.type))) ||
		(position.y > 0 &&
			blockVisible(block[position.x][position.y][position.z].type, block[position.x][position.y - 1][position.z].type)))
	{
		const unsigned int vertexIndices[4] = { 4, 8, 7, 9 };

		pushFace(vertexIndices, position, current.type, highlighted);
	}
	if ((position.z == chunkSizeXZ - 1 && (neighborhood[4] == nullptr ||
		blockVisible(block[position.x][position.y][position.z].type, neighborhood[4]->GetBlock(glm::ivec3(position.x, position.y, 0)).first.type))) ||
		(position.z < chunkSizeXZ - 1 &&
			blockVisible(block[position.x][position.y][position.z].type, block[position.x][position.y][position.z + 1].type)))
	{
		const unsigned int vertexIndices[4] = { 5, 4, 6, 7 };

		pushFace(vertexIndices, position, current.type, highlighted);
	}
	if ((position.z == 0 && (neighborhood[5] == nullptr ||
		blockVisible(block[position.x][position.y][position.z].type, neighborhood[5]->GetBlock(glm::ivec3(position.x, position.y, chunkSizeXZ - 1)).first.type))) ||
		(position.z > 0 &&
			blockVisible(block[position.x][position.y][position.z].type, block[position.x][position.y][position.z - 1].type)))
	{
		const unsigned int vertexIndices[4] = { 0, 1, 3, 2 };

		pushFace(vertexIndices, position, current.type, highlighted);
	}
}*/

Chunk::Chunk(glm::ivec3 position, ChunkHandler& world) :
	/*vboSolid({VertexAttribute(3, GL_FLOAT),
		VertexAttribute(2, GL_FLOAT),
		VertexAttribute(1, GL_FLOAT) }, true),
	vboTransparent({ VertexAttribute(3, GL_FLOAT),
		VertexAttribute(2, GL_FLOAT),
		VertexAttribute(1, GL_FLOAT),
		VertexAttribute(1, GL_FLOAT) }, true),*/
	position(position),
	world(world)
{

}

/*void Chunk::CreateMesh(Chunk* neighborhood[6])
{
	toUpdate = false;

	vboSolid.ClearVertices();
	vboTransparent.ClearVertices();

	/*Chunk* neighborhood[6] = { world.GetChunkByCoord(position + glm::ivec3(1, 0, 0)),
		world.GetChunkByCoord(position + glm::ivec3(-1, 0, 0)),
		world.GetChunkByCoord(position + glm::ivec3(0, 1, 0)),
		world.GetChunkByCoord(position + glm::ivec3(0, -1, 0)),
		world.GetChunkByCoord(position + glm::ivec3(0, 0, 1)),
		world.GetChunkByCoord(position + glm::ivec3(0, 0, -1)) };*/
/*
	for (int x = 0; x < chunkSizeXZ; x++)
		for (int y = 0; y < chunkSizeY; y++)
			for (int z = 0; z < chunkSizeXZ; z++)
			{
				const glm::ivec3 pos = { x, y, z };
				pushBlock(pos, block[x][y][z], neighborhood);
			}

	loaded = true;

	vboSolid.UpdateBuffers();
	vboTransparent.UpdateBuffers();
	vboSolid.ClearVertices();
	vboTransparent.ClearVertices();
}
*/
void Chunk::Update(bool force)
{
	//if (force || toUpdate)
	//	CreateMesh();
}

void Chunk::Render(RenderPass pass)
{
	//if (!loaded)
	//	return;
	if (pass == RenderPass::Solid)
		chunkMesh.GetVertexBuffer().Render();
	/*else if (pass == RenderPass::Transparent)
		vboTransparent.Render();*/
	else
		return;
}

/*void Chunk::Clear(glm::ivec3 position)
{
	memset(block, 0, sizeof(block));
	this->position = position;
}*/

/*std::pair<Block, bool> Chunk::GetBlock(glm::uvec3 pos) //LOCAL POSITION
{
	if (!Math::IsBetween(pos.x, 0, chunkSizeXZ) ||
		!Math::IsBetween(pos.y, 0, chunkSizeY) ||
		!Math::IsBetween(pos.z, 0, chunkSizeXZ))
		return { Block(), false };

	return { block[pos.x][pos.y][pos.z], true };
}

bool Chunk::SetBlock(glm::uvec3 pos, Block& newBlock, bool update) //LOCAL POSITION
{
	if (!Math::IsBetween(pos.x, 0, chunkSizeXZ) ||
		!Math::IsBetween(pos.y, 0, chunkSizeY) ||
		!Math::IsBetween(pos.z, 0, chunkSizeXZ))
		return false;

	block[pos.x][pos.y][pos.z] = newBlock;

	if (update)
		toUpdate = true;

	return true;
}

void Chunk::SetHighlightedBlock(glm::ivec3 position)
{
	highlighted = true;
	highlightedBlock = position;

	toUpdate = true;
}

void Chunk::DisableHighlightedBlock()
{
	highlighted = false;

	toUpdate = true;
}*/