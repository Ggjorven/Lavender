#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/PipelineLayout.hpp"

namespace Lavender
{

	class RenderCommandBuffer;
	class Pipeline;

	// Note(Jorben): Note that there is no 'Create' function since this is not supposed to be used by the client.
	class DescriptorSet
	{
	public:
		DescriptorSet() = default;
		virtual ~DescriptorSet() = default;

		virtual void Bind(Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer) = 0;
		virtual void Bind(Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer, size_t offset) = 0;
		virtual SetID GetSetID() = 0;
	};

	class DescriptorSetGroup
	{
	public:
		struct DescriptorCount
		{
		public:
			DescriptorCount();
			DescriptorCount(Dict<SetID, uint32_t> count);
			DescriptorCount(const DescriptorCount& other) = default;

			void SetCount(SetID set, uint32_t count);
			uint32_t GetCount(SetID set) const;

		private:
			Dict<SetID, uint32_t> m_Counts = { };
		};

	public:
		DescriptorSetGroup() = default;
		virtual ~DescriptorSetGroup() = default;

		virtual void AddMoreSetsTo(SetID id, uint32_t amount) = 0;

		virtual UniformLayout& GetLayout() = 0;
		virtual const DescriptorCount& GetCount() = 0;

		virtual std::vector<Ref<DescriptorSet>>& GetSets(SetID id) = 0;

		static Ref<DescriptorSetGroup> Create(const UniformLayout& layout, DescriptorCount count = {});
	};

}