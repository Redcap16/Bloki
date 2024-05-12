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

#include <window/WindowsKeyboard.hpp>
#include <window/WindowsMouse.hpp>
#include <util/Debug.hpp>

namespace window {
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
				* m_StandardOutput,
				* m_StandardError;
		};

		std::shared_ptr<Console> m_Pointer;
		static std::weak_ptr<Console> s_Instance;
	};

	class WindowListener
	{
	public:
		virtual void OnWindowResize(glm::ivec2 size) = 0;
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

		void AddWindowResizeListener(WindowListener& listener) { m_WindowListeners.insert(&listener); }
		void RemoveWindowResizeListener(WindowListener& listener) { m_WindowListeners.erase(&listener); }

	private:
		std::set<WindowListener*> m_WindowListeners;

		HGLRC m_RenderingContextHandle;
		HDC m_DeviceContextHandle;
		HWND m_WindowHandle;
		HINSTANCE m_InstanceHandle;

		glm::ivec2 m_Size;

		bool m_Done,
			m_Fullscreen,
			m_Active;

		std::string m_Title;

		WindowsMouse m_Mouse;
		WindowsKeyboard m_Keyboard;

		std::unique_ptr<ConsoleWindow> m_Console;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		static constexpr const wchar_t* c_ClassName = L"BLOKI";
		static bool s_ClassRegistered;
		static void registerClass();

		bool openWindow();
		void closeWindow();

		bool processMouseEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool processKeyboardEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool processResizeEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}