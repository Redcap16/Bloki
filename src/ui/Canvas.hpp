#pragma once

#include <ui/Widget.hpp>
#include <graphics/ShaderProgram.hpp>

#include <vector>

class Canvas : public WidgetParent
{
public:
	Canvas(glm::ivec2 windowSize);
	~Canvas() = default;
	Canvas(const Canvas&) = delete;
	Canvas& operator=(const Canvas&) = delete;

	void SetWindowSize(glm::ivec2 windowSize);

	void MouseClicked(bool LMB);
	void MouseReleased(bool LMB);
	void MouseMoved(glm::ivec2 position);

	void Render();

protected:
	glm::ivec2 GetAsParentPositition() const override { return glm::ivec2(0, 0); }
	glm::ivec2 GetAsParentSize() const override { return m_WindowSize; };

	void AddWidget(Widget* widget) override { m_Widgets.push_back(widget); };
	void RemoveWidget(const Widget* widget) override;

private:
	static constexpr const char* c_ShaderFile = "ui.shader";

	struct
	{
		glm::ivec2 Position;
		bool LMBClicked,
			RMBClicked;
	} m_MouseState;

	std::vector<Widget*> m_Widgets;
	glm::ivec2 m_WindowSize;

	Resource<ShaderProgram> m_Shader;
	glm::mat4 m_ProjectionMatrix;

	void updateProjectionMatrix();
};