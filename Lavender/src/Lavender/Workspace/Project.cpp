#include "lvpch.h"
#include "Project.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Renderer/FrameResources.hpp"

#include "Lavender/Workspace/SceneRenderer.hpp"

#include "Lavender/FileSystem/SceneSerializer.hpp"

namespace Lavender
{

    Project* Project::s_Instance = nullptr;

    Project::Project()
        : Project("")
    {
    }

    Project::Project(const std::filesystem::path& path)
        : m_Assets(AssetManager::Create())
    {
        s_Instance = this;

        m_Directories.Project = path;

        auto& window = Application::Get().GetWindow();
        m_Viewport = Viewport::Create(window.GetWidth(), window.GetHeight());

        FrameResources::Init(this);
        SceneRenderer::Init();
    }

    Project::~Project()
    {
        if (m_State == ProjectState::Runtime)
            SwitchState();

        m_Assets->Serialize();

        FrameResources::Destroy();
        SceneRenderer::Destroy();
        s_Instance = nullptr;
    }

    void Project::StartRuntime()
    {
        m_State = ProjectState::Runtime;
        m_Scenes.GetActive()->StartRuntime();
    }

    void Project::StopRuntime()
    {
        m_Scenes.GetActive()->StopRuntime();
        m_State = ProjectState::Editor;
    }

    void Project::SwitchState()
    {
        m_Assets->SwitchAssets();
        
        switch (m_State)
        {
        case ProjectState::Editor:
            StartRuntime();
            break;
        case ProjectState::Runtime:
            StopRuntime();
            break;

        default:
            LV_LOG_FATAL("No proper state selected.");
            break;
        }
    }

    void Project::OnUpdate(float deltaTime)
    {
        m_Scenes.GetActive()->OnUpdate(m_State, deltaTime);
    }

    void Project::OnRender()
    {
        m_Viewport->BeginFrame();
        m_Scenes.GetActive()->OnRender(m_State, m_Viewport->GetRenderPass()->GetCommandBuffer());
        m_Viewport->EndFrame();
    }

    void Project::OnImGuiRender()
    {
        m_Viewport->BeginRender();
        m_Viewport->RenderUI(this);
        m_Viewport->EndRender();
    }

    void Project::OnEvent(Event& e)
    {
        m_Scenes.GetActive()->OnEvent(m_State, e);
    }

    Ref<Scene> Project::CreateAndAddScene()
    {
        auto scene = Scene::Create(m_Viewport, m_Assets, m_Script);
        AddScene(scene);
        return scene;
    }

    void Project::AddScene(Ref<Scene> scene)
    {
        m_Scenes.Add(scene);
    }

    void Project::AddScene(const UUID& uuid, const SceneMetaData& data)
    {
        m_Scenes.Add(uuid, data);
    }

    void Project::InitializeStartScene()
    {
        Ref<Scene> scene = CreateAndAddScene();
        scene->GetAssetManager()->GetAssetsFromDirectory(m_Directories.ProjectDir / m_Directories.Assets);

        SceneSerializer serializer(scene);
        serializer.Deserialize(m_Directories.ProjectDir / m_StartScenePath);
    }

    void Project::SetScript(Ref<ScriptLoader> script)
    {
        m_Script = script;

        if (m_Scenes.GetActive())
            m_Scenes.GetActive()->SetScript(script);
    }

    void Project::ReloadScript()
    {
        m_Script->Reload();

        if (m_Scenes.GetActive())
            m_Scenes.GetActive()->ReloadScript();
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
