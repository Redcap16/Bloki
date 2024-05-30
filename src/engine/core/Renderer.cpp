#include <engine/core/Renderer.hpp>

RenderableParameters::RenderableParameters(bool transparent, ShaderProgram* usedShader, const Texture* texture) :
	Transparent(transparent),
	UsedShader(usedShader),
	UsedTexture(texture)
{

}

Renderer3D::RenderableRecord::RenderableRecord(Renderable* object, RenderableParameters params) :
	Object(object),
	Params(params)
{

}

bool Renderer3D::RenderableCompare::operator()(const RenderableRecord& a, const RenderableRecord& b) const
{
	if (!a.Params.Transparent && b.Params.Transparent)
		return true;
	else if (a.Params.Transparent && !b.Params.Transparent)
		return false;
	else
	{
		return a.Params.UsedShader->GetHandle() < a.Params.UsedShader->GetHandle();
	}
}

Renderer3D::Renderer3D(glm::ivec2 windowSize) :
	m_WindowSize(windowSize),
	m_ListSorted(true),
	m_CurrentCamera3D(nullptr)
{
	setupGL();
}

Renderer3D::~Renderer3D()
{

}

void Renderer3D::RegisterRenderable(Renderable* renderable, const RenderableParameters& params)
{
	if (renderable == nullptr)
		return;

	m_Renderables.push_back(RenderableRecord(renderable, params));
	m_ListSorted = false;
}

bool Renderer3D::RemoveRenderable(Renderable* renderable)
{
	for (std::vector<RenderableRecord>::const_iterator it = m_Renderables.begin(); it != m_Renderables.end(); ++it)
		if (it->Object == renderable)
		{
			m_Renderables.erase(it);
			return true;
		}
	
	return false;
}

void Renderer3D::Resize(glm::ivec2 windowSize)
{
	m_WindowSize = windowSize;
}

void Renderer3D::RenderScene()
{
	glViewport(0, 0, (GLsizei)m_WindowSize.x, (GLsizei)m_WindowSize.y);
	CHECK_GL_ERROR();

	if (m_CurrentCamera3D == nullptr)
		return;

	if (!m_ListSorted)
		sortRenderList();
		
	RenderableParameters* lastParams = nullptr;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	m_Context.CurrentCamera3D = m_CurrentCamera3D;
	for (RenderableRecord& record : m_Renderables)
	{
		if (lastParams == nullptr ||
			lastParams->UsedShader->GetHandle() != record.Params.UsedShader->GetHandle())
			record.Params.UsedShader->UseProgram();
		if (lastParams == nullptr ||
			lastParams->UsedTexture->GetHandle() != record.Params.UsedTexture->GetHandle())
			record.Params.UsedTexture->Bind(0);
		//if ((lastParams == nullptr || !lastParams->Transparent) && record.Params.Transparent)
			//glDisable(GL_DEPTH_TEST);

		record.Params.UsedShader->SetMVPMatrix(m_CurrentCamera3D->GetCameraMatrix() * record.Object->GetModelMatrix());
		record.Object->Render(m_Context);

		lastParams = &record.Params;
	}
}

void Renderer3D::setupGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CHECK_GL_ERROR();
}

void Renderer3D::sortRenderList()
{
	if (m_ListSorted)
		return;

	std::sort(m_Renderables.begin(), m_Renderables.end(), RenderableCompare());
	m_ListSorted = true;
}