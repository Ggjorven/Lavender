#include "Viewport.hpp"

#include "Lavender/Utils/Profiler.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/CommandBuffer.hpp"

#include "Lavender/WorkSpace/Scene.hpp"
#include "Lavender/WorkSpace/EngineTracker.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace Lavender::UI
{

	Viewport::Viewport()
	{
		//FrameResources& resources = Scene::Get()->GetRenderer()->GetResources();
	}

	Viewport::~Viewport()
	{
	}

	void Viewport::RenderUI()
	{
		APP_PROFILE_SCOPE("Viewport::UI");

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_None);

		//s_TopLeftCursorPos = glm::vec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y);

		auto imWindow = ImGui::GetCurrentWindow();
		auto& position = imWindow->Pos;
		auto& size = imWindow->Size;
		if ((uint32_t)size.x != Track::Viewport::Width || (uint32_t)size.y != Track::Viewport::Height)
		{
			Resize((uint32_t)size.x, (uint32_t)size.y);
		}

		auto& mainWindow = Application::Get().GetWindow();
		Track::Viewport::Position.x = (uint32_t)position.x - mainWindow.GetPositionX();
		Track::Viewport::Position.y = (uint32_t)position.y - mainWindow.GetPositionY();

		auto region = ImGui::GetContentRegionAvail();
		ImGui::Image(Scene::Get()->GetRenderer()->GetResources().Shading.Attachment->GetTextureID(), { region.x, region.y }, { 0.0f, 0.0f }, { 1.0f, 1.0f });
		
		ImGui::End();

		ImGui::PopStyleVar(1);
	}

	void Viewport::Resize(uint32_t width, uint32_t height)
	{
		// Set size
		Track::Viewport::Width = width;
		Track::Viewport::Height = height;

		// Resize
		Scene::Get()->GetRenderer()->Resize(width, height);
	}

}