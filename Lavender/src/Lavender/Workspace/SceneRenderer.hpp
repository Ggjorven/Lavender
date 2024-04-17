#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Camera.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/Image.hpp"

#include "Lavender/Workspace/Scene.hpp"

namespace Lavender
{

	struct ShaderPointLight
	{
	public:
		alignas(16) glm::vec3 Position = { 0.0f, 0.0f, 0.0f }; 
		alignas(16) glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f }; 

		alignas(16) glm::vec3 Ambient = {};
		alignas(16) glm::vec3 Diffuse = {};
		alignas(16) glm::vec3 Specular = {};

		float Constant = 0.5f;
		float Linear = 0.5f;
		float Quadratic = 0.5f;
		PUBLIC_PADDING(0, 4);

	public:
		ShaderPointLight() = default;
		ShaderPointLight(const PointLightComponent& component)
			: Ambient(component.Ambient), Diffuse(component.Diffuse), Specular(component.Specular),
			Constant(component.Constant), Linear(component.Linear), Quadratic(component.Quadratic)
		{
		}
	};

	struct LightSettings
	{
	public:
		std::vector<ShaderPointLight> PointLights = { };
		uint32_t AmountOfPointLights = 0;

	public:
		LightSettings(uint32_t amountOfPointLights, size_t sizeOfPointLights)
			: AmountOfPointLights(amountOfPointLights), PointLights(sizeOfPointLights)
		{
		}

		inline constexpr static size_t GetSize(uint32_t amountOfPointLights) { return amountOfPointLights * sizeof(ShaderPointLight) + sizeof(uint32_t); }

		inline void AddDataTo(Ref<UniformBuffer> buffer)
		{
			buffer->SetData((void*)PointLights.data(), PointLights.size() * sizeof(ShaderPointLight));
			buffer->SetData((void*)&AmountOfPointLights, sizeof(uint32_t), PointLights.size() * sizeof(ShaderPointLight));
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