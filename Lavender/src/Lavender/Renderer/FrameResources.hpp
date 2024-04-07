#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Pipeline.hpp"

namespace Lavender
{

	class Project;

	struct SceneData
	{
	public:
		uint32_t DirectionalLights = 0;
	};
	
	class FrameResources
	{
	public:
		static void Init(Project* project);
		static void Destroy();

		inline static Ref<Pipeline> GetPipeline() { return s_Pipeline; }

	private:
		static void CreatePipeline();

	private:
		static Ref<Pipeline> s_Pipeline;
	};

}
