#include "lvpch.h"
#include "Renderer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/RenderInstance.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

namespace Lavender
{

	static RenderInstance* s_RenderInstance = nullptr;
	RendererSpecification Renderer::s_Specification = {};

	void Renderer::Init()
	{
		s_RenderInstance = RenderInstance::Create();
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

	void Renderer::Submit(RenderFunction function)
	{
		s_RenderInstance->Submit(function);
	}

	void Renderer::WaitFor(Ref<RenderCommandBuffer> commandBuffer)
	{
		s_RenderInstance->WaitFor(commandBuffer);
	}

	void Renderer::Wait()
	{
		s_RenderInstance->Wait();
	}

	void Renderer::DrawIndexed(Ref<RenderCommandBuffer> commandBuffer, Ref<IndexBuffer> indexBuffer)
	{
		s_RenderInstance->DrawIndexed(commandBuffer, indexBuffer);
	}

	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		s_RenderInstance->OnResize(width, height);
	}

	RenderInstance* Renderer::GetInstance()
	{
		return s_RenderInstance;
	}

	inline RenderData Renderer::GetRenderData()
	{
		s_RenderInstance->GetRenderData();
	}

}