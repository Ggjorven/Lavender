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

	std::vector<Ref<UniformBuffer>> SceneRenderer::s_ModelBuffers = { };
	Ref<Image2D> SceneRenderer::s_EmptyImage = nullptr;

	void SceneRenderer::Init()
	{
		s_ModelBuffers.resize((size_t)Renderer::GetSpecification().PreAllocatedDescriptorSets);
		for (uint32_t i = 0; i < Renderer::GetSpecification().PreAllocatedDescriptorSets; i++)
		{
			s_ModelBuffers[i] = UniformBuffer::Create(sizeof(glm::mat4));
		}

		s_EmptyImage = Image2D::Create(1, 1);
	}

	void SceneRenderer::Destroy()
	{
		s_ModelBuffers.clear();
		s_EmptyImage.reset();
	}

	void SceneRenderer::RenderScene(Scene* scene, Ref<EditorCamera>& camera, Ref<RenderCommandBuffer> cmdBuffer)
	{
		LV_PROFILE_SCOPE("SceneRenderer::RenderScene");

		auto& registry = scene->GetCollection()->GetMainRegistry()->GetRegistry();

		// Note(Jorben): Set 0 is for images and the model matrix
		uint32_t index = 0;
		auto pipeline = FrameResources::GetPipeline();
		auto group = pipeline->GetDescriptorSets();
		auto sets = group->GetSets(0);

		auto view = registry.view<MeshComponent>();
		// Check if we need to resize
		if (view.size() > sets.size())
		{
			group->AddMoreSetsTo(0, (uint32_t)(view.size() - (sets.size())));
			sets = group->GetSets(0);
		
			s_ModelBuffers.resize(view.size());
			for (size_t i = sets.size() - 1; i < view.size(); i++)
			{
				s_ModelBuffers[i] = UniformBuffer::Create(sizeof(glm::mat4));
			}
		}

		for (auto& entity : view)
		{
			auto& set = sets[index];

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

			// Change Model matrix based on transformation
			glm::mat4 modelMatrix(1.0f);

			auto transformView = registry.view<TransformComponent>();
			if (transformView.contains(entity))
			{
				TransformComponent& transform = transformView.get<TransformComponent>(entity);

				modelMatrix = glm::translate(modelMatrix, transform.Position);
				modelMatrix = glm::scale(modelMatrix, transform.Size);

				modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			}
			s_ModelBuffers[index]->SetData((void*)&modelMatrix, sizeof(glm::mat4));
			s_ModelBuffers[index]->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_Model"));

			set->Bind(pipeline, cmdBuffer);
			if (mesh.MeshObject) 
				Renderer::DrawIndexed(cmdBuffer, mesh.MeshObject->GetMesh()->GetIndexBuffer());
			
			index++;
		}
	}

}