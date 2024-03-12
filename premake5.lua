include "Dependencies.lua"

workspace "Lavender"
	architecture "x86_64"
	startproject "Editor"

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
	include "vendor/yaml-cpp"
	include "vendor/VulkanMemoryAllocator"
group ""

group "Lavender"
	include "Lavender"
	include "Scripting"
group ""

include "Editor"