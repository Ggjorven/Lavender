#include "lvpch.h"
#include "Project.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Renderer/FrameResources.hpp"

namespace Lavender
{

    Project::Project()
        : Project("")
    {
    }

    Project::Project(const std::filesystem::path& path)
    {
        m_Directories.Project = path;

        auto& window = Application::Get().GetWindow();
        m_Viewport = Viewport::Create(window.GetWidth(), window.GetHeight());

        FrameResources::Init(this);
    }

    Project::~Project()
    {
        FrameResources::Destroy();
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

    Ref<Scene> Project::CreateAndAddScene()
    {
        return Scene::Create(m_Viewport);
    }

    void Project::AddScene(Ref<Scene> scene, const std::string& name, bool active)
    {
        m_Scenes.Add(scene, name, active);
        
        auto& camera = scene->GetCamera();
        camera = EditorCamera::Create(m_Viewport);
    }

    Ref<Project> Project::Create()
    {
        return RefHelper::Create<Project>();
    }

    Ref<Project> Project::Create(const std::filesystem::path& path)
    {
        return RefHelper::Create<Project>(path);
    }

}
