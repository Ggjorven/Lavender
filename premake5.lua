include "Dependencies.lua"

workspace "Lavender"
	architecture "x86_64"
	startproject "Sandbox"

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

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/GLFW"
	include "vendor/ImGui"
	include "vendor/tracy"
	include "vendor/VulkanMemoryAllocator"
group ""

group "Lavender"
	include "Lavender"
group ""

include "Sandbox"