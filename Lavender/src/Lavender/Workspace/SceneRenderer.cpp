#include "lvpch.h"
#include "SceneRenderer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/FrameResources.hpp"
#include "Lavender/Renderer/DescriptorSet.hpp"

#include "Lavender/ECS/Components.hpp"

#include "Lavender/Workspace/Assets/MeshAsset.hpp"
#include "Lavender/Workspace/Assets/MaterialAsset.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Lavender
{

	static uint32_t s_EntitiesAllocated = 0; // Is used for images and uniforms
	//static uint32_t s_DirLightsAllocated = 0; // Is used for directional lights and more
	static uint32_t s_LightsAllocated = 0;
	static size_t s_UniformSize = sizeof(glm::mat4); // glm::mat4 'cause of Model matrix

	Ref<DynamicUniformBuffer> SceneRenderer::s_ModelBuffer = nullptr;
	Ref<UniformBuffer> SceneRenderer::s_LightBuffer = nullptr;
	Ref<Image2D> SceneRenderer::s_EmptyImage = nullptr;

	Ref<UniformBuffer> SceneRenderer::s_SceneDataBuffer = nullptr;

	static SceneRenderer::DrawView s_Entities = {};

	void SceneRenderer::Init()
	{
		s_ModelBuffer = DynamicUniformBuffer::Create(Renderer::GetSpecification().PreAllocatedDescriptorSets, s_UniformSize);
		s_EntitiesAllocated = Renderer::GetSpecification().PreAllocatedDescriptorSets;
		
		//s_LightBuffer = UniformBuffer::Create(sizeof(DirectionalLightComponent) * Renderer::GetSpecification().PreAllocatedLightSlots);
		//s_DirLightsAllocated = Renderer::GetSpecification().PreAllocatedLightSlots;

		ImageSpecification specs = {};
		specs.Usage = ImageSpecification::ImageUsage::Size;
		specs.Flags = ImageSpecification::ImageUsageFlags::Sampled;
		specs.Width = 1;
		specs.Height = 1;
		s_EmptyImage = Image2D::Create(specs);

		s_SceneDataBuffer = UniformBuffer::Create(sizeof(SceneData));
	}

	void SceneRenderer::Destroy()
	{
		s_ModelBuffer.reset();
		//s_DirectionalLightBuffer.reset();
		s_EmptyImage.reset();

		s_SceneDataBuffer.reset();
	}

	void SceneRenderer::RenderScene(Scene* scene, Ref<RenderCommandBuffer> cmdBuffer)
	{
		LV_PROFILE_SCOPE("SceneRenderer::RenderScene");

		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();
		auto pipeline = FrameResources::GetPipeline();

		SceneRenderer::UpdateLights(scene, pipeline);
		SceneRenderer::UpdateSceneData(scene, pipeline);
		SceneRenderer::UpdateModels(scene, pipeline);

		SceneRenderer::RenderModels(scene, pipeline, cmdBuffer);
	}

	void SceneRenderer::UpdateLights(Scene* scene, Ref<Pipeline> pipeline)
	{
		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();
		/*
		auto dirView = registry.view<DirectionalLightComponent>();
		// Check if we need to resize
		if (dirView.size() > s_DirLightsAllocated)
		{
			s_DirectionalLightBuffer.reset();
			s_DirectionalLightBuffer = UniformBuffer::Create(sizeof(DirectionalLightComponent) * dirView.size());

			s_DirLightsAllocated = (uint32_t)dirView.size();
		}

		// Upload lights
		uint32_t index = 0;
		std::vector<DirectionalLightComponent> lights((size_t)s_DirLightsAllocated);
		for (auto& entity : dirView)
		{
			DirectionalLightComponent& light = dirView.get<DirectionalLightComponent>(entity);
			lights[index] = light;
			index++;
		}
		size_t size = sizeof(DirectionalLightComponent) * s_DirLightsAllocated;
		s_DirectionalLightBuffer->SetData((void*)lights.data(), size);
		s_DirectionalLightBuffer->Upload(pipeline->GetDescriptorSets()->GetSets(1)[0], pipeline->GetSpecification().Uniformlayout.GetElementByName(1, "u_DirectionalLights"));
		*/
	}

	void SceneRenderer::UpdateSceneData(Scene* scene, Ref<Pipeline> pipeline)
	{
		SceneData data = {};
		//data.DirectionalLights = (uint32_t)dirView.size();
		//
		//s_SceneDataBuffer->SetData((void*)&data, sizeof(SceneData));
		//s_SceneDataBuffer->Upload(pipeline->GetDescriptorSets()->GetSets(1)[0], pipeline->GetSpecification().Uniformlayout.GetElementByName(1, "u_SceneData"));
	}

	void SceneRenderer::UpdateModels(Scene* scene, Ref<Pipeline> pipeline)
	{
		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();
		s_Entities = registry.view<MeshComponent>();

		// Check if we need to resize
		if (s_Entities.size() > s_EntitiesAllocated)
		{
			s_ModelBuffer.reset();
			s_ModelBuffer = DynamicUniformBuffer::Create((uint32_t)s_Entities.size(), s_UniformSize);
			pipeline->GetDescriptorSets()->AddMoreSetsTo(0, (uint32_t)s_Entities.size() - s_EntitiesAllocated);

			s_EntitiesAllocated = (uint32_t)s_Entities.size();
		}

		// Upload model matrices to dynamic buffer
		uint32_t index = 0;
		std::vector<glm::mat4> matrices((size_t)s_EntitiesAllocated, glm::mat4(1.0f));
		for (auto& entity : s_Entities)
		{
			MeshComponent& mesh = s_Entities.get<MeshComponent>(entity);

			auto transformmeshView = registry.view<TransformComponent>();
			if (transformmeshView.contains(entity))
			{
				TransformComponent& transform = transformmeshView.get<TransformComponent>(entity);

				matrices[index] = glm::translate(matrices[index], transform.Position);
				matrices[index] = glm::scale(matrices[index], transform.Size);

				matrices[index] = glm::rotate(matrices[index], glm::radians(transform.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				matrices[index] = glm::rotate(matrices[index], glm::radians(transform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				matrices[index] = glm::rotate(matrices[index], glm::radians(transform.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			}

			s_ModelBuffer->SetDataIndexed(index, (void*)&matrices[index], s_UniformSize);

			index++;
		}
		s_ModelBuffer->UploadIndexedData();
	}

	void SceneRenderer::RenderModels(Scene* scene, Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer)
	{
		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();
		auto group = pipeline->GetDescriptorSets();
		auto set = group->GetSets(1)[0];
		set->Bind(pipeline, cmdBuffer);

		// Actually draw using the data and uploading texture
		uint32_t index = 0;
		for (auto& entity : s_Entities)
		{
			auto set = group->GetSets(0)[index];

			// Mesh
			MeshComponent& mesh = s_Entities.get<MeshComponent>(entity);
			if (mesh.MeshObject)
			{
				mesh.MeshObject->GetMesh()->GetVertexBuffer()->Bind(cmdBuffer);
				mesh.MeshObject->GetMesh()->GetIndexBuffer()->Bind(cmdBuffer);
			}

			// Material
			if (mesh.Material)
			{
				mesh.Material->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_Image"));
			}
			else
			{
				LV_LOG_WARN("Mesh of entity[{0}] has no material. This is not critical, but not recommended.", registry.get<UUID>(entity).Get());
				s_EmptyImage->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_Image"));
			}

			s_ModelBuffer->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_Model"), index * s_ModelBuffer->GetAlignment());
			set->Bind(pipeline, cmdBuffer, 0);
			if (mesh.MeshObject)
				Renderer::DrawIndexed(cmdBuffer, mesh.MeshObject->GetMesh()->GetIndexBuffer());

			index++;
		}
	}

}