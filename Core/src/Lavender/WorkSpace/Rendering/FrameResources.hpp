#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/Buffers.hpp"
#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/RenderPass.hpp"
#include "Lavender/Renderer/Descriptors.hpp"
#include "Lavender/Renderer/CommandBuffer.hpp"

#include <glm/glm.hpp>

#define TILE_SIZE 16
#define MAX_POINTLIGHTS 1024
#define MAX_POINTLIGHTS_PER_TILE 64

namespace Lavender
{

	class FrameResources
	{
	public:
		FrameResources();
		virtual ~FrameResources();

		struct
		{
			Ref<Pipeline>				Pipeline = nullptr;
			Ref<RenderPass>				RenderPass = nullptr;
			Ref<DescriptorSets>			DescriptorSets = nullptr;
		} Depth;

		struct
		{
			Ref<Pipeline>				Pipeline = nullptr;
			Ref<DescriptorSets>			DescriptorSets = nullptr;

			Ref<ComputeShader>			ComputeShader = nullptr;
			Ref<CommandBuffer>			CommandBuffer = nullptr;

			Ref<StorageBuffer>			LightsBuffer = nullptr;
			Ref<StorageBuffer>			LightVisibilityBuffer = nullptr;
		} LightCulling;

		struct
		{
			Ref<Pipeline>				Pipeline = nullptr;
			Ref<Image2D>				Attachment = nullptr;
			Ref<RenderPass>				RenderPass = nullptr;
			Ref<DescriptorSets>			DescriptorSets = nullptr;
		} Shading;

		struct // Uses Shading.Attachment if Editor
		{
			Ref<Pipeline>				Pipeline = nullptr;
			Ref<RenderPass>				RenderPass = nullptr;
			Ref<DescriptorSets>			DescriptorSets = nullptr;
		} UI;

		struct Constants
		{
			inline static constexpr const uint32_t PreAllocatedModels = 100u;
			inline static constexpr const uint32_t PreAllocatedUISets = 10u;

			// To be set at runtime.
			inline static ShaderPath DepthVert = {};
			inline static ShaderPath DepthFrag = {};

			inline static ShaderPath LightCulling = {};

			inline static ShaderPath ShadingVert = {};
			inline static ShaderPath ShadingFrag = {};

			inline static ShaderPath UIVert = {};
			inline static ShaderPath UIFrag = {};
		};

		uint32_t AllocatedModels = Constants::PreAllocatedModels;
		uint32_t AllocatedUISets = Constants::PreAllocatedUISets;
		
		Ref<UniformBuffer>				SceneBuffer = nullptr;
		Ref<DynamicUniformBuffer>		ModelBuffer = nullptr;
		Ref<UniformBuffer>				CameraBuffer = nullptr;

	public:
		void Resize(uint32_t width, uint32_t height);

	private:
		void InitDepth(Ref<ShaderCompiler> compiler, Ref<ShaderCacher> cacher);
		void InitLightCulling(Ref<ShaderCompiler> compiler, Ref<ShaderCacher> cacher);
		void InitShading(Ref<ShaderCompiler> compiler, Ref<ShaderCacher> cacher);
		void InitUI(Ref<ShaderCompiler> compiler, Ref<ShaderCacher> cacher);
		void InitResources();
	};



	// Additional shader resources
	namespace ShaderResource
	{

		struct Scene
		{
		public:
			glm::uvec2 ScreenSize = { 0u, 0u };
		};

		struct Model
		{
		public:
			glm::mat4 Matrix = glm::mat4(1.0f);
		};
	
	}

}