#include <core/Renderer.hpp>

#include <world/World.hpp>
#include <player.hpp>

//TODO REMOVE THIS SHIT ^

RenderableParameters::RenderableParameters(bool transparent, ShaderProgram& usedShader, Texture& texture) :
	Transparent(transparent),
	UsedShader(usedShader),
	UsedTexture(texture)
{

}

NotInitializedEx::NotInitializedEx(const std::string message) :
	std::logic_error(message)
{

}

Renderer::RenderableRecord::RenderableRecord(Renderable& object, RenderableParameters params) :
	Object(object),
	Params(params)
{
	
}

bool Renderer::RenderableCompare::operator()(const std::unique_ptr<RenderableRecord>& a, const std::unique_ptr<RenderableRecord>& b) const
{
	if (!a->Params.Transparent && b->Params.Transparent)
		return true;
	else if (a->Params.Transparent && !b->Params.Transparent)
		return false;
	else
	{
		return a->Params.UsedShader.GetProgramID() < a->Params.UsedShader.GetProgramID();
	}
}

Renderer* Renderer::instance = nullptr;
void Renderer::initGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::sortRenderList()
{
	std::sort(m_Renderables.begin(), m_Renderables.end(), RenderableCompare());
	m_ListUpdated = false;
}

Renderer::Renderer(glm::ivec2 windowSize) :
	windowSize(windowSize),
	camera2d(windowSize)
{
	initGL();
}

Renderer& Renderer::GetInstance()
{
	if (!instance)
		throw NotInitializedEx("Renderer not initialized yet. Please initialize it before use.");
	return *instance;
}

void Renderer::Init(glm::ivec2 windowSize)
{
	if (instance)
		return;
	instance = new Renderer(windowSize);
}

void Renderer::Close()
{
	if (!instance)
		return;
	delete instance;
	instance = nullptr;
}

void Renderer::RegisterRenderable(Renderable& renderable, const RenderableParameters& params)
{
	m_Renderables.push_back(std::make_unique<RenderableRecord>(renderable, params));
	m_ListUpdated = true;
}

bool Renderer::RemoveRenderable(Renderable& renderable)
{
	for (std::vector<std::unique_ptr<RenderableRecord>>::iterator it = m_Renderables.begin(); it != m_Renderables.end(); ++it)
		if (&(*it)->Object == &renderable)
		{
			m_Renderables.erase(it);
			m_ListUpdated = true;
			return true;
		}

	return false;
}

void Renderer::Resize(glm::ivec2 windowSize)
{
	glViewport(0, 0, windowSize.x, windowSize.y);

	if (windowSize.y == 0)
		windowSize.y = 1;

	camera2d.SetWindowSize(windowSize);
	for (auto cam = cameras.begin(); cam != cameras.end(); cam++)
		(*cam).SetAspectRatio((float)windowSize.x / windowSize.y);
}

Camera3D* Renderer::CreateCamera3D()
{
	cameras.push_back(Camera3D((float)windowSize.x / windowSize.y));
	return &cameras.back();
}

void Renderer::SetCamera3D(Camera3D* camera)
{
	currentCamera3d = camera;
}

void Renderer::RenderScene(World& world, Player& player)
{
	if (m_ListUpdated)
		sortRenderList();
	//TODO: REMOVE THIS SHIT
	//context.SetCamera2D(&camera2d);
	//context.SetCamera3D(currentCamera3d);
	context.CurrentCamera2D = &camera2d;
	context.CurrentCamera3D = currentCamera3d;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Draw 3D
	if (currentCamera3d)
	{
		glDepthMask(GL_TRUE);
		//world.Render(context);

		//player.Render(*currentCamera3d);

		RenderableRecord* lastRecord = nullptr;
		for (std::unique_ptr<RenderableRecord>& record : m_Renderables)
		{
			if (lastRecord == nullptr ||
				lastRecord->Params.UsedShader.GetProgramID() != record->Params.UsedShader.GetProgramID())
			{
				record->Params.UsedShader.UseProgram();

				record->Params.UsedShader.SetUniform("texture", 0);
			}
			if (lastRecord == nullptr ||
				lastRecord->Params.UsedTexture.GetTextureID() != record->Params.UsedTexture.GetTextureID())
				record->Params.UsedTexture.BindTexture(0);

			record->Object.Render(context);
		}

		glDepthMask(GL_FALSE);
		//World::GetInstance().Render(*currentCamera3d, RenderPass::Transparent);
	}
	glDepthMask(GL_TRUE);

	//Draw 2D
	UIManager::GetInstance().Render(context);
}