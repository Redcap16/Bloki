#include <window/window.h>

LRESULT CALLBACK Window::processEvent(HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	Window& win = GetInstance();

	switch (uMsg)
	{
	case WM_ACTIVATE:
	{
		if (!HIWORD(wParam))
		{
			win.active = TRUE;
		}
		else
		{
			win.active = FALSE;
		}

		return 0;
	}
	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		win.keys[wParam] = TRUE;
		return 0;
	}
	case WM_KEYUP:
	{
		win.keys[wParam] = FALSE;
		return 0;
	}
	case WM_SIZE:
	{
		win.width = LOWORD(lParam);
		win.height = HIWORD(lParam);

		Event event = Event(Event::EventType::Resize);
		event.params.window.size = glm::ivec2(win.width, win.height);
		win.event(event);

		return 0;
	}
	case WM_MOUSEMOVE:
	{
		const glm::ivec2 mousePos = glm::ivec2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		win.mouse.position = mousePos;

		Event event = Event(Event::EventType::MouseMoved);
		event.params.mouse.position = mousePos;
		win.event(event);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		win.mouse.lmb = true;
		
		Event event = Event(Event::EventType::MouseClicked);
		event.params.mouseButton.button = Event::MouseButton::LMB;
		win.event(event);
		break;
	}
	case WM_LBUTTONUP:
	{
		win.mouse.lmb = false;
		Event event = Event(Event::EventType::MouseReleased);
		event.params.mouseButton.button = Event::MouseButton::LMB;
		win.event(event);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		win.mouse.rmb = true;
		Event event = Event(Event::EventType::MouseClicked);
		event.params.mouseButton.button = Event::MouseButton::RMB;
		win.event(event);
		break;
	}
	case WM_RBUTTONUP:
	{
		win.mouse.rmb = false;
		Event event = Event(Event::EventType::MouseReleased);
		event.params.mouseButton.button = Event::MouseButton::RMB;
		win.event(event);
		break;
	}
	}
	
	if (win.keys[VK_F1])
	{
		win.keys[VK_F1] = FALSE;
		win.killWindow();
		win.fullscreen = !win.fullscreen;

		if (!win.openWindow())
			exit(0);
	}
	if (win.keys[VK_ESCAPE])
	{
		win.done = TRUE;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
bool Window::openWindow()
{
	GLuint PixelFormat;
	WNDCLASS wc;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT WindowRect;
	WindowRect.left = (long)0;
	WindowRect.right = (long)width;
	WindowRect.top = (long)0;
	WindowRect.bottom = (long)height;

	hInstance = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)processEvent;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"OpenGL";
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", L"NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;
			}
			else
			{
				MessageBox(NULL, L"Program Will Now Close.", L"ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
		}
	}
	if (fullscreen)
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
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	int nTitle = MultiByteToWideChar(CP_ACP, 0, title.c_str(), -1, NULL, 0);
	WCHAR* wTitle = new WCHAR[nTitle];
	MultiByteToWideChar(CP_ACP, 0, title.c_str(), -1, (LPWSTR)wTitle, nTitle);

	if (!(hWnd = CreateWindowEx(dwExStyle,
		L"OpenGL",
		wTitle,
		WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN |
		dwStyle,
		0, 0,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL)))
	{
		DWORD code = GetLastError();
		killWindow();
		MessageBox(NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
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

	if (!(hDC = GetDC(hWnd)))
	{
		killWindow();
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
	{
		killWindow();
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	if (!SetPixelFormat(hDC, PixelFormat, &pfd))
	{
		killWindow();
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	if (!(hRC = wglCreateContext(hDC)))
	{
		killWindow();
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!wglMakeCurrent(hDC, hRC))
	{
		killWindow();
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	return TRUE;
}

void Window::killWindow()
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}
	for (auto it = renderingContexts.begin(); it != renderingContexts.end(); it++)
	{
		if(!wglDeleteContext(*it))
		{
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
	}
	if (hRC)
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC))
		{
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;
	}
	if (hDC && !ReleaseDC(hWnd, hDC))
	{
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL; 
	}
	if (hWnd && !DestroyWindow(hWnd))
	{
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;
	}
	if (!UnregisterClass(L"OpenGL", hInstance))
	{
		MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;
	}
}

Window& Window::GetInstance()
{
	static Window instance;
	return instance;
}

void Window::CheckForEvents()
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			done = TRUE;
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

bool Window::Initialize(int width,
	int height,
	std::string title,
	void (*event)(Event&),
	bool fullscreen)
{
	this->width = width;
	this->height = height;
	this->title = title;
	this->event = event;
	this->fullscreen = fullscreen;
	return openWindow();
}

void Window::Close()
{
	CloseConsole();
	killWindow();
}

void Window::SetMousePos(int x, int y)
{
	POINT xy = { x, y };
	ClientToScreen(hWnd, &xy);
	SetCursorPos(xy.x, xy.y);
}

bool Window::GetKey(char key)
{
	return keys[key];
}

void Window::SetKey(char key, bool state)
{
	keys[key] = state;
}

inline MouseState& Window::GetMouse()
{
	return mouse;
}

void Window::OpenConsole()
{
	AllocConsole();

	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
}

HGLRC Window::CreateRenderingContext()
{
	HGLRC newContext = wglCreateContext(hDC);
	if (newContext == NULL)
	{
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return NULL;
	}
	if (wglShareLists(hRC, newContext) == FALSE)
	{
		DWORD errorCode = GetLastError();
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, L"Error", MB_OK | MB_ICONINFORMATION);
		LocalFree(lpMsgBuf);
		wglDeleteContext(newContext);
		return NULL;
	}
	renderingContexts.push_back(newContext);

	return newContext;
}

void Window::CloseConsole()
{
	//TODO
}