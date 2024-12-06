project "ImGuizmo"
	kind "StaticLib"
	language "C++"

	architecture "x86_64"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"GraphEditor.h",
		"GraphEditor.cpp",
		"ImCurveEdit.h",
		"ImCurveEdit.cpp",
		"ImGradient.h",
		"ImGradient.cpp",
		"ImGuizmo.h",
		"ImGuizmo.cpp",
		"ImSequencer.h",
		"ImSequencer.cpp",
		"ImZoomSlider.h"
	}

	includedirs
	{
		"%{Dependencies.ImGui.IncludeDir}"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++20"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++20"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
