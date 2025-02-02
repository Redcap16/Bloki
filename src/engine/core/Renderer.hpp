#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <engine/graphics/Camera3D.hpp>
#include <engine/graphics/ShaderProgram.hpp>
#include <engine/graphics/Texture.hpp>
#include <engine/graphics/RenderingContext.hpp>
#include <engine/graphics/ErrorCheck.hpp>
#include <algorithm>

struct RenderableParameters
{
	bool Transparent;
	ShaderProgram* UsedShader;
	const Texture* UsedTexture;

	RenderableParameters(bool transparent, ShaderProgram* usedShader, const Texture* texture);
};

class Renderable
{
public:
	virtual glm::mat4 GetModelMatrix() const = 0;
	virtual glm::vec3 GetRenderPosition() const = 0;
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
		const glm::vec3 CameraPosition;
		RenderableCompare(glm::vec3 camPos) : CameraPosition(camPos) {}
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