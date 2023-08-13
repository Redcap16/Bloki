#pragma once

#include <fcntl.h>
#include <windows.h>
#include <windowsx.h>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <array>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>

#include <window/Input.hpp>
#include <util/Debug.hpp>

class ConsoleWindow
{
public:
	ConsoleWindow();
private:
	class Console
	{
	public:
		Console();
		~Console();

	private:
		std::fstream m_ConsoleInput,
			m_ConsoleOutput,
			m_ConsoleError;

		std::streambuf* m_StandardInput,
			*m_StandardOutput,
			*m_StandardError;
	};

	std::shared_ptr<Console> m_Pointer;
	static std::weak_ptr<Console> s_Instance;
};

class KeyboardListener
{
public:
	struct KeyboardEvent
	{
		enum Type
		{
			KeyPressed,
			KeyReleased
		};

		Type m_Type;
		Keyboard::Key m_Key;
	}; //TODO: Handle key press counter

	virtual void OnKeyboardEvent(const KeyboardEvent& event) = 0;
};

class WindowListener
{
public:
	virtual void OnWindowResize(glm::ivec2 size) = 0;
};

class MouseListener
{
public:
	struct MouseButtonEvent
	{
		enum Type
		{
			Pressed,
			Released
		};

		Type m_Type;
		Mouse::Button m_Button;
	};

	virtual void OnMouseButtonEvent(const MouseButtonEvent& event) = 0;
	virtual void OnMouseMove(glm::ivec2 position) = 0;
};

class Window
{
public:
	Window(glm::ivec2 size, const std::string& title, bool openConsole);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	void CheckForEvents();
	void SetFullscreen(bool fullscreen);
	bool Done() const { return m_Done; }
	bool Active() const { return m_Active; }
	HDC GetDeviceContext() const { return m_DeviceContextHandle; }
	HGLRC GetRenderingContext() const { return m_RenderingContextHandle; }
	glm::ivec2 GetSize() const { return m_Size; }

	Mouse& GetMouse() { return m_Mouse; }
	Keyboard& GetKeyboard() { return m_Keyboard; }

	bool MakeRenderingContextCurrent();

	void AddKeyboardListener(KeyboardListener& listener) { m_KeyboardListeners.insert(&listener); }
	void AddWindowResizeListener(WindowListener& listener) { m_WindowListeners.insert(&listener); }
	void AddMouseMoveListener(MouseListener& listener) { m_MouseListeners.insert(&listener); }

	void RemoveKeyboardListener(KeyboardListener& listener) { m_KeyboardListeners.erase(&listener); }
	void RemoveWindowResizeListener(WindowListener& listener) { m_WindowListeners.erase(&listener); }
	void RemoveMouseMoveListener(MouseListener& listener) { m_MouseListeners.erase(&listener); }

private:
	std::set<KeyboardListener*> m_KeyboardListeners;
	std::set<WindowListener*> m_WindowListeners;
	std::set<MouseListener*> m_MouseListeners;

	HGLRC m_RenderingContextHandle;
	HDC m_DeviceContextHandle;
	HWND m_WindowHandle;
	HINSTANCE m_InstanceHandle;

	glm::ivec2 m_Size;

	bool m_Done,
		m_Fullscreen,
		m_Active;

	std::string m_Title;

	Mouse m_Mouse;
	Keyboard m_Keyboard;

	std::unique_ptr<ConsoleWindow> m_Console;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static constexpr const wchar_t* c_ClassName = L"BLOKI";
	static bool s_ClassRegistered;
	static void registerClass();

	bool openWindow();
	void closeWindow();
};