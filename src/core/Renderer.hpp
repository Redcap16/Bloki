#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <graphics/Camera3D.hpp>
#include <graphics/ShaderProgram.hpp>
#include <graphics/Texture.hpp>
#include <graphics/RenderingContext.hpp>
#include <graphics/ErrorCheck.hpp>
#include <algorithm>

struct RenderableParameters
{
	bool Transparent;
	ShaderProgram* UsedShader;
	Texture* UsedTexture;

	RenderableParameters(bool transparent, ShaderProgram* usedShader, Texture* texture);
};

class Renderable
{
public:
	virtual glm::mat4 GetModelMatrix() const = 0;
	virtual void Render(const RenderingContext& context) = 0;
};

class Renderer3D
{
public:
	Renderer3D(glm::ivec2 windowSize);
	~Renderer3D();
	Renderer3D(const Renderer3D&) = delete;
	Renderer3D& operator=(const Renderer3D&) = delete;

	void RegisterRenderable(Renderable* renderable, const RenderableParameters& params);
	bool RemoveRenderable(Renderable* renderable);

	void Resize(glm::ivec2 windowSize);
	inline void SetCamera(Camera3D* camera);

	void RenderScene();
private:
	struct RenderableRecord
	{
		Renderable* Object;
		RenderableParameters Params;

		RenderableRecord(Renderable* object, RenderableParameters params);
	};

	struct RenderableCompare
	{
		bool operator()(const RenderableRecord& a, const RenderableRecord& b) const;
	};

	glm::ivec2 m_WindowSize;
	Camera3D* m_CurrentCamera3D;
	RenderingContext m_Context;

	std::vector<RenderableRecord> m_Renderables;
	bool m_ListSorted;

	void setupGL();
	void sortRenderList();
};

void Renderer3D::SetCamera(Camera3D* camera)
{
	m_CurrentCamera3D = camera;
}