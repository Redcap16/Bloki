#define _USE_MATH_DEFINES
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <cmath>
#include <cctype>
#include <ctime>

#include <window/window.h>
#include <world/World.hpp>
#include <player.hpp>
#include <ui/DebugBox.hpp>
#include <renderer.hpp>

/*
	 ________  ___  ___        ___   _______          _______
	/  _____/ /  / /  /       /  /  /  ___  \        /  ___  \
   /  /___   /  / /  /       /  /  /  /__/  /       /  /__/  /
  /  ____/  /  / /  /       /  /  /  ______/       /  ______/
 /  /      /  / /  /_____  /  /  /  /             /  /    __
/__/      /__/ /________/ /__/  /__/             /__/    /_/
*/

bool running = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void event(Event& e)
{
	Window& win = Window::GetInstance();

	switch (e.type)
	{
	case Event::EventType::Resize:
		if(Renderer::IsInitialized())
			Renderer::GetInstance().Resize(e.params.window.size);
		break;
	}

	if(running)
		Player::GetInstance().OnEvent(e);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE   hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Window& win = Window::GetInstance();
	bool fs = true;

	win.OpenConsole();

	if (MessageBox(NULL, L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		fs = false;

	if (!win.Initialize(1280, 720, "Bloki Alpha 1", event, fs))
	{
		MessageBox(NULL, L"Initialization failed", L"Fail", MB_OK | MB_ICONERROR);
		return 0;
	}

	win.SetMousePos(640, 360);

	glewInit();

	{
		Renderer::Init(glm::ivec2(1280, 720));
		Renderer& renderer = Renderer::GetInstance();

		Player::GetInstance().SetEyeCamera(renderer.CreateCamera3D());
		renderer.SetCamera3D(Player::GetInstance().GetEyeCamera());

		running = true;

		Block::TextureManager::GetInstance(); //Not proper way of initialising

		clock_t currentTime = clock(),
			lastTime = currentTime,
			frameCounter = 0;

		float deltaTime = 0;

		while (!win.Done())
		{
			win.CheckForEvents();
			if (win.Active())
			{
				if (win.GetKey('P'))
				{
					win.SetKey('P', false);
					running = !running;
				}

				if (running)
				{
					World::GetInstance().cm.Update();
					Player::GetInstance().Update(deltaTime / 1000.0f);
				}

				renderer.RenderScene();
				SwapBuffers(win.GetDeviceContext());
			}

			if (frameCounter >= 10)
			{
				lastTime = currentTime;
				currentTime = clock();
				deltaTime = currentTime - lastTime;
				const int fps = deltaTime != 0 ? ((float)frameCounter / (deltaTime / CLOCKS_PER_SEC)) : 0;
				char buffer[64];
				sprintf_s(buffer, "%i", fps);
				DebugBox::GetInstance().ChangeDebugValue("FPS", std::string(buffer));
				//std::cout << fps << std::endl;
				frameCounter = 0;
			}
			frameCounter++;
		}
		GLenum error = glGetError();
		if (error)
			std::cout << "OPENGL ERROR: " << error << std::endl; 
		
		DWORD err2 = GetLastError();
		if (err2)
			std::cout << "WINAPI ERROR T1: " << err2 << std::endl;
	}

	win.Close();
	return 0;
}