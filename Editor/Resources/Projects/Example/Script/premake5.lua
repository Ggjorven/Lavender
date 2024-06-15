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
	GLFW = 
	{
		LibName = "GLFW",
		IncludeDir = "../../../../../vendor/GLFW/include"
	},
	Tracy = 
	{
		LibName = "Tracy",
		IncludeDir = "../../../../../vendor/tracy/tracy/public"
	},

	Assimp = 
	{
		IncludeDir = "../../../../../vendor/assimp/include",

		Windows = 
		{
			LibName = "assimp-vc143-mt",
			DebugLibName = "assimp-vc143-mtd",
			LibDir = "../../../../../vendor/assimp/bin/windows/",
			DynamicLib = "%{Dependencies.Assimp.Windows.LibDir}" .. "assimp-vc143-mt.dll",
			DebugDynamicLib = "%{Dependencies.Assimp.Windows.LibDir}" .. "assimp-vc143-mtd.dll",
		},
		Linux =  
		{
			LibName = "libassimp.so",				-- TODO: Check this out
			DebugDynamicLibName = "libassimp.so.5",	-- TODO: Check this out
			LibDir = "../../../../../vendor/assimp/bin/linux/",
		}
	},
	ImGui = 
	{
		IncludeDir = "../../../../../vendor/imgui",
		LibName = "ImGui"
	},

	-- Custom Libs
	Flow = 
	{
		LibName = "Flow",
		IncludeDir = "../../../../../vendor/Flow/Core/src",
	},
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
	Spdlog = 
	{
		IncludeDir = "../../../../../vendor/spdlog/include"
	},
	Stb_image =
	{
		IncludeDir = "../../../../../vendor/stb/include"
	},
	EnTT = 
	{
		IncludeDir = "../../../../../vendor/entt/include"
	}
}
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Solution
------------------------------------------------------------------------------
outputdir = "%{cfg.buildcfg}-" .. FirstToUpper("%{cfg.system}")

workspace "Example"
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

		"%{Dependencies.GLFW.IncludeDir}",
		"%{Dependencies.GLM.IncludeDir}",
		"%{Dependencies.Spdlog.IncludeDir}",
		"%{Dependencies.Stb_image.IncludeDir}",
		"%{Dependencies.Assimp.IncludeDir}",
		"%{Dependencies.ImGui.IncludeDir}",
		"%{Dependencies.Tracy.IncludeDir}",
		"%{Dependencies.EnTT.IncludeDir}",

		"%{Dependencies.Flow.IncludeDir}",
		"%{Dependencies.Insight.IncludeDir}"
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
			
			
	-- TODO: Change the way we do linking on actual dist/binary builds
	-- filter { "system:windows" }
	-- 	links
	-- 	{
	-- 		"../../../../../bin/%{cfg.buildcfg}-windows/Lavender/Lavender.lib"
	-- 	}
------------------------------------------------------------------------------
------------------------------------------------------------------------------