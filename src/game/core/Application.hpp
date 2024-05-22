#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include <iostream>
#include <cmath>
#include <cctype>
#include <ctime>

#include <engine/window/Window.hpp>
#include <engine/core/Renderer.hpp>
#include <game/core/Player.hpp>
#include <game/world/LoadedChunks.hpp>
#include <game/graphics/WorldRenderer.hpp>
#include <game/core/UIManager.hpp>
#include <engine/ui/Button.hpp>
#include <game/graphics/DroppedItemRenderer.hpp>

class Application : public window::KeyboardListener, 
	public window::WindowListener, 
	public window::MouseListener
{
public:

	Application();
	~Application() = default;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void OnKeyboardEvent(const window::KeyboardEvent& event) override;
	void OnWindowResize(glm::ivec2 size) override;
	void OnMouseButtonEvent(const window::MouseButtonEvent& event) override;
	void OnMouseMove(glm::ivec2 position) override;
	void OnMouseWheelMove(int movement) override {};

	void SetChunksToRender(); //Move to WorldRenderer
	void Start();
private:
	window::Window m_Window;
	Renderer3D m_Renderer;
	LoadedChunks m_World;
	Player m_Player;
	WorldRenderer m_WorldRenderer;
	DroppedItemRepository m_DroppedItemRepository;
	DroppedItemRenderer m_DroppedItemsRenderer;

	Camera3D m_Camera;
	UIManager m_UIManager;

	bool m_Running = false,
		m_Done = false;
};
