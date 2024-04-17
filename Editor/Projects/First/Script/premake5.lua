workspace "LavenderScript"
    architecture "x86_64"
    startproject "Script"

	outputdir = "%{cfg.buildcfg}"

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