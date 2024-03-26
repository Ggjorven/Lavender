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
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
		
		"%{wks.location}/vendor/stb_image/src/stb_image.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"yaml_cpp_EXPORTS"
		-- "ASSIMP_BUILD_STATIC_LIB"
	}

	includedirs
	{
		"src",
		"src/Lavender",

		"%{wks.location}/Scripting/src",

		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.tracy}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.VMA}"
	}

	links
	{
		"%{Library.Vulkan}",
		
		"GLFW",
		"ImGui",
		"yaml-cpp",
		"Tracy",
		"VMA"

--		"Scripting"
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
			"LV_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "LV_DEBUG"
		runtime "Debug"
		symbols "on"
		editandcontinue "Off"

		defines
		{
			"TRACY_ENABLE",
			"NOMINMAX"
		}

	filter "configurations:Release"
		defines "LV_RELEASE"
		runtime "Release"
		optimize "on"

		defines 
		{
			"TRACY_ENABLE",
			"NOMINMAX"
		}

	filter "configurations:Dist"
		defines "LV_DIST"
		runtime "Release"
		optimize "Full"

	filter { "system:windows", "configurations:Debug" }
		links
		{
			"%{wks.location}/vendor/assimp/bin/windows/Debug/assimp-vc143-mtd.lib"
		}

	filter { "system:windows", "configurations:Release" }
		links
		{
			"%{wks.location}/vendor/assimp/bin/windows/Release/assimp-vc143-mt.lib"
		}

	filter { "system:windows", "configurations:Dist" }
		links
		{
			"%{wks.location}/vendor/assimp/bin/windows/Release/assimp-vc143-mt.lib"
		}