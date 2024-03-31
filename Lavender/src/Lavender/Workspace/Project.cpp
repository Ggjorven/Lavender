#include "lvpch.h"
#include "Project.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Renderer/FrameResources.hpp"

#include "Lavender/FileSystem/SceneSerializer.hpp"

namespace Lavender
{

    Project* Project::s_Instance = nullptr;

    Project::Project()
        : Project("")
    {
    }

    Project::Project(const std::filesystem::path& path)
    {
        s_Instance = this;

        m_Directories.Project = path;

        auto& window = Application::Get().GetWindow();
        m_Viewport = Viewport::Create(window.GetWidth(), window.GetHeight());

        FrameResources::Init(this);
    }

    Project::~Project()
    {
        FrameResources::Destroy();
        s_Instance = nullptr;
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

    void Project::OnEvent(Event& e)
    {
        m_Scenes.GetActive()->OnEvent(e);
    }

    Ref<Scene> Project::CreateAndAddScene(bool active)
    {
        auto scene = Scene::Create(m_Viewport);
        AddScene(scene, active);
        return scene;
    }

    void Project::AddScene(Ref<Scene> scene, bool active)
    {
        m_Scenes.Add(scene, active);
        
        auto& camera = scene->GetCamera();
        camera = EditorCamera::Create(m_Viewport);
    }

    void Project::InitializeStartScene()
    {
        Ref<Scene> scene = CreateAndAddScene(true);
        scene->GetAssetManager()->GetAssetsFromDirectory(m_Directories.ProjectDir / m_Directories.Assets);

        SceneSerializer serializer(scene);
        serializer.Deserialize(m_StartScenePath);
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
