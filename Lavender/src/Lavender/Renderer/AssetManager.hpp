#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Pipeline.hpp"

namespace Lavender
{

	class AssetManager
	{
	public:
		enum class PipelineType : uint8_t
		{
			None = 0,
			MeshAndImage
		};

	public:
		AssetManager(Ref<RenderPass> renderpass);
		virtual ~AssetManager();

		Ref<Pipeline> GetPipeline(PipelineType type) { return m_Pipelines[type]; }

		static Ref<AssetManager> Create(Ref<RenderPass> renderpass);

	private:
		Ref<RenderPass> m_RenderPass = nullptr;

		std::unordered_map<PipelineType, Ref<Pipeline>> m_Pipelines = { };
	};
}