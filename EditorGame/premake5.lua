project "EditorGame"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	
	architecture "x86_64"
	
	-- debugdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}") 
	
	targetdir ("%{wks.location}/Editor/Resources/Editor/Game/%{cfg.buildcfg}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}") -- TODO: Maybe change

	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",

		"vendor/**.h",
		"vendor/**.hpp",
		"vendor/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/vendor",

		"%{wks.location}/Core/src",

		"%{Dependencies.GLFW.IncludeDir}",
		"%{Dependencies.GLM.IncludeDir}",
		"%{Dependencies.Spdlog.IncludeDir}",
		"%{Dependencies.Stb_image.IncludeDir}",
		"%{Dependencies.Assimp.IncludeDir}",
		"%{Dependencies.ImGui.IncludeDir}",
		"%{Dependencies.ImGuizmo.IncludeDir}",
		"%{Dependencies.Tracy.IncludeDir}",
		"%{Dependencies.EnTT.IncludeDir}",
		"%{Dependencies.VMA.IncludeDir}",
		"%{Dependencies.JoltPhysics.IncludeDir}",

		"%{Dependencies.Flow.IncludeDir}",
		"%{Dependencies.Insight.IncludeDir}"
	}

	links
	{
		"Lavender"
	}
	
	defines
	{
		"yaml_cpp_EXPORTS"
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
			"APP_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "APP_DEBUG"
		runtime "Debug"
		symbols "on"
		editandcontinue "Off"

		defines
		{
			"TRACY_ENABLE",
			"NOMINMAX",

			"JPH_DEBUG_RENDERER",
			"JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
			"JPH_EXTERNAL_PROFILE"
		}

	filter "configurations:Release"
		defines "APP_RELEASE"
		runtime "Release"
		optimize "on"

		defines
		{
			"TRACY_ENABLE",
			"NOMINMAX",

			"JPH_DEBUG_RENDERER",
			"JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
			"JPH_EXTERNAL_PROFILE"
		}

	filter "configurations:Dist"
		defines "APP_DIST"
		runtime "Release"
		optimize "Full"

	filter { "system:windows", "configurations:Debug" }
		postbuildcommands
		{
			'{COPYFILE} "%{Dependencies.Assimp.Windows.DebugDynamicLib}" "%{cfg.targetdir}"'
		}

	filter { "system:windows", "configurations:Release or configurations:Dist" }
		postbuildcommands
		{
			'{COPYFILE} "%{Dependencies.Assimp.Windows.DynamicLib}" "%{cfg.targetdir}"'
		}

	filter { "system:windows", "configurations:Dist" }
		-- Dist filter for Windows for Windowed Applications
		kind "WindowedApp"