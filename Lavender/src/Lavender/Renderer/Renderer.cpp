#include "lvpch.h"
#include "Renderer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/RenderInstance.hpp"

namespace Lavender
{

	static RenderInstance* s_RenderInstance = nullptr;

	void Renderer::Init()
	{
		s_RenderInstance = RenderInstance::Create();
	}

	void Renderer::Destroy()
	{
		delete s_RenderInstance;
	}

	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		s_RenderInstance->OnResize(width, height);
	}

}