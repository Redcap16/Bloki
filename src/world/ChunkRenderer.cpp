#include <world/ChunkRenderer.hpp>

BlockSubtextures ChunkRenderer::m_BlockSubtextures;
bool ChunkRenderer::m_TextureSetup = false;

ChunkRenderer::ChunkRenderer(ChunkArray& chunkArray, GameState& gameState) :
	m_ChunkArray(chunkArray),
	m_GameState(gameState)
{
	m_ShaderProgram = m_GameState.GetResourceManager().GetShaderProgram("chunk-solid.shader");
	setupBlockTextures();
}

void ChunkRenderer::RenderChunks(RenderingContext& context, glm::vec3 position)
{
	static float time = 0;

	if (m_ShaderProgram == nullptr ||
		context.CurrentCamera3D == nullptr)
		return;

	m_ShaderProgram->UseProgram(); 
	
	const glm::mat4 cameraMatrix = context.CurrentCamera3D->GetCameraMatrix();

	m_ShaderProgram->SetUniform("textureAtlas", 0);
	m_ShaderProgram->SetUniform("lightDir", glm::vec3(cos(time), sin(time), 0));
	time += 0.003f;

	//m_ShaderProgram->SetUniform("cameraPos", context.CurrentCamera3D->GetPosition());
	m_BlockTexture->BindTexture(0);

	for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
			{
				Chunk* const currentChunk = m_ChunkArray.Chunks[x][y][z];
				if (currentChunk == nullptr)
					continue;

				const glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), calculateChunkPosition({ x, y, z }, position));
				m_ShaderProgram->SetMVPMatrix(cameraMatrix * modelMatrix);
				//m_ShaderProgram->SetUniform("modelMatrix", modelMatrix);
				currentChunk->Render(false);
			}

	for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
			{
				Chunk* const currentChunk = m_ChunkArray.Chunks[x][y][z];
				if (currentChunk == nullptr)
					continue;

				m_ShaderProgram->SetMVPMatrix(cameraMatrix * glm::translate(glm::mat4(1.0f), calculateChunkPosition({x, y, z}, position)));
				currentChunk->Render(true);
			}
}

void ChunkRenderer::setupBlockTextures()
{
	m_BlockTexture = m_GameState.GetResourceManager().GetAtlasTexture("blocks");
	if (m_BlockTexture == nullptr || m_TextureSetup)
		return;

	for (int i = 0; i < Block::blockCount; ++i)
	{
		std::string& texture = Block::blockData[i].TextureName;
		if (texture != "")
			m_BlockTexture->Load(texture, m_BlockSubtextures.TextureCoords[i]);
	}

	m_BlockTexture->Bake();
	m_TextureSetup = true;
}