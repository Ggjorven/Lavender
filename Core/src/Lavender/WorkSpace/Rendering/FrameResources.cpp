#include "lvpch.h"
#include "FrameResources.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Renderer/Mesh.hpp"
#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/ECS/ShaderResources.hpp"

#include "Lavender/WorkSpace/EngineTracker.hpp"

namespace Lavender
{

	FrameResources::FrameResources()
	{
		Ref<ShaderCompiler> compiler = ShaderCompiler::Create();
		Ref<ShaderCacher> cacher = ShaderCacher::Create();

		// Initialize constants
		if (Constants::DepthVert.Shader.empty())
		{
			Constants::DepthVert = { Track::Lavender::Directory / "Editor/assets/shaders/Depth.vert.glsl", Track::Lavender::Directory / "Editor/assets/shaders/caches/Depth.vert.cache" };
			Constants::DepthFrag = { Track::Lavender::Directory / "Editor/assets/shaders/Depth.frag.glsl", Track::Lavender::Directory / "Editor/assets/shaders/caches/Depth.frag.cache" };
			Constants::LightCulling = { Track::Lavender::Directory / "Editor/assets/shaders/LightCulling.comp.glsl", Track::Lavender::Directory / "Editor/assets/shaders/caches/LightCulling.comp.cache" };
			Constants::ShadingVert = { Track::Lavender::Directory / "Editor/assets/shaders/Shading.vert.glsl", Track::Lavender::Directory / "Editor/assets/shaders/caches/Shading.vert.cache" };
			Constants::ShadingFrag = { Track::Lavender::Directory / "Editor/assets/shaders/Shading.frag.glsl", Track::Lavender::Directory / "Editor/assets/shaders/caches/Shading.frag.cache" };
		}

		// Initialize all resources
		InitDepth(compiler, cacher);
		InitLightCulling(compiler, cacher);
		InitShading(compiler, cacher);
		InitResources();
	}

	FrameResources::~FrameResources()
	{
		// Depth
		Depth.Pipeline.reset();
		Depth.RenderPass.reset();
		Depth.DescriptorSets.reset();

		// LightCulling
		LightCulling.Pipeline.reset();
		LightCulling.DescriptorSets.reset();

		LightCulling.ComputeShader.reset();
		LightCulling.CommandBuffer.reset();

		LightCulling.LightsBuffer.reset();
		LightCulling.LightVisibilityBuffer.reset();

		// Shading
		Shading.Pipeline.reset();
		Shading.RenderPass.reset();
		Shading.DescriptorSets.reset();

		// FrameResources
		SceneBuffer.reset();
		ModelBuffer.reset();
		CameraBuffer.reset();
	}

	void FrameResources::Resize(uint32_t width, uint32_t height)
	{
		// Depth
		{
			Depth.RenderPass->Resize(width, height);
		}

		// LightCulling
		{
			uint32_t tiles = ((width + TILE_SIZE - 1) / TILE_SIZE) * ((height + TILE_SIZE - 1) / TILE_SIZE);
			size_t size = ((sizeof(uint32_t) + (sizeof(char) * 12) + ((sizeof(uint32_t) + (sizeof(char) * 12)) * MAX_POINTLIGHTS_PER_TILE))) * tiles;
			LightCulling.LightVisibilityBuffer = StorageBuffer::Create(size);
		}

		// Shading
		{
			Shading.RenderPass->Resize(width, height);
		}
	}

	void FrameResources::InitDepth(Ref<ShaderCompiler> compiler, Ref<ShaderCacher> cacher)
	{
		Depth.DescriptorSets = DescriptorSets::Create(
		{
			// Set 0
			{ Constants::PreAllocatedModels, { 0, {
				{ DescriptorType::DynamicUniformBuffer, 0, "u_Model", ShaderStage::Vertex }
			}}},

			// Set 1
			{ 1, { 1, {
				{ DescriptorType::UniformBuffer, 0, "u_Camera", ShaderStage::Vertex }
			}}}
		});

		CommandBufferSpecification cmdBufSpecs = {};
		cmdBufSpecs.Usage = CommandBufferUsage::Sequence;

		auto cmdBuf = CommandBuffer::Create(cmdBufSpecs);

		RenderPassSpecification renderPassSpecs = {};
		renderPassSpecs.DepthLoadOp = LoadOperation::Clear;
		renderPassSpecs.DepthAttachment = Renderer::GetDepthImage();
		renderPassSpecs.PreviousDepthImageLayout = ImageLayout::Depth;
		renderPassSpecs.FinalDepthImageLayout = ImageLayout::Depth;

		Depth.RenderPass = RenderPass::Create(renderPassSpecs, cmdBuf);

		ShaderSpecification shaderSpecs = {};
		shaderSpecs.Vertex = cacher->GetLatest(compiler, Constants::DepthVert.Cache, Constants::DepthVert.Shader, ShaderStage::Vertex);
		shaderSpecs.Fragment = cacher->GetLatest(compiler, Constants::DepthFrag.Cache, Constants::DepthFrag.Shader, ShaderStage::Fragment);

		auto shader = Shader::Create(shaderSpecs);

		PipelineSpecification pipelineSpecs = {};
		pipelineSpecs.Bufferlayout = MeshVertex::GetLayout();
		pipelineSpecs.Polygonmode = PolygonMode::Fill;
		pipelineSpecs.Cullingmode = CullingMode::None;
		pipelineSpecs.LineWidth = 1.0f;
		pipelineSpecs.Blending = false;

		Depth.Pipeline = Pipeline::Create(pipelineSpecs, Depth.DescriptorSets, shader, Depth.RenderPass);
	}

