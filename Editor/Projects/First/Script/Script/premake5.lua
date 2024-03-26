project "Script"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	architecture "x86_64"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
	}

	defines
	{
	}

	includedirs
	{
		"Script/src",
		"%{wks.location}/../../../../Lavender/src",
		"%{wks.location}/../../../../Lavender/src/Lavender",
		"%{wks.location}/../../../../Scripting/src",

		"%{wks.location}/../../../../vendor/glm",
		"%{wks.location}/../../../../vendor/spdlog/include"
	}

	links
	{
		"Scripting"
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

	filter "configurations:Release"
		defines "LV_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "LV_DIST"
		runtime "Release"
		optimize "Full"