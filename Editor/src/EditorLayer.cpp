#include "EditorLayer.hpp"

#include <fstream>

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Input/Input.hpp>
#include <Lavender/Utils/Profiler.hpp>

#include <Lavender/Renderer/Shader.hpp>

#include <Lavender/FileSystem/ProjectSerializer.hpp>

#include <Lavender/WorkSpace/EngineTracker.hpp>

#include <imgui.h>

#include <Lavender/ECS/Entity.hpp>
#include <Lavender/ECS/Components.hpp>

/////////////////////////////////////////////////////////////////
// Base Functions
/////////////////////////////////////////////////////////////////
void EditorLayer::OnAttach()
{
	m_Project = Project::Create();
	m_Project->GetState() = WorkSpace::State::Editor;

	// Load a project from .lvproj file
	if (__argc == 2)
	{
		std::filesystem::path projectPath = __argv[1];

		if (projectPath.extension() != ".lvproj")
		{
			APP_LOG_ERROR("File loaded is not a Lavender Project!");
		}
		else
		{
			ProjectSerializer serializer(m_Project);
			serializer.Deserialize(projectPath);
		}
	}
	else
	{
		ProjectSerializer serializer(m_Project);
		serializer.Deserialize("Resources/Projects/Example/Example.lvproj");
	}
}

void EditorLayer::OnDetach()
{
	ProjectSerializer serializer(m_Project);
	serializer.Serialize();
}

void EditorLayer::OnUpdate(float deltaTime)
{
	APP_PROFILE_SCOPE("Editor::Update");
	Application::Get().GetWindow().SetTitle(fmt::format("Editor | FPS: {0} | Frametime: {1:.3f}ms", Track::Frame::FPS, Track::Frame::FrameTime)); // TODO: Remove takes 300microsecs

	m_Project->OnUpdate(deltaTime);
}

void EditorLayer::OnRender()
{
	APP_PROFILE_SCOPE("Editor::Render");

	m_Viewport.StartRecording();
	m_Project->OnRender();
	m_Viewport.EndRecording();
}

void EditorLayer::OnEvent(Event& e)
{
	m_Project->OnEvent(e);
}

void EditorLayer::OnUIRender()
{
	APP_PROFILE_SCOPE("Editor::UI");

	m_Viewport.RenderUI();
}

/////////////////////////////////////////////////////////////////
// Custom Functionality
/////////////////////////////////////////////////////////////////
// TODO: Update + clean
void EditorLayer::CreateProject(const std::string& name, const std::filesystem::path& directory)
{
	std::filesystem::path projectPath = directory / name;
	
	// Create Directories
	{
		std::filesystem::create_directory(projectPath);
		std::filesystem::create_directory(projectPath / "Assets");
		std::filesystem::create_directory(projectPath / "Scenes");
		std::filesystem::create_directory(projectPath / "Script");
	}

	// Create project and scene files
	{
		// Project file
		std::ofstream projectFile = std::ofstream(projectPath / fmt::format("{0}.lvproj", name));
		projectFile << "Project: " << name << '\n';
		projectFile << "Directories: " << '\n';
		projectFile << "  Assets: Assets" << '\n';
		projectFile << "  Script: Script" << '\n';
		projectFile << "  Scenes: Scenes" << '\n';
		projectFile << "Scenes: " << '\n';
		projectFile << "  - Scene: 1" << '\n';
		projectFile << "    Path: main.lvscene" << '\n';
		projectFile << "StartScene: 1" << '\n';
		projectFile << "Scripting: " << '\n';
		projectFile << "  Type: 1" << '\n';
		projectFile << "  Path: bin/Debug-windows/Script/Script.dll" << '\n';

		projectFile.close();

		// Main Scene file
		std::ofstream sceneFile = std::ofstream(projectPath / "Scenes" / "main.lvscene");
		sceneFile << "Scene: 1" << '\n';
		sceneFile << "Data: " << '\n';
		sceneFile << "  Entities: " << '\n';
		sceneFile << "    - Entity: 1" << '\n';
		sceneFile << "    TagComponent: " << '\n';
		sceneFile << "      Tag: First" << '\n';
		sceneFile << "    TransformComponent: " << '\n';
		sceneFile << "      Position: [0, 0, 0]" << '\n';
		sceneFile << "      Size: [1, 1, 1]" << '\n';
		sceneFile << "      Rotation: [0, 0, 0]" << '\n';

		sceneFile.close();
	}

	// Create Script files
	{
		// Create Directories
		std::filesystem::create_directory(projectPath/ "Script" / "scripts");
		std::filesystem::create_directory(projectPath/ "Script" / "src");

		// main.cpp file
		std::ofstream mainFile = std::ofstream(projectPath / "Script" / "src" / "main.cpp");
		mainFile << R"(
#include <Lavender/Scripting/C++/Mutual/ScriptableEntity.hpp>

#include <iostream>

using namespace Lavender;

class MyEntity : public Lavender::ScriptableEntity
{
public:
	void OnCreate() override
	{
		std::cout << "OnCreate: " << (uint64_t)m_UUID << std::endl;
	}

	void OnUpdate(float deltaTime) override
	{

	}
} LavenderEntity(MyEntity);
)";

		mainFile.close();

		// Batch file
		std::ofstream batFile = std::ofstream(projectPath / "Script" / "scripts" / "vs2022-Generate-Projects.bat");
		batFile << "@echo off" << "\n\n";
		batFile << "pushd %~dp0\\..\\" << "\n\n";
		batFile << "call ..\\..\\..\\..\\..\\vendor\\premake\\premake5.exe vs2022" << "\n\n";
		batFile << "popd" << '\n';
		batFile << "PAUSE";

		batFile.close();

		// Premake file
		std::ofstream premakeFile = std::ofstream(projectPath / "Script" / "premake5.lua");
		premakeFile << R"(
------------------------------------------------------------------------------
-- Utilities
------------------------------------------------------------------------------
function FirstToUpper(str)
	return (str:gsub("^%l", string.upper))
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Dependencies
------------------------------------------------------------------------------
Dependencies = 
{
	-- Libs
	Insight = 
	{
		LibName = "Insight",
		IncludeDir = "../../../../../vendor/Insight/Core/src",
	},

	-- Includes
	GLM = 
	{
		IncludeDir = "../../../../../vendor/glm"
	},
}
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Solution
------------------------------------------------------------------------------
outputdir = "%{cfg.buildcfg}-" .. FirstToUpper("%{cfg.system}")

workspace ")";
	premakeFile << name;
	premakeFile << R"("
	architecture "x86_64"
	startproject "Script"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

group "Dependencies"
	include "../../../../../vendor/Insight/Core"
group ""

------------------------------------------------------------------------------
-- Project
------------------------------------------------------------------------------
project "Script"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	
	architecture "x86_64"
	
	-- debugdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}") 
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
	}

	includedirs
	{
		"src",

		"../../../../../Core/src",

		"%{Dependencies.Insight.IncludeDir}",
		"%{Dependencies.GLM.IncludeDir}"
	}

	links
	{
		"Insight"
	}
	
	defines
	{
		"LAVENDER_DLL"
	}

	disablewarnings
	{
		"4005",
		"4996"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

		defines
		{
			"APP_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "APP_DEBUG"
		runtime "Debug"
		symbols "on"
		editandcontinue "Off"

	filter "configurations:Release"
		defines "APP_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "APP_DIST"
		runtime "Release"
		optimize "Full"
------------------------------------------------------------------------------
------------------------------------------------------------------------------
)";

		premakeFile.close();
	}
}