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
		typedef entt::basic_view<entt::get_t<entt::constness_as_t<entt::storage_type_t<Lavender::MeshComponent, entt::entity, std::allocator<Lavender::MeshComponent>>, Lavender::MeshComponent>>, entt::exclude_t<>, void> DrawView;
	public:
		static void Init();
		static void Destroy();

		// TODO: Add some kind of custom/runtime camera as well
		static void RenderScene(Scene* scene, Ref<RenderCommandBuffer> cmdBuffer);

	private:
		static void UpdateLights(Scene* scene, Ref<Pipeline> pipeline);
		static void UpdateSceneData(Scene* scene, Ref<Pipeline> pipeline);
		static void UpdateModels(Scene* scene, Ref<Pipeline> pipeline);

		static void RenderModels(Scene* scene, Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer);

	private:
		static Ref<DynamicUniformBuffer> s_ModelBuffer;
		static Ref<UniformBuffer> s_LightBuffer;
		static Ref<Image2D> s_EmptyImage;

		static Ref<UniformBuffer> s_SceneDataBuffer;
	};

}