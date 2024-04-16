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
	static uint32_t s_PointLightsAllocated = 0; 
	static size_t s_UniformSize = sizeof(glm::mat4); // glm::mat4 'cause of Model matrix

	Ref<Image2D> SceneRenderer::s_EmptyImage = nullptr;

	Ref<DynamicUniformBuffer> SceneRenderer::s_ModelBuffer = nullptr;
	Ref<DynamicUniformBuffer> SceneRenderer::s_MaterialBuffer = nullptr;

	Ref<UniformBuffer> SceneRenderer::s_LightsBuffer = nullptr;
	Ref<UniformBuffer> SceneRenderer::s_SceneDataBuffer;

	static SceneRenderer::DrawView s_Entities = {};

	void SceneRenderer::Init()
	{
		s_ModelBuffer = DynamicUniformBuffer::Create(Renderer::GetSpecification().PreAllocatedDescriptorSets, s_UniformSize);
		s_MaterialBuffer = DynamicUniformBuffer::Create(Renderer::GetSpecification().PreAllocatedDescriptorSets, sizeof(ShaderMaterial));
		s_EntitiesAllocated = Renderer::GetSpecification().PreAllocatedDescriptorSets;
		
		s_LightsBuffer = UniformBuffer::Create(LightSettings::GetSize(Renderer::GetSpecification().PreAllocatedLightSlots));
		s_PointLightsAllocated = Renderer::GetSpecification().PreAllocatedLightSlots;

		s_SceneDataBuffer = UniformBuffer::Create(sizeof(SceneData));

		ImageSpecification specs = {};
		specs.Usage = ImageSpecification::ImageUsage::Size;
		specs.Flags = ImageSpecification::ImageUsageFlags::Sampled;
		specs.Width = 1;
		specs.Height = 1;
		s_EmptyImage = Image2D::Create(specs);

		uint32_t data = 0xFFFFFFFF;
		s_EmptyImage->SetData((void*)&data, sizeof(uint32_t));
	}

	void SceneRenderer::Destroy()
	{
		s_EmptyImage.reset();

		s_ModelBuffer.reset();
		s_MaterialBuffer.reset();

		s_LightsBuffer.reset();
		s_SceneDataBuffer.reset();
	}

	void SceneRenderer::RenderScene(Scene* scene, const glm::vec3& cameraPosition, Ref<RenderCommandBuffer> cmdBuffer)
	{
		LV_PROFILE_SCOPE("SceneRenderer::RenderScene");

		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();
		auto pipeline = FrameResources::GetPipeline();

		SceneRenderer::UpdateLights(scene, pipeline);
		SceneRenderer::UpdateSceneData(scene, cameraPosition, pipeline);
		SceneRenderer::UpdateBuffers(scene, pipeline);

		SceneRenderer::RenderModels(scene, pipeline, cmdBuffer);
	}

	void SceneRenderer::UpdateLights(Scene* scene, Ref<Pipeline> pipeline)
	{
		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();
		auto pointLightView = registry.view<PointLightComponent>();

		// Check if we need to resize light buffer
		if (pointLightView.size() > s_PointLightsAllocated)
		{
			s_LightsBuffer.reset();
			s_LightsBuffer = UniformBuffer::Create(LightSettings::GetSize((uint32_t)pointLightView.size()));

			s_PointLightsAllocated = (uint32_t)pointLightView.size();
		}

		// Upload lights
		uint32_t index = 0;
		LightSettings lights((uint32_t)pointLightView.size(), (size_t)s_PointLightsAllocated);
		for (auto& entity : pointLightView)
		{
			PointLightComponent& light = pointLightView.get<PointLightComponent>(entity);
			lights.PointLights[index] = light;
			index++;
		}
		lights.AddDataTo(s_LightsBuffer);
		s_LightsBuffer->Upload(pipeline->GetDescriptorSets()->GetSets(1)[0], pipeline->GetSpecification().Uniformlayout.GetElementByName(1, "u_LightSettings"));
	}

	void SceneRenderer::UpdateSceneData(Scene* scene, const glm::vec3& cameraPosition, Ref<Pipeline> pipeline)
	{
		SceneData data = {};
		data.ViewPosition = cameraPosition;

		s_SceneDataBuffer->SetData((void*)&data, sizeof(SceneData));
		s_SceneDataBuffer->Upload(pipeline->GetDescriptorSets()->GetSets(1)[0], pipeline->GetSpecification().Uniformlayout.GetElementByName(1, "u_SceneData"));
	}

	void SceneRenderer::UpdateBuffers(Scene* scene, Ref<Pipeline> pipeline)
	{
		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();
		s_Entities = registry.view<MeshComponent>();

		// Check if we need to resize set 0 elements
		if (s_Entities.size() > s_EntitiesAllocated)
		{
			// Model buffer
			s_ModelBuffer.reset();
			s_ModelBuffer = DynamicUniformBuffer::Create((uint32_t)s_Entities.size(), s_UniformSize);
			pipeline->GetDescriptorSets()->AddMoreSetsTo(0, (uint32_t)s_Entities.size() - s_EntitiesAllocated);

			// Material buffer
			s_MaterialBuffer.reset();
			s_MaterialBuffer = DynamicUniformBuffer::Create((uint32_t)s_Entities.size(), s_UniformSize);
			pipeline->GetDescriptorSets()->AddMoreSetsTo(0, (uint32_t)s_Entities.size() - s_EntitiesAllocated);

			s_EntitiesAllocated = (uint32_t)s_Entities.size();
		}

		// Only loop through entities with an actual mesh
		uint32_t index = 0;
		std::vector<glm::mat4> matrices((size_t)s_EntitiesAllocated, glm::mat4(1.0f));
		std::vector<ShaderMaterial> materials((size_t)s_EntitiesAllocated);
		for (auto& entity : s_Entities)
		{
			// Model buffer
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

			// Material buffer
			if (s_Entities.contains(entity))
			{
				MeshComponent& mesh = s_Entities.get<MeshComponent>(entity);

				if (mesh.Material)
					materials[index] = mesh.Material->AsShaderMaterial();
			}
			s_MaterialBuffer->SetDataIndexed(index, (void*)&materials[index], sizeof(ShaderMaterial));

			index++;
		}
		s_ModelBuffer->UploadIndexedData();
		s_MaterialBuffer->UploadIndexedData();
	}

	void SceneRenderer::RenderModels(Scene* scene, Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer)
	{
		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();
		auto group = pipeline->GetDescriptorSets();

		// Bind set 1 (used for camera)
		group->GetSets(1)[0]->Bind(pipeline, cmdBuffer);

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
				mesh.Material->Upload(pipeline, set, s_EmptyImage);
			}
			else
			{
				LV_LOG_WARN("Mesh of entity[{0}] has no material. This is not critical, but not recommended.", registry.get<UUID>(entity).Get());
			}

			s_ModelBuffer->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_Model"), index * s_ModelBuffer->GetAlignment());

			size_t a = s_MaterialBuffer->GetAlignment();
			s_MaterialBuffer->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_Material"), index * s_MaterialBuffer->GetAlignment());

			set->Bind(pipeline, cmdBuffer, { 0, 0 });
			if (mesh.MeshObject)
				Renderer::DrawIndexed(cmdBuffer, mesh.MeshObject->GetMesh()->GetIndexBuffer());

			index++;
		}
	}

}