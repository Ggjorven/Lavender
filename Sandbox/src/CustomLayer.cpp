#include "CustomLayer.hpp"

#include <Lavender/Core/Logging.hpp>

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
public:
	glm::mat4 Model = {};
	glm::mat4 View = {};
	glm::mat4 Projection = {};
};

void CustomLayer::OnAttach()
{
	
}

void CustomLayer::OnDetach()
{
}

void CustomLayer::OnUpdate(float deltaTime)
{
	m_Timer += deltaTime;

	float r = 0.5f * (1.0f + glm::sin(m_Timer * 2.0f * 3.1415f));
	float g = 0.5f * (1.0f + glm::sin(m_Timer * 2.0f * 3.1415f + 2.0f * 3.1415f / 3.0f));
	float b = 0.5f * (1.0f + glm::sin(m_Timer * 2.0f * 3.1415f + 4.0f * 3.1415f / 3.0f));
}

void CustomLayer::OnRender()
{
	
}

void CustomLayer::OnImGuiRender()
{
	
}

void CustomLayer::OnEvent(Event& e)
{
}