	void FrameResources::InitLightCulling(Ref<ShaderCompiler> compiler, Ref<ShaderCacher> cacher)
	{
		LightCulling.DescriptorSets = DescriptorSets::Create(
		{
			// Set 0
			{ 1, { 0, {
				{ DescriptorType::Image, 0, "u_DepthBuffer", ShaderStage::Compute },
				{ DescriptorType::StorageBuffer, 1, "u_Lights", ShaderStage::Compute },
				{ DescriptorType::StorageBuffer, 2, "u_Visibility", ShaderStage::Compute }
			}}},

			// Set 1
			{ 1, { 1, {
				{ DescriptorType::UniformBuffer, 0, "u_Camera", ShaderStage::Compute },
				{ DescriptorType::UniformBuffer, 1, "u_Scene", ShaderStage::Compute }
			}}},
		});

		{
			size_t size = sizeof(uint32_t) + (sizeof(char) * 12) + (sizeof(ShaderResource::PointLight) * MAX_POINTLIGHTS);
			LightCulling.LightsBuffer = StorageBuffer::Create(size);

			uint32_t tiles = ((Track::Viewport::Width + TILE_SIZE - 1) / TILE_SIZE) * ((Track::Viewport::Height + TILE_SIZE - 1) / TILE_SIZE);
			size = ((sizeof(uint32_t) + (sizeof(char) * 12) + ((sizeof(uint32_t) + (sizeof(char) * 12)) * MAX_POINTLIGHTS_PER_TILE))) * tiles;
			LightCulling.LightVisibilityBuffer = StorageBuffer::Create(size);
		}

		CommandBufferSpecification cmdBufSpecs = {};
		cmdBufSpecs.Usage = CommandBufferUsage::Sequence;

		LightCulling.CommandBuffer = CommandBuffer::Create(cmdBufSpecs);

		ShaderSpecification shaderSpecs = {};
		shaderSpecs.Compute = cacher->GetLatest(compiler, Constants::LightCulling.Cache, Constants::LightCulling.Shader, ShaderStage::Compute);

		LightCulling.ComputeShader = ComputeShader::Create(shaderSpecs);
		LightCulling.Pipeline = Pipeline::Create({ }, LightCulling.DescriptorSets, LightCulling.ComputeShader);
	}

	void FrameResources::InitShading(Ref<ShaderCompiler> compiler, Ref<ShaderCacher> cacher)
	{
		Shading.DescriptorSets = DescriptorSets::Create(
		{
			// Set 0
			{ Constants::PreAllocatedModels, { 0, {
				{ DescriptorType::DynamicUniformBuffer, 0, "u_Model", ShaderStage::Vertex },
				{ DescriptorType::Image, 1, "u_Albedo", ShaderStage::Fragment },
				{ DescriptorType::StorageBuffer, 2, "u_Lights", ShaderStage::Fragment },
				{ DescriptorType::StorageBuffer, 3, "u_Visibility", ShaderStage::Fragment }
			}}},

			// Set 1
			{ 1, { 1, {
				{ DescriptorType::UniformBuffer, 0, "u_Camera", ShaderStage::Vertex },
				{ DescriptorType::UniformBuffer, 1, "u_Scene", ShaderStage::Fragment }
			}}}
		});

		CommandBufferSpecification cmdBufSpecs = {};
		cmdBufSpecs.Usage = CommandBufferUsage::Sequence;

		auto cmdBuf = CommandBuffer::Create(cmdBufSpecs);

		RenderPassSpecification renderPassSpecs = {};
		renderPassSpecs.ColourAttachment = Renderer::GetSwapChainImages();
		renderPassSpecs.ColourLoadOp = LoadOperation::Clear;
		renderPassSpecs.ColourClearColour = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassSpecs.PreviousColourImageLayout = ImageLayout::Undefined;
		renderPassSpecs.FinalColourImageLayout = ImageLayout::Presentation;

		renderPassSpecs.DepthLoadOp = LoadOperation::Clear;
		renderPassSpecs.DepthAttachment = Renderer::GetDepthImage();
		renderPassSpecs.PreviousDepthImageLayout = ImageLayout::DepthRead;
		renderPassSpecs.FinalDepthImageLayout = ImageLayout::Depth;

		Shading.RenderPass = RenderPass::Create(renderPassSpecs, cmdBuf);

		ShaderSpecification shaderSpecs = {};
		shaderSpecs.Vertex = cacher->GetLatest(compiler, Constants::ShadingVert.Cache, Constants::ShadingVert.Shader, ShaderStage::Vertex);
		shaderSpecs.Fragment = cacher->GetLatest(compiler, Constants::ShadingFrag.Cache, Constants::ShadingFrag.Shader, ShaderStage::Fragment);

		auto shader = Shader::Create(shaderSpecs);

		PipelineSpecification pipelineSpecs = {};
		pipelineSpecs.Bufferlayout = MeshVertex::GetLayout();
		pipelineSpecs.Polygonmode = PolygonMode::Fill;
		pipelineSpecs.Cullingmode = CullingMode::None;
		pipelineSpecs.LineWidth = 1.0f;
		pipelineSpecs.Blending = false;

		Shading.Pipeline = Pipeline::Create(pipelineSpecs, Shading.DescriptorSets, shader, Shading.RenderPass);
	}

	void FrameResources::InitResources()
	{
		SceneBuffer = UniformBuffer::Create(sizeof(ShaderResource::Scene));

		ModelBuffer = DynamicUniformBuffer::Create(Constants::PreAllocatedModels, sizeof(ShaderResource::Model));
		CameraBuffer = UniformBuffer::Create(sizeof(ShaderResource::Camera));
	}

}