#include <core/Application.hpp>


Application::Application() :
	m_Window(Window::GetInstance()),
	m_Camera({1280, 720})
{
	bool fs = true;

	m_Window.OpenConsole();

	if (MessageBox(NULL, L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		fs = false;

	if (!m_Window.Initialize(1280, 720, "Bloki Alpha 1", this, fs))
	{
		MessageBox(NULL, L"Initialization failed", L"Fail", MB_OK | MB_ICONERROR);
		return;
	}

	m_Window.SetMousePos(640, 360);

	glewInit();

	DebugProvider::Set(&m_DebugHandler);

	Block::SetupBlockData();

	m_Renderer = std::make_unique<Renderer3D>(m_Window.GetSize());
	m_World = std::make_unique<LoadedChunks>(*m_Renderer, "saves/first");
	m_Player = std::make_unique<Player>(*m_World, m_Keyboard, m_Mouse, m_Window.GetSize());
	m_Player->SetPosition(glm::vec3(0, 40, 0));
	m_Player->SetEyeCamera(&m_Camera);
	m_Renderer->SetCamera(&m_Camera);

	m_Inventory = std::make_unique<Inventory>();
	m_Inventory->SetItemStack(2, { Item::Bread, 2 });
	m_UIManager = std::make_unique<UIManager>(m_Window, *m_Inventory);
}

void Application::OnWindowEvent(const Event& e)
{
	switch (e.type)
	{
	case Event::EventType::Resize:
		if(m_Renderer)
			m_Renderer->Resize(e.params.window.size);
		if(m_Player)
			m_Player->SetWindowSize(e.params.window.size);

		if(m_UIManager) m_UIManager->SetWindowSize(e.params.window.size);
		break;
	case Event::EventType::MouseClicked:
		if(m_UIManager && !m_UIManager->IsInventoryVisible())
			m_Player->MouseClicked(glm::ivec2(0), e.params.mouseButton.button == Event::MouseButton::LMB);

		if (m_UIManager) m_UIManager->MouseClicked(e.params.mouseButton.button == Event::MouseButton::LMB);
		break;
	case Event::EventType::MouseMoved:
		if (m_UIManager && !m_UIManager->IsInventoryVisible())
			if(m_Player) m_Player->MouseMoved(e.params.mouse.position);

		if (m_UIManager) m_UIManager->MouseMoved(e.params.mouse.position);
		break;
	case Event::EventType::MouseReleased:
		if (m_UIManager) m_UIManager->MouseReleased(e.params.mouseButton.button == Event::MouseButton::LMB);
		break;
	}
}

void Application::Start()
{
	clock_t currentTime = clock(),
		lastTime = currentTime,
		frameCounter = 0;

	float deltaTime = 0;

	m_Running = true;

	while (!m_Window.Done())
	{
		m_Window.CheckForEvents();
		if (m_Window.Active())
		{
			if (m_Window.GetKey('P'))
			{
				m_Window.SetKey('P', false);
				m_Running = !m_Running;
			}
			if (m_Window.GetKey('E'))
			{
				m_Window.SetKey('E', false);
				m_UIManager->ShowInventory(!m_UIManager->IsInventoryVisible());
				glm::ivec2 pos = m_Window.GetSize() / 2;
				m_Window.SetMousePos(pos.x, pos.y);
			}

			if (m_Running && m_Player)
			{
				const int parts = ceil(deltaTime / 200.0f); //Max 200ms per update, no more
				for (int i = 0; i < parts; ++i)
					m_Player->Update(deltaTime / 1000.0f / parts);
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_Renderer->RenderScene();
			m_UIManager->Render();
			m_UIManager->Update();
			SwapBuffers(m_Window.GetDeviceContext());
			CHECK_GL_ERROR();
		}

		if (frameCounter >= 10)
		{
			lastTime = currentTime;
			currentTime = clock();
			deltaTime = currentTime - lastTime;
			const int fps = deltaTime != 0 ? ((float)frameCounter / (deltaTime / CLOCKS_PER_SEC)) : 0;
			char buffer[64];
			sprintf_s(buffer, "%i", fps);
			frameCounter = 0;
		}
		frameCounter++;

		m_World->Update();
		m_World->SetCenter(m_Player->GetPosition());
	}

	m_World.reset();
	m_UIManager.reset();

	m_Window.Close();
}