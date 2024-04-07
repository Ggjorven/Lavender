#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Camera.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/Image.hpp"

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
		static Ref<DynamicUniformBuffer> s_ModelBuffer;
		static Ref<UniformBuffer> s_DirectionalLightBuffer;
		static Ref<Image2D> s_EmptyImage;

		static Ref<UniformBuffer> s_SceneDataBuffer;
	};

}