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

	static uint32_t s_EntitiesAllocated = 100u; // Is used for images and uniforms
	static size_t s_UniformSize = sizeof(glm::mat4); // glm::mat4 'cause of Model matrix

	Ref<DynamicUniformBuffer> SceneRenderer::s_ModelBuffer = nullptr;
	Ref<Image2D> SceneRenderer::s_EmptyImage = nullptr;

	void SceneRenderer::Init()
	{
		s_ModelBuffer = DynamicUniformBuffer::Create(s_EntitiesAllocated, s_UniformSize);
		s_EmptyImage = Image2D::Create(1, 1);
	}

	void SceneRenderer::Destroy()
	{
		s_ModelBuffer.reset();
		s_EmptyImage.reset();
	}

	void SceneRenderer::RenderScene(Scene* scene, Ref<EditorCamera>& camera, Ref<RenderCommandBuffer> cmdBuffer)
	{
		LV_PROFILE_SCOPE("SceneRenderer::RenderScene");

		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();

		// Note(Jorben): Set 0 is for images and the model matrix
		auto pipeline = FrameResources::GetPipeline();
		auto group = pipeline->GetDescriptorSets();

		auto view = registry.view<MeshComponent>();
		// Check if we need to resize
		if (view.size() > s_EntitiesAllocated)
		{
			s_ModelBuffer.reset();
			s_ModelBuffer = DynamicUniformBuffer::Create(s_EntitiesAllocated + (s_EntitiesAllocated - (uint32_t)view.size()), s_UniformSize);
		}

		// Upload model matrices to dynamic buffer
		uint32_t index = 0;
		std::vector<glm::mat4> matrices((size_t)s_EntitiesAllocated, glm::mat4(1.0f));
		for (auto& entity : view)
		{
			MeshComponent& mesh = view.get<MeshComponent>(entity);

			auto transformView = registry.view<TransformComponent>();
			if (transformView.contains(entity))
			{
				TransformComponent& transform = transformView.get<TransformComponent>(entity);

				matrices[index] = glm::translate(matrices[index], transform.Position);
				matrices[index] = glm::scale(matrices[index], transform.Size);

				matrices[index] = glm::rotate(matrices[index], glm::radians(transform.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				matrices[index] = glm::rotate(matrices[index], glm::radians(transform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				matrices[index] = glm::rotate(matrices[index], glm::radians(transform.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			}

			// TODO: Change 
			s_ModelBuffer->SetDataIndexed(index, (void*)&matrices[index], s_UniformSize);

			index++;
		}
		s_ModelBuffer->UploadIndexedData();
		s_ModelBuffer->Upload(group->GetSets(1)[0], pipeline->GetSpecification().Uniformlayout.GetElementByName(1, "u_Model"));

		// Actually draw using the data and uploading texture
		index = 0;
		for (auto& entity : view)
		{
			auto set = group->GetSets(0)[index];

			// Mesh
			MeshComponent& mesh = view.get<MeshComponent>(entity);
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

			set->Bind(pipeline, cmdBuffer);
			group->GetSets(1)[0]->Bind(pipeline, cmdBuffer, (size_t)index * s_ModelBuffer->GetAlignment());
			if (mesh.MeshObject) 
				Renderer::DrawIndexed(cmdBuffer, mesh.MeshObject->GetMesh()->GetIndexBuffer());
			
			index++;
		}
	}

}