#pragma once

#include <glm/glm.hpp>

#include <graphics/Camera3D.hpp>
#include <graphics/Camera2D.hpp>

#include <exception>
#include <string>
#include <vector>
#include <graphics/RenderPass.hpp>
#include <world/World.hpp>
#include <player.hpp>
#include <ui/UIManager.hpp>

class NotInitializedEx : public std::logic_error
{
protected:
public:
	NotInitializedEx(const std::string message);
};

class Renderer
{
private:
	static Renderer* instance;

	glm::ivec2 windowSize;
	std::vector<Camera3D> cameras;
	Camera3D* currentCamera3d = nullptr;
	Camera2D camera2d;

	void initGL();

	Renderer(glm::ivec2 windowSize);
public:
	static Renderer& GetInstance();
	static void Init(glm::ivec2 windowSize);
	static void Close();
	static inline bool IsInitialized()
	{
		return instance != nullptr;
	}

	void Resize(glm::ivec2 windowSize);
	Camera3D* CreateCamera3D();
	void SetCamera3D(Camera3D* camera);

	void RenderScene();
};