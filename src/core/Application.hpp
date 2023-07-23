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

#include <window/window.h>
#include <core/Renderer.hpp>
#include <core/Player.hpp>
#include <world/LoadedChunks.hpp>
#include <ui/Canvas.hpp>
#include <ui/Button.hpp>

class KeyboardDummy : public Keyboard
{
public:
	KeyboardDummy() :
		m_Window(Window::GetInstance())
	{
	}

	bool GetKeyState(char key) override
	{
		return m_Window.GetKey(key);
	}
private:
	Window& m_Window;
};

class MouseDummy : public Mouse
{
public:
	MouseDummy() :
		m_Window(Window::GetInstance())
	{
	}
	void SetMousePosition(glm::ivec2 position) override
	{
		m_Window.SetMousePos(position.x, position.y);
	}
private:
	Window& m_Window;
};

class Application : public WindowListener
{
public:
	Application();
	~Application() = default;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void OnWindowEvent(const Event& e) override;
	void Start();
private:
	Window& m_Window;
	std::unique_ptr<Renderer3D> m_Renderer;
	std::unique_ptr<Player> m_Player;
	std::unique_ptr<LoadedChunks> m_World;

	KeyboardDummy m_Keyboard;
	MouseDummy m_Mouse;
	ConsoleDebugHandler m_DebugHandler;

	ResourceManager m_ResourceManager;

	Camera3D m_Camera;
	std::unique_ptr<Canvas> m_Canvas;

	bool m_Running = false;
};
