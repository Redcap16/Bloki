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
	m_Player->SetEyeCamera(&m_Camera);
	m_Renderer->SetCamera(&m_Camera);
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
		break;
	case Event::EventType::MouseClicked:
		//m_Player->MouseClicked(glm::ivec2(0), e.params.mouseButton.button == Event::MouseButton::LMB);
		break;
	case Event::EventType::MouseMoved:
		if(m_Player) m_Player->MouseMoved(e.params.mouse.position);
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
	

	/*struct Vert
	{
		glm::ivec3 position;
	};
	std::vector<Vert> data;
	for (int i = 0; i < 30; i++)
	{
		data.push_back({ {rand() % 30 - 15, rand() % 30 - 15, rand() % 30 - 15} });
	}

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vert), data.data(), GL_STATIC_DRAW);

	glVertexAttribIPointer(0, 3, GL_INT, sizeof(Vert), 0);

	glEnableVertexAttribArray(0);*/
	{
		//ShaderProgram shader("test.glsl");

		/*VertexBuffer<Vertex3DS> vbo(false);
		VertexArray vao;
		ElementBuffer ebo(false);
		vao.AddBuffer(&vbo);
		vao.SetElementBuffer(&ebo);
		for (int i = 0; i < 30; i++)
		{
			vbo.AddVertex({ {rand() % 30 - 15, rand() % 30 - 15, rand() % 30 - 15} });
			ebo.AddIndex(vbo.GetCurrentIndex());
		}
		vbo.UpdateBuffer();
		ebo.UpdateBuffer();*/

		/*glm::vec3 pos(0);
		AtlasTexture* tex = m_ResourceManager.GetAtlasTexture("block.td");
		AtlasTexture::SubTexture subs[Block::c_BlockCount];
		for (int i = 0; i < Block::c_BlockCount; i++)
		{
			if (i == 0)
				continue;
			subs[i] = *tex->GetSubTexture(Block::GetBlockName((Block::BlockType)i));
		}
		ChunkMesh mesh(pos, subs);

		for (int i = 0; i < 300; i++)
		{
			mesh.AddFace((Direction)(rand() % 6), { rand() % 16, rand() % 64, rand() % 16 }, Block::Stone, { 0 });
		}
		mesh.FinishGeometry();
		RenderingContext context;*/

		/*std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(*m_Renderer, glm::ivec3(0, 0, 0));
		ChunkRenderer renderer(*m_Renderer, std::const_pointer_cast<const Chunk>(chunk));
		for (int i = 0; i < 100; i++)
		{
			chunk->SetBlock({ rand() % 16, rand() % 64, rand() % 16 }, Block::Stone);
		}
		renderer.UpdateGeometry();*/

		//shader.UseProgram();

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

				if (m_Running && m_Player)
				{
					const int parts = ceil(deltaTime / 200.0f); //Max 200ms per update, no more
					for (int i = 0; i < parts; ++i)
						m_Player->Update(deltaTime / 1000.0f / parts);
				}

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				/*glBegin(GL_TRIANGLES);
				glVertex3f(0, 0, 0);
				glVertex3f(1, 0, 0);
				glVertex3f(1, 1, 0);

				glVertex3f(1, 1, 0);
				glVertex3f(0, 1, 0);
				glVertex3f(0, 0, 0);
				glEnd();
				CHECK_GL_ERROR();*/

				m_Renderer->RenderScene();
				//shader.SetMVPMatrix(m_Camera.GetCameraMatrix());
				//glDrawArrays(GL_TRIANGLES, 0, 10);
				//vao.Draw();
				
				//mesh.Render(context);
				
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
		}
	}
	m_World.reset();
	m_ResourceManager.FreeResources();

	m_Window.Close();
}