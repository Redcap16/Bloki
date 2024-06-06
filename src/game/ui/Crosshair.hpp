#pragma once

#include <engine/graphics/ImageTexture.hpp>
#include <engine/graphics/Rectangle.hpp>
#include <engine/ui/Widget.hpp>

class Crosshair : public Widget
{
public:
	Crosshair(WidgetParent& parent);
	~Crosshair() = default;
	Crosshair(const Crosshair&) = delete;
	Crosshair& operator= (const Crosshair&) = delete;
	Crosshair(Crosshair&&) = delete;
	Crosshair& operator=(Crosshair&&) = delete;

protected:
	void handleMouseEvent(const MouseEvent& event) override { };
	void render(RenderingParams& params) override;

private:
	static constexpr const char* c_TextureFilename = "crosshair.png";

	Resource<ImageTexture> m_Texture;
	graphics::Rectangle m_Rectangle;
};