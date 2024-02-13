#pragma once

#include <Lavender/Core/Layer.hpp>

using namespace Lavender;

class CustomLayer : public Layer
{
public:
	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(float deltaTime) override;
	void OnRender();
	void OnImGuiRender() override;

	void OnEvent(Event& e);

private:
	float m_Timer = 0.0f; // For FPS updates
};