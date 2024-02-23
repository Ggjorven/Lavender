#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>
#include <Lavender/Utils/Utils.hpp>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include <functional>

static void A(int a)
{
	LV_LOG_TRACE("A: {0}", a);
}

static void B(int b)
{
	LV_LOG_TRACE("B: {0}", b);
}

void EditorLayer::OnAttach()
{
	Utils::Queue<std::function<void(int)>> queue = {};

	queue.Add(&A);
	queue.Add(&B);

	queue.Execute(2);
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
