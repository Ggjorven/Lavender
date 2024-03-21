#include "lvpch.h"
#include "Project.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

namespace Lavender
{

    Project::Project()
    {
        auto& window = Application::Get().GetWindow();
        m_Viewport = Viewport::Create(window.GetWidth(), window.GetHeight());
    }

    Project::~Project()
    {
    }

    void Project::StartRuntime()
    {
        m_Scenes.GetActive()->StartRuntime();
    }

    void Project::StopRuntime()
    {
        m_Scenes.GetActive()->StopRuntime();
    }

    void Project::OnUpdate(float deltaTime)
    {
        m_Scenes.GetActive()->OnUpdate(deltaTime);
    }

    void Project::OnRender()
    {
        m_Viewport->BeginFrame();
        m_Scenes.GetActive()->OnRender(m_Viewport->GetRenderPass()->GetCommandBuffer());
        m_Viewport->EndFrame();
    }

    void Project::OnImGuiRender()
    {
        m_Viewport->BeginRender();
        m_Viewport->EndRender();
    }

    void Project::AddScene(Ref<Scene> scene, const std::string& name, bool active)
    {
        m_Scenes.Add(scene, name, active);
        scene->InitializeAssets(m_Viewport->GetRenderPass());
    }

    Ref<Project> Project::Create()
    {
        return RefHelper::Create<Project>();
    }

}
