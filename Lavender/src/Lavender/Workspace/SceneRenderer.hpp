#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Camera.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"

#include "Lavender/Workspace/Scene.hpp"

namespace Lavender
{

	class SceneRenderer
	{
	public:
		static void Init();
		static void Destroy();

		// TODO: Add some kind of custom/runtime camera as well
		static void RenderScene(Scene* scene, Ref<EditorCamera>& camera, Ref<RenderCommandBuffer> cmdBuffer);

	private:
		static std::vector<Ref<UniformBuffer>> s_ModelBuffers;
	};

}