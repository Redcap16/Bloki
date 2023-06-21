#pragma once

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <windowsx.h>
#include <string>
#include <functional>
#include <vector>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>

struct MouseState
{
	glm::ivec2 position;
	bool lmb, rmb;
};

class Event
{
public:
	enum class EventType
	{
		Resize,
		MouseMoved,
		MouseClicked,
		MouseReleased
	};

	struct MouseParams
	{
		glm::ivec2 position;
	}; 

	enum class MouseButton
	{
		LMB,
		RMB,
	};
	struct MouseButtonParams
	{
		MouseButton button;
	};
	struct WindowParams
	{
		glm::ivec2 size;
	};

	union
	{
		WindowParams window;
		MouseParams mouse;
		MouseButtonParams mouseButton;
	} params;

	EventType type;

	Event(EventType type) :
		type(type)
	{
		memset(&params, 0, sizeof(params));
	}
};

class Window
{
private:
	int width, height;
	std::string title;
	void (*event)(Event&);

	HGLRC hRC = nullptr;
	HDC hDC = nullptr;
	HWND hWnd = nullptr;
	HINSTANCE hInstance;

	std::vector<HGLRC> renderingContexts;

	MouseState mouse;

	bool    keys[256];
	bool    active = TRUE;
	bool done = FALSE;
	bool    fullscreen = TRUE;

	static LRESULT CALLBACK processEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool openWindow();
	void killWindow();
public:
	static Window& GetInstance();
	void CheckForEvents();
	bool Initialize(int width, int height, std::string title, void (*event)(Event&), bool fullscreen = false);
	void Close();
	void SetMousePos(int x, int y);
	bool GetKey(char key);
	void SetKey(char key, bool state);
	MouseState& GetMouse();
	void OpenConsole();
	void CloseConsole();
	inline bool Done() { return done; }
	inline bool Active() { return active; }
	inline HDC GetDeviceContext() { return hDC; }
	inline HGLRC GetMainRenderingContext() { return hRC; }
	HGLRC CreateRenderingContext();
	inline glm::ivec2 GetSize() { return glm::ivec2(width, height); }
};