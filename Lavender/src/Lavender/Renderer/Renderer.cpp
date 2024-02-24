#include "lvpch.h"
#include "Renderer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/RenderInstance.hpp"

namespace Lavender
{

	static RenderInstance* s_RenderInstance = nullptr;
	RendererSpecification Renderer::s_Specifications = {};

	void Renderer::Init(const RendererSpecification& specs)
	{
		s_RenderInstance = RenderInstance::Create(specs);
		s_Specifications = specs;
	}

	void Renderer::Destroy()
	{
		delete s_RenderInstance;
	}

	void Renderer::BeginFrame()
	{
		s_RenderInstance->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		s_RenderInstance->EndFrame();
	}

	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		s_RenderInstance->OnResize(width, height);
	}

}