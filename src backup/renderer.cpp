#include <renderer.hpp>

NotInitializedEx::NotInitializedEx(const std::string message) :
	std::logic_error(message)
{

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

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Draw 3D
	if (currentCamera3d)
	{
		glDepthMask(GL_TRUE);
		World::GetInstance().Render(*currentCamera3d, RenderPass::Solid);
		Player::GetInstance().Render(*currentCamera3d);

		glDepthMask(GL_FALSE);
		World::GetInstance().Render(*currentCamera3d, RenderPass::Transparent);
	}
	glDepthMask(GL_TRUE);

	//Draw 2D
	UIManager::GetInstance().Render(camera2d);
}