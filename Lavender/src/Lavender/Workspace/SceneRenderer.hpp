#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Camera.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/Image.hpp"

#include "Lavender/Workspace/Scene.hpp"

namespace Lavender
{

	struct LightSettings
	{
	public:
		std::vector<PointLightComponent> PointLights = { };
		uint32_t AmountOfPointLights = 0;

	public:
		LightSettings(uint32_t amountOfPointLights, size_t sizeOfPointLights)
			: AmountOfPointLights(amountOfPointLights), PointLights(sizeOfPointLights)
		{
		}

		inline constexpr static size_t GetSize(uint32_t amountOfPointLights) { return amountOfPointLights * sizeof(PointLightComponent) + sizeof(uint32_t); }

		inline void AddDataTo(Ref<UniformBuffer> buffer)
		{
			buffer->SetData((void*)PointLights.data(), PointLights.size() * sizeof(PointLightComponent));
			buffer->SetData((void*)&AmountOfPointLights, sizeof(uint32_t), PointLights.size() * sizeof(PointLightComponent));
		}
	};

	struct SceneData
	{
	public:
		glm::vec3 ViewPosition = {};
	};

	class SceneRenderer
	{
	public:
		typedef entt::basic_view<entt::get_t<entt::constness_as_t<entt::storage_type_t<Lavender::MeshComponent, entt::entity, std::allocator<Lavender::MeshComponent>>, Lavender::MeshComponent>>, entt::exclude_t<>, void> DrawView;
	public:
		static void Init();
		static void Destroy();

		// TODO: Add some kind of custom/runtime camera as well
		static void RenderScene(Scene* scene, const glm::vec3& cameraPosition, Ref<RenderCommandBuffer> cmdBuffer);

	private:
		static void UpdateLights(Scene* scene, Ref<Pipeline> pipeline);
		static void UpdateSceneData(Scene* scene, const glm::vec3& cameraPosition, Ref<Pipeline> pipeline);
		static void UpdateBuffers(Scene* scene, Ref<Pipeline> pipeline);

		static void RenderModels(Scene* scene, Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer);

	private:
		static Ref<Image2D> s_EmptyImage;
		
		static Ref<DynamicUniformBuffer> s_ModelBuffer;
		static Ref<DynamicUniformBuffer> s_MaterialBuffer;

		static Ref<UniformBuffer> s_LightsBuffer;
		static Ref<UniformBuffer> s_SceneDataBuffer;
	};

}