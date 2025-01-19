#include <game/core/Application.hpp>

using window::Window;
using window::WindowListener;
using window::Mouse;
using window::MouseButtonEvent;
using window::Keyboard;
using window::KeyboardEvent;

#include <game/items/FoodItem.hpp>

Application::Application() :
	m_Window(glm::ivec2(1280, 720), "Bloki Alpha 2", true),
	m_Camera(m_Window.GetSize()),
	m_Renderer(m_Window.GetSize()),
	m_World(m_Renderer, "saves/second"),
	m_Player(m_World, m_Window.GetKeyboard(), m_Window.GetMouse(), m_Window.GetSize(), m_DroppedItemRepository),
	m_WorldRenderer(m_Renderer, m_World, m_Player),
	m_UIManager(m_Window, m_Player.GetInventory()),
	m_DroppedItemRepository(m_World),
	m_DroppedItemsRenderer(m_Renderer, m_DroppedItemRepository)
{
	m_Window.GetMouse().SetPosition(m_Window.GetSize() / 2);
	m_Window.GetKeyboard().AddKeyboardListener(*this);
	m_Window.GetMouse().AddMouseListener(*this);
	m_Window.AddWindowResizeListener(*this);

	m_DroppedItemRepository.AddDroppedItem(ItemStack(FoodItem(FoodItem::FoodType::Apple), 3), { 3, 50, 0 });
	m_Player.SetPosition(glm::vec3(0, 40, 0));
	m_Player.SetEyeCamera(&m_Camera);
	m_Renderer.SetCamera(&m_Camera);
}

void Application::OnKeyboardEvent(const KeyboardEvent& event)
{
	if(event.EventType == event.KeyPressed &&
		event.KeyCode == 'p')
		m_Running = !m_Running;
	else if (event.EventType == event.KeyPressed &&
		event.KeyCode == 'e')
	{
		m_UIManager.ShowInventory(!m_UIManager.IsInventoryVisible());
		m_Window.GetMouse().SetPosition(m_Window.GetSize() / 2);
	}
	if (event.EventType == KeyboardEvent::KeyPressed &&
		event.KeyCode == Keyboard::Key::Escape)
		m_Done = true;
}

void Application::OnWindowResize(glm::ivec2 size)
{
	m_Renderer.Resize(size);
	m_Player.SetWindowSize(size);

	m_UIManager.SetWindowSize(size);
}

void Application::OnMouseButtonEvent(const MouseButtonEvent& event)
{
	if (event.EventType == MouseButtonEvent::Pressed)
	{
		if (!m_UIManager.IsInventoryVisible())
			m_Player.MouseClicked(glm::ivec2(0), event.ButtonType == Mouse::Button::Left);

		m_UIManager.MouseClicked(event.ButtonType == Mouse::Button::Left);
	}
	else if (event.EventType == MouseButtonEvent::Released)
	{
		m_UIManager.MouseReleased(event.ButtonType == Mouse::Button::Left);
	}
}

void Application::OnMouseMove(glm::ivec2 position)
{
	if (!m_UIManager.IsInventoryVisible())
		m_Player.MouseMoved(position);

	m_UIManager.MouseMoved(position);
}

void Application::SetChunksToRender() {
	std::set<Chunk*> chunksToRender;
	ChunkPos centerChunk = Chunk::GetChunkPosition(m_Player.GetPosition());
	for (int x = -3; x <= 3; ++x)
		for(int y = -1; y <= 1; ++y)
			for (int z = -3; z <= 3; ++z) {
				Chunk* const chunk = m_World.GetChunk(centerChunk + glm::ivec3(x, y, z));
				if (chunk != nullptr)
					chunksToRender.insert(chunk);
			}

	m_WorldRenderer.SetChunksToRender(chunksToRender);
	m_DroppedItemRepository.SetCenterChunk(centerChunk);
}

void Application::Start()
{
	clock_t currentTime = clock(),
		lastTime = currentTime,
		frameCounter = 0;

	float deltaTime = 0;

	m_Running = true;

	while (!m_Window.Done() && !m_Done)
	{
		m_Window.CheckForEvents();
		if (m_Window.Active())
		{
			if (m_Running)
			{
				const int parts = ceil(deltaTime / 200.0f); //Max 200ms per update, no more
				for (int i = 0; i < parts; ++i) {
					m_Player.Update(deltaTime / 1000.0f / parts);
					m_DroppedItemRepository.Update(deltaTime / 1000.0f / parts);
				}
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_Renderer.RenderScene();
			m_UIManager.Render();
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

		m_UIManager.Update();
		m_World.Update();
		m_World.SetCenter(m_Player.GetPosition());
		SetChunksToRender();
		m_WorldRenderer.Update();
	}
}