#define _USE_MATH_DEFINES

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
#include <core/Renderer.hpp>
#include <core/GameState.hpp>

/*
	 ________  ___  ___        ___   _______          _______
	/  _____/ /  / /  /       /  /  /  ___  \        /  ___  \
   /  /___   /  / /  /       /  /  /  /__/  /       /  /__/  /
  /  ____/  /  / /  /       /  /  /  ______/       /  ______/
 /  /      /  / /  /_____  /  /  /  /             /  /    __
/__/      /__/ /________/ /__/  /__/             /__/    /_/
*/

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class Application : public WindowListener
{
public:
	Application()
	{
		setup();
	}

	void OnWindowEvent(const Event& e) override
	{
		Window& win = Window::GetInstance();

		switch (e.type)
		{
		case Event::EventType::Resize:
			if (Renderer::IsInitialized())
				Renderer::GetInstance().Resize(e.params.window.size);
			break;
		}

		if (m_Running)
			m_Player->OnEvent(e);
	}

	void Start()
	{
		if (!m_Setup)
			return;

		clock_t currentTime = clock(),
			lastTime = currentTime,
			frameCounter = 0;

		float deltaTime = 0;

		Window& win = Window::GetInstance();
		Renderer& renderer = Renderer::GetInstance();

		m_Running = true;

		std::cout << glGetError() << std::endl;
		while (!win.Done())
		{
			win.CheckForEvents();
			if (win.Active())
			{
				if (win.GetKey('P'))
				{
					win.SetKey('P', false);
					m_Running = !m_Running;
				}

				if (m_Running)
				{
					const int parts = ceil(deltaTime / 200.0f); //Max 200ms per update, no more
					for (int i = 0; i < parts; ++i)
						m_Player->Update(deltaTime / 1000.0f / parts, *m_World);
				}

				renderer.RenderScene(*m_World, *m_Player);
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
				frameCounter = 0;
			}
			frameCounter++;

			m_World->Update();

			GLenum error = glGetError();
			if (error)
				std::cout << "OPENGL ERROR: " << error << std::endl;
		}
		win.Close();
	}

private:
	std::unique_ptr<Player> m_Player; 
	std::unique_ptr<World> m_World;
	ResourceManager m_ResourceManager;
	std::unique_ptr<GameState> m_GameState;

	bool m_Running = false,
		m_Setup = false;

	void setup()
	{
		Window& win = Window::GetInstance();
		bool fs = true;

		win.OpenConsole();

		if (MessageBox(NULL, L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
			fs = false;

		if (!win.Initialize(1280, 720, "Bloki Alpha 1", this, fs))
		{
			MessageBox(NULL, L"Initialization failed", L"Fail", MB_OK | MB_ICONERROR);
			return;
		}

		win.SetMousePos(640, 360);

		glewInit();

		Renderer::Init(glm::ivec2(1280, 720));
		Renderer& renderer = Renderer::GetInstance();
		m_GameState = std::make_unique<GameState>(Renderer::GetInstance(), m_ResourceManager);

		DebugBox::GetInstance().SetResourceManager(&m_ResourceManager);
		Block::SetupBlockData();

		m_World = std::make_unique<World>(*m_GameState);
		m_World->LoadWorld();

		m_Player = std::make_unique<Player>(*m_World);
		m_Player->SetEyeCamera(renderer.CreateCamera3D());
		renderer.SetCamera3D(m_Player->GetEyeCamera());

		m_Setup = true;
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE   hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application app;
	app.Start();

	return 0;
}