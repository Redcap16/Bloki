#include <engine/window/Window.hpp>

namespace window {
	std::weak_ptr<ConsoleWindow::Console> ConsoleWindow::s_Instance;

	ConsoleWindow::ConsoleWindow()
	{
		m_Pointer = s_Instance.lock();
		if (!m_Pointer)
		{
			m_Pointer = std::make_shared<Console>();
			s_Instance = m_Pointer;
		}
	}

	ConsoleWindow::Console::Console() :
		m_StandardInput(std::cin.rdbuf()),
		m_StandardOutput(std::cout.rdbuf()),
		m_StandardError(std::cerr.rdbuf())
	{
		if (!AllocConsole())
			return;

		m_ConsoleInput = std::fstream("CONIN$", std::ios_base::in);
		m_ConsoleOutput = std::fstream("CONOUT$", std::ios_base::out);
		std::cin.rdbuf(m_ConsoleInput.rdbuf());
		std::cout.rdbuf(m_ConsoleOutput.rdbuf());
		std::cerr.rdbuf(m_ConsoleOutput.rdbuf());
	}

	ConsoleWindow::Console::~Console()
	{
		std::cin.rdbuf(m_StandardInput);
		std::cout.rdbuf(m_StandardOutput);
		std::cerr.rdbuf(m_StandardError);

		FreeConsole();
	}

	Window::Window(glm::ivec2 size, const std::string& title, bool openConsole) :
		m_Size(size),
		m_Title(title),
		m_Mouse(m_WindowHandle)
	{
		if (openConsole)
			m_Console = std::make_unique<ConsoleWindow>();

		openWindow();
	}

	Window::~Window()
	{
		closeWindow();
	}

	void Window::CheckForEvents()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				m_Done = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	void Window::SetFullscreen(bool fullscreen)
	{
		if (m_Fullscreen == fullscreen)
			return;

		closeWindow();
		m_Fullscreen = fullscreen;
		openWindow();
	}

	bool Window::MakeRenderingContextCurrent()
	{
		if (!wglMakeCurrent(m_DeviceContextHandle, m_RenderingContextHandle))
		{
			closeWindow();
			DEBUG_LOG("Error: Can't activate the GL rendering context");
			return false;
		}
		glewInit();
		return true;
	}

	LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LONG_PTR windowPointer = GetWindowLongPtr(hWnd, 0);
		if (windowPointer == NULL)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

		Window& window = *(Window*)windowPointer;

		window.processMouseEvents(uMsg, wParam, lParam);
		window.processKeyboardEvents(uMsg, wParam, lParam);

		switch (uMsg)
		{
		case WM_ACTIVATE:
			if (!HIWORD(wParam))
				window.m_Active = true;
			else
				window.m_Active = false;

			return 0;
		case WM_SYSCOMMAND:
			switch (wParam)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				return 0;
			}
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	bool Window::s_ClassRegistered = false;

	void Window::registerClass()
	{
		if (s_ClassRegistered)
			return;

		WNDCLASS windowClass{ };
		HINSTANCE instance = GetModuleHandle(NULL);

		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = (WNDPROC)WndProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = sizeof(LONG_PTR);
		windowClass.hInstance = instance;
		windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = NULL;
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = c_ClassName;

		if (!RegisterClass(&windowClass))
		{
			DEBUG_LOG("Error: Failed To register the window class");
			return;
		}

		s_ClassRegistered = true;
	}

	bool Window::openWindow()
	{
		registerClass();

		if (m_Fullscreen)
		{
			DEVMODE dmScreenSettings{ };
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = m_Size.x;
			dmScreenSettings.dmPelsHeight = m_Size.y;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				DEBUG_LOG("Error: Cant switch into fullscreen mode");
				m_Fullscreen = false;
			}
		}

		DWORD dwExStyle;
		DWORD dwStyle;
		if (m_Fullscreen)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP;
			ShowCursor(FALSE);
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW;
		}

		RECT windowRect{};
		windowRect.left = 0;
		windowRect.right = m_Size.x;
		windowRect.top = 0;
		windowRect.bottom = m_Size.y;
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

		int nTitle = MultiByteToWideChar(CP_ACP, 0, m_Title.c_str(), -1, NULL, 0);
		std::vector<WCHAR> wTitle(nTitle);
		MultiByteToWideChar(CP_ACP, 0, m_Title.c_str(), -1, (LPWSTR)wTitle.data(), nTitle);

		HINSTANCE instance = GetModuleHandle(NULL);
		if (!(m_WindowHandle = CreateWindowEx(dwExStyle,
			c_ClassName,
			wTitle.data(),
			WS_CLIPSIBLINGS |
			WS_CLIPCHILDREN |
			dwStyle,
			0, 0,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			instance,
			NULL)))
		{
			DWORD code = GetLastError();
			closeWindow();
			DEBUG_LOG("Error: Cant open window");
			return false;
		}

		SetWindowLongPtr(m_WindowHandle, 0, (LONG_PTR)this);

		static  PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,
			0,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		if (!(m_DeviceContextHandle = GetDC(m_WindowHandle)))
		{
			closeWindow();
			DEBUG_LOG("Error: Can't create a GL device context");
			return false;
		}
		GLuint pixelFormat;
		if (!(pixelFormat = ChoosePixelFormat(m_DeviceContextHandle, &pfd)))
		{
			closeWindow();
			DEBUG_LOG("Error: Can't find a suitable pixel format");
			return false;
		}
		if (!SetPixelFormat(m_DeviceContextHandle, pixelFormat, &pfd))
		{
			closeWindow();
			DEBUG_LOG("Error: Can't set the pixel format");
			return false;
		}
		if (!(m_RenderingContextHandle = wglCreateContext(m_DeviceContextHandle)))
		{
			closeWindow();
			DEBUG_LOG("Error: Can't create a GL rendering context");
			return false;
		}
		if (!MakeRenderingContextCurrent())
			return false;

		ShowWindow(m_WindowHandle, SW_SHOW);
		SetForegroundWindow(m_WindowHandle);
		SetFocus(m_WindowHandle);
		return true;
	}

	void Window::closeWindow()
	{
		if (m_Fullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
			ShowCursor(TRUE);
		}
		if (m_RenderingContextHandle)
		{
			if (!wglMakeCurrent(NULL, NULL))
				DEBUG_LOG("Error: Release of DC and RC failed");
			if (!wglDeleteContext(m_RenderingContextHandle))
				DEBUG_LOG("Error: Release rendering context failed");
			m_RenderingContextHandle = NULL;
		}
		if (m_DeviceContextHandle && !ReleaseDC(m_WindowHandle, m_DeviceContextHandle))
		{
			DEBUG_LOG("Error: Release device context failed");
			m_DeviceContextHandle = NULL;
		}
		if (m_WindowHandle && !DestroyWindow(m_WindowHandle))
		{
			DEBUG_LOG("Error: Could not release window");
			m_WindowHandle = NULL;
		}
	}

	bool Window::processMouseEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_MOUSEMOVE:
		{
			const glm::ivec2 mousePos = glm::ivec2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			m_Mouse.MouseMoved(mousePos);
			return true;
		}
		case WM_LBUTTONDOWN:
			m_Mouse.ButtonPressed(Mouse::Button::Left);
			return true;
		case WM_LBUTTONUP:
			m_Mouse.ButtonReleased(Mouse::Button::Left);
			return true;
		case WM_RBUTTONDOWN:
			m_Mouse.ButtonPressed(Mouse::Button::Right);
			return true;
		case WM_RBUTTONUP:
			m_Mouse.ButtonReleased(Mouse::Button::Right);
			return true;
		case WM_MBUTTONDOWN:
			m_Mouse.ButtonPressed(Mouse::Button::Middle);
			return true;
		case WM_MBUTTONUP:
			m_Mouse.ButtonReleased(Mouse::Button::Middle);
			return true;
		}

		return false;
	}

	bool Window::processKeyboardEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_KEYDOWN:
			m_Keyboard.KeyPressed(wParam, LOWORD(lParam));
			return true;
		case WM_KEYUP:
			m_Keyboard.KeyReleased(wParam, LOWORD(lParam));
			return true;
		}

		return false;
	}

	bool Window::processResizeEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (uMsg != WM_SIZE)
			return false;

		m_Size = { LOWORD(lParam), HIWORD(lParam) };

		for (auto listener : m_WindowListeners)
			listener->OnWindowResize(m_Size);

		return true;
	}
}