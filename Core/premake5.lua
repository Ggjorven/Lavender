project "Lavender"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	architecture "x86_64"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "lvpch.h"
	pchsource "src/Lavender/lvpch.cpp"

	files
	{
		"src/Lavender/*.h",
		"src/Lavender/*.hpp",
		"src/Lavender/*.cpp",

		"src/Lavender/Core/**.h",
		"src/Lavender/Core/**.hpp",
		"src/Lavender/Core/**.cpp",

		"src/Lavender/FileSystem/**.h",
		"src/Lavender/FileSystem/**.hpp",
		"src/Lavender/FileSystem/**.cpp",
		
		"src/Lavender/Renderer/**.h",
		"src/Lavender/Renderer/**.hpp",
		"src/Lavender/Renderer/**.cpp",

		"src/Lavender/Utils/**.h",
		"src/Lavender/Utils/**.hpp",
		"src/Lavender/Utils/**.cpp",

		"src/Lavender/UI/**.h",
		"src/Lavender/UI/**.hpp",
		"src/Lavender/UI/**.cpp",

		"src/Lavender/Vulkan/**.h",
		"src/Lavender/Vulkan/**.hpp",
		"src/Lavender/Vulkan/**.cpp",

		"src/Lavender/ECS/**.h",
		"src/Lavender/ECS/**.hpp",
		"src/Lavender/ECS/**.cpp",

		"src/Lavender/WorkSpace/**.h",
		"src/Lavender/WorkSpace/**.hpp",
		"src/Lavender/WorkSpace/**.cpp",

		"src/Lavender/Platforms/" .. FirstToUpper("%{cfg.system}") .. "/**.h",
		"src/Lavender/Platforms/" .. FirstToUpper("%{cfg.system}") .. "/**.hpp",
		"src/Lavender/Platforms/" .. FirstToUpper("%{cfg.system}") .. "/**.cpp",

		"%{wks.location}/vendor/stb/src/stb_image.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"yaml_cpp_EXPORTS"
	}

	includedirs
	{
		"src",
		"src/Lavender",

		"%{Dependencies.GLFW.IncludeDir}",
		"%{Dependencies.GLM.IncludeDir}",
		"%{Dependencies.Spdlog.IncludeDir}",
		"%{Dependencies.Stb_image.IncludeDir}",
		"%{Dependencies.Assimp.IncludeDir}",
		"%{Dependencies.ImGui.IncludeDir}",
		"%{Dependencies.Tracy.IncludeDir}",
		"%{Dependencies.EnTT.IncludeDir}",
		"%{Dependencies.VMA.IncludeDir}",

		"%{Dependencies.Flow.IncludeDir}",
		"%{Dependencies.Insight.IncludeDir}"
	}

	links
	{
		"%{Dependencies.GLFW.LibName}",
		"%{Dependencies.VMA.LibName}",
		"%{Dependencies.ImGui.LibName}",

		"%{Dependencies.Flow.LibName}"
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

		includedirs
		{
			"%{Dependencies.Vulkan.Windows.IncludeDir}"
		}

		links
		{
			"%{Dependencies.Tracy.LibName}",
			"%{Dependencies.Vulkan.Windows.LibDir}" .. "%{Dependencies.ShaderC.LibName}",
			"%{Dependencies.Vulkan.Windows.LibDir}" .. "%{Dependencies.Vulkan.Windows.LibName}"
		}

	filter "system:linux"
		systemversion "latest"
		staticruntime "on"

		defines
		{
			"APP_PLATFORM_LINUX"
		}

		includedirs
		{
			"%{Dependencies.Vulkan.Linux.IncludeDir}"
		}

		links
		{
			"%{Dependencies.Vulkan.Linux.LibDir}" .. "%{Dependencies.Vulkan.Linux.LibName}",
			"%{Dependencies.Vulkan.Linux.LibDir}" .. "%{Dependencies.ShaderC.LibName}"
		}

	filter "configurations:Debug"
		defines "APP_DEBUG"
		runtime "Debug"
		symbols "on"
		editandcontinue "Off"

		defines
		{
			"TRACY_ENABLE",
			"NOMINMAX"
		}

	filter "configurations:Release"
		defines "APP_RELEASE"
		runtime "Release"
		optimize "on"

		defines 
		{
			"TRACY_ENABLE",
			"NOMINMAX"
		}

	filter "configurations:Dist"
		defines "APP_DIST"
		runtime "Release"
		optimize "Full"

	-- Windows
	filter { "system:windows", "configurations:Debug" }
	links
	{
		"%{Dependencies.Assimp.Windows.LibDir}" .. "%{Dependencies.Assimp.Windows.DebugLibName}"
	}

	filter { "system:windows", "configurations:Release or configurations:Dist" }
		links
		{
			"%{Dependencies.Assimp.Windows.LibDir}" .. "%{Dependencies.Assimp.Windows.LibName}"
		}

	-- Linux
	filter { "system:linux", "configurations:Debug" }
		links
		{
			"%{Dependencies.Assimp.Linux.LibDir}" .. "%{Dependencies.Assimp.Linux.DebugLibName}"
		}

	filter { "system:linux", "configurations:Release or configurations:Dist" }
		links
		{
			"%{Dependencies.Assimp.Linux.LibDir}" .. "%{Dependencies.Assimp.Linux.LibName}"
		}