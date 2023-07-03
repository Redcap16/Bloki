#pragma once

#include <glm/glm.hpp>

#include <graphics/Camera3D.hpp>
#include <graphics/Camera2D.hpp>

#include <exception>
#include <string>
#include <vector>
#include <graphics/RenderPass.hpp>
#include <ui/UIManager.hpp>

class World;
class Player;

class NotInitializedEx : public std::logic_error
{
protected:
public:
	NotInitializedEx(const std::string message);
};

struct RenderableParameters
{
	bool Transparent;
	ShaderProgram& UsedShader;
	Texture& UsedTexture;

	RenderableParameters(bool transparent, ShaderProgram& usedShader, Texture& texture);
};

class Renderable
{
public:
	//virtual glm::mat4 GetModelMatrix() const = 0;
	virtual void Render(const RenderingContext& context) = 0;
};

class Renderer
{
private:
	struct RenderableRecord
	{
		Renderable& Object;
		RenderableParameters Params;

		RenderableRecord(Renderable& object, RenderableParameters params);
	};
	struct RenderableCompare
	{
		bool operator()(const std::unique_ptr<RenderableRecord>& a, const std::unique_ptr<RenderableRecord>& b) const;
	};

	static Renderer* instance;

	glm::ivec2 windowSize;
	std::vector<Camera3D> cameras;
	Camera3D* currentCamera3d = nullptr;
	Camera2D camera2d;
	RenderingContext context;

	std::vector<std::unique_ptr<RenderableRecord>> m_Renderables;
	bool m_ListUpdated = false;

	void initGL();
	void sortRenderList();

	Renderer(glm::ivec2 windowSize);
public:
	static Renderer& GetInstance();
	static void Init(glm::ivec2 windowSize);
	static void Close();
	static inline bool IsInitialized()
	{
		return instance != nullptr;
	}

	void RegisterRenderable(Renderable& renderable, const RenderableParameters& params);
	bool RemoveRenderable(Renderable& renderable);

	void Resize(glm::ivec2 windowSize);
	Camera3D* CreateCamera3D();
	void SetCamera3D(Camera3D* camera);

	void RenderScene(World& world, Player& player);
};