#include "lvpch.h"

#define IMGUI_IMPL_VULKAN
#include <backends/imgui_impl_vulkan.cpp>

//#define IMGUI_IMPL_OPENGL_LOADER_GLEW		 // TODO: Add back when OpenGL is added (if OpenGL is added)
//#include <backends/imgui_impl_opengl3.cpp> // TODO: Add back when OpenGL is added (if OpenGL is added)

// Note(Jorben): GLFW is our windowing library
#include <backends/imgui_impl_glfw.cpp>