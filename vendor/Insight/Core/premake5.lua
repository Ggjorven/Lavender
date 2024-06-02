project "Insight"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	architecture "x86_64"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "ispch.h"
	pchsource "src/Insight/ispch.cpp"

	files
	{
		"src/Insight/*.h",
		"src/Insight/*.cpp",
		"src/Insight/*.hpp",

		"src/Insight/Core/*.h",
		"src/Insight/Core/*.hpp",
		"src/Insight/Core/*.cpp",

		"src/Insight/Utils/*.h",
		"src/Insight/Utils/*.hpp",
		"src/Insight/Utils/*.cpp"
	}

	includedirs
	{
		"src",
		"src/Insight"
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

	filter "system:linux"
		systemversion "latest"
		staticruntime "on"

		defines
		{
			"APP_PLATFORM_LINUX"
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