#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

struct A
{
	virtual ~A() = default;

	void Potato()
	{
		LV_LOG_TRACE("Potato");
	}
};

struct B : public A
{
	void Carrot()
	{
		LV_LOG_TRACE("Carrot");
	}
};

void EditorLayer::OnAttach()
{
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float deltaTime)
{
}

void EditorLayer::OnRender()
{
}

void EditorLayer::OnImGuiRender()
{
}

void EditorLayer::OnEvent(Event& e)
{
}
