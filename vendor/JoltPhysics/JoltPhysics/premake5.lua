project "JoltPhysics"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Jolt/**.cpp",
        "Jolt/**.h",
        "Jolt/**.inl",
        "Jolt/**.gliffy"
    }

    includedirs 
    { 
        "../JoltPhysics",
        "Jolt"
    }

    filter "system:windows"
        systemversion "latest"

        files 
        { 
            "Jolt/Jolt.natvis" 
        }

    filter "configurations:Debug"
        symbols "on"
        optimize "off"

        defines
        {
            "_DEBUG",
            "JPH_DEBUG_RENDERER",
            "JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
            "JPH_EXTERNAL_PROFILE"
        }

    filter "configurations:Release"
        optimize "speed"
        vectorextensions "AVX2"
        isaextensions { "BMI", "POPCNT", "LZCNT", "F16C" }

        defines
        {
            "JPH_DEBUG_RENDERER",
            "JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
            "JPH_EXTERNAL_PROFILE"
        }

    filter "configurations:Dist"
        optimize "speed"
        symbols "off"
        vectorextensions "AVX2"
        isaextensions { "BMI", "POPCNT", "LZCNT", "F16C" }