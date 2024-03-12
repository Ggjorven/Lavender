workspace "First"
    architecture "x86_64"
    startproject "Script"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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

group "Lavender"
    include "../../../../Scripting"
group ""

include "Script"