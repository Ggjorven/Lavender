#include "lvpch.h"
#include "Project.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

    Project::Project()
    {
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
        m_Scenes.GetActive()->OnRender();
    }

    Ref<Project> Project::Create()
    {
        return RefHelper::Create<Project>();
    }

}
