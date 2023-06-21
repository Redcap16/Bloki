#include <ui/UIManager.hpp>

UIManager::UIManager() :
	shader("ui.shader")
{
}

UIManager& UIManager::GetInstance()
{
	static UIManager instance;
	return instance;
}

void UIManager::Render(Camera2D& camera)
{
	shader.UseProgram();
	for (auto it = elements.begin(); it != elements.end(); it++)
	{
		const glm::mat4 mvp = camera.GetCameraMatrix() * (*it)->GetModelMatrix();
		UIManager::GetInstance().GetShaderProgram().SetUniform("mvpMatrix", mvp);

		(*it)->Render();
	}
}

void UIManager::AddElement(Element* newElement)
{
	if (!newElement)
		return;
	elements.push_back(newElement);
}