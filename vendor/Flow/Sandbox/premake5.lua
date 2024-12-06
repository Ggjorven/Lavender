project "Sandbox"
	kind "ConsoleApp"
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

		"vendor/**.h",
		"vendor/**.hpp",
		"vendor/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/vendor",

		"%{wks.location}/Core/src",
	}

	links
	{
		"Flow"
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