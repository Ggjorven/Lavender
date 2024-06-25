#include "lvpch.h"
#include "SceneRenderer.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/WorkSpace/Project.hpp"
#include "Lavender/WorkSpace/EngineTracker.hpp"
#include "Lavender/WorkSpace/SharedResources.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Lavender
{

    SceneRenderer::SceneRenderer(Scene* scene)
        : m_Scene(scene)
    {
    }

    SceneRenderer::~SceneRenderer()
    {
    }

    void SceneRenderer::Render(Ref<Camera> view)
    {
		APP_PROFILE_SCOPE("SceneRenderer");

		{
			APP_PROFILE_SCOPE("SceneRenderer::Update");
			UpdateCamera(view);
			UpdateModels();
			UpdateLights();
			UpdateSceneData();
		}
		{
			APP_PROFILE_SCOPE("SceneRenderer::Render");
			DepthPrePass();
			LightCulling();
			FinalShading();
		}
    }

	void SceneRenderer::OnEvent(Event& e)
	{
		EventHandler handler(e);
		handler.Handle<WindowResizeEvent>(APP_BIND_EVENT_FN(SceneRenderer::OnResize));
	}

	void SceneRenderer::Resize(uint32_t width, uint32_t height)
	{
		m_Resources.Resize(width, height);
	}

    Ref<SceneRenderer> SceneRenderer::Create(Scene* scene)
    {
        return RefHelper::Create<SceneRenderer>(scene);
    }

	void SceneRenderer::UpdateCamera(Ref<Camera> view)
	{
		APP_PROFILE_SCOPE("SceneRenderer::UpdateCamera");
		// Note(Jorben): Note that the camera's OnUpdate has to be called in the scene loop

		ShaderResource::Camera cam = view->AsResource();
		m_Resources.CameraBuffer->SetData((void*)&cam, sizeof(ShaderResource::Camera));

		m_Resources.CameraBuffer->Upload(m_Resources.Depth.DescriptorSets->GetSets(1)[0], m_Resources.Depth.DescriptorSets->GetLayout(1).GetDescriptorByName("u_Camera"));
		m_Resources.CameraBuffer->Upload(m_Resources.LightCulling.DescriptorSets->GetSets(1)[0], m_Resources.LightCulling.DescriptorSets->GetLayout(1).GetDescriptorByName("u_Camera"));
		m_Resources.CameraBuffer->Upload(m_Resources.Shading.DescriptorSets->GetSets(1)[0], m_Resources.Shading.DescriptorSets->GetLayout(1).GetDescriptorByName("u_Camera"));
	}

	void SceneRenderer::UpdateModels()
	{
		APP_PROFILE_SCOPE("SceneRenderer::UpdateModels");

		auto view = m_Scene->GetRegistry(Project::Get()->GetState()).GetRegistry().view<MeshComponent>();
		
		if (view.size() > (size_t)m_Resources.AllocatedModels)
		{
			m_Resources.ModelBuffer = DynamicUniformBuffer::Create((uint32_t)view.size(), sizeof(ShaderResource::Model));
			m_Resources.AllocatedModels = (uint32_t)view.size();
		}

		std::vector<ShaderResource::Model> matrices(view.size());

		size_t i = 0;
		for (auto& entity : view)
		{
			auto transforms = m_Scene->GetRegistry(Project::Get()->GetState()).GetRegistry().view<TransformComponent>();
			APP_ASSERT(transforms.contains(entity), "Entity with MeshComponent doesn't have TransformComponent.");

			matrices[i] = { CalculateModelMatrix(transforms.get<TransformComponent>(entity)) };
			m_Resources.ModelBuffer->SetDataIndexed((uint32_t)i, &matrices[i], sizeof(ShaderResource::Model));

			i++;
		}
		m_Resources.ModelBuffer->UploadIndexedData();
	}

	void SceneRenderer::UpdateLights()
	{
		APP_PROFILE_SCOPE("SceneRenderer::UpdateLights");

		auto view = m_Scene->GetRegistry(Project::Get()->GetState()).GetRegistry().view<PointLightComponent>();
		std::vector<ShaderResource::PointLight> lights(view.size());

		size_t i = 0;
		for (auto& entity : view)
		{
			PointLightComponent pointLight = view.get<PointLightComponent>(entity);

			auto transforms = m_Scene->GetRegistry(Project::Get()->GetState()).GetRegistry().view<TransformComponent>();
			APP_ASSERT(transforms.contains(entity), "Entity with PointLightComponent doesn't have TransformComponent.");

			ShaderResource::PointLight light = {};
			light.Position = transforms.get<TransformComponent>(entity).Position;
			light.Colour = pointLight.Colour;
			light.Radius = pointLight.Radius;
			light.Intensity = pointLight.Intensity;

			lights[i] = light;

			i++;
		}

		uint32_t size = (uint32_t)lights.size();
		m_Resources.LightCulling.LightsBuffer->SetData((void*)&size, sizeof(uint32_t));
		m_Resources.LightCulling.LightsBuffer->SetData((void*)lights.data(), sizeof(ShaderResource::PointLight) * lights.size(), sizeof(uint32_t) + (sizeof(char) * 12));
		m_Resources.LightCulling.LightsBuffer->Upload(m_Resources.LightCulling.DescriptorSets->GetSets(0)[0], m_Resources.LightCulling.DescriptorSets->GetLayout(0).GetDescriptorByName("u_Lights"));

		m_Resources.LightCulling.LightVisibilityBuffer->Upload(m_Resources.LightCulling.DescriptorSets->GetSets(0)[0], m_Resources.LightCulling.DescriptorSets->GetLayout(0).GetDescriptorByName("u_Visibility"));
	}

	void SceneRenderer::UpdateSceneData()
	{
		APP_PROFILE_SCOPE("SceneRenderer::UpdateSceneData");

		ShaderResource::Scene uniform = { { Track::Viewport::Width, Track::Viewport::Height } };
		m_Resources.SceneBuffer->SetData((void*)&uniform, sizeof(ShaderResource::Scene));
		m_Resources.SceneBuffer->Upload(m_Resources.LightCulling.DescriptorSets->GetSets(1)[0], m_Resources.LightCulling.DescriptorSets->GetLayout(1).GetDescriptorByName("u_Scene"));
		m_Resources.SceneBuffer->Upload(m_Resources.Shading.DescriptorSets->GetSets(1)[0], m_Resources.Shading.DescriptorSets->GetLayout(1).GetDescriptorByName("u_Scene"));
	}

	void SceneRenderer::DepthPrePass()
    {
		// Depth pre pass
		Renderer::Submit([this]()
		{
			APP_PROFILE_SCOPE("SceneRenderer::DepthPrePass");

			auto modelSets = m_Resources.Depth.DescriptorSets->GetSets(0);
			auto cameraSet = m_Resources.Depth.DescriptorSets->GetSets(1)[0];

			m_Resources.Depth.RenderPass->Begin();

			m_Resources.Depth.Pipeline->Use(m_Resources.Depth.RenderPass->GetCommandBuffer());
			cameraSet->Bind(m_Resources.Depth.Pipeline, m_Resources.Depth.RenderPass->GetCommandBuffer());

			auto view = m_Scene->GetRegistry(Project::Get()->GetState()).GetRegistry().view<MeshComponent>();
			std::vector<ShaderResource::Model> matrices(view.size());

			size_t i = 0;
			for (auto& entity : view)
			{
				MeshComponent mesh = view.get<MeshComponent>(entity);

				m_Resources.ModelBuffer->Upload(modelSets[i], m_Resources.Depth.DescriptorSets->GetLayout(0).GetDescriptorByName("u_Model"), sizeof(ShaderResource::Model) * i);
				modelSets[i]->Bind(m_Resources.Depth.Pipeline, m_Resources.Depth.RenderPass->GetCommandBuffer(), PipelineBindPoint::Graphics, { 0 });

				if (mesh.Mesh)
				{
					mesh.Mesh->GetMesh()->GetVertexBuffer()->Bind(m_Resources.Depth.RenderPass->GetCommandBuffer());
					mesh.Mesh->GetMesh()->GetIndexBuffer()->Bind(m_Resources.Depth.RenderPass->GetCommandBuffer());

					Renderer::DrawIndexed(m_Resources.Depth.RenderPass->GetCommandBuffer(), mesh.Mesh->GetMesh()->GetIndexBuffer());
				}

				i++;
			}

			m_Resources.Depth.RenderPass->End();
			m_Resources.Depth.RenderPass->Submit();
		});
    }

    void SceneRenderer::LightCulling()
    {
		// Light culling
		Renderer::Submit([this]()
		{
			APP_PROFILE_SCOPE("SceneRenderer::LightCulling");
			
			const glm::uvec2 tiles = GetTileCount();
			auto& set0 = m_Resources.LightCulling.DescriptorSets->GetSets(0)[0];
			auto& set1 = m_Resources.LightCulling.DescriptorSets->GetSets(1)[0];

			m_Resources.LightCulling.CommandBuffer->Begin();

			{
				APP_PROFILE_SCOPE("SceneRenderer::LightCulling::Transition");
				Renderer::GetDepthImage()->Transition(ImageLayout::Depth, ImageLayout::DepthRead);
				Renderer::GetDepthImage()->Upload(set0, m_Resources.LightCulling.DescriptorSets->GetLayout(0).GetDescriptorByName("u_DepthBuffer"));
			}

			m_Resources.LightCulling.Pipeline->Use(m_Resources.LightCulling.CommandBuffer, PipelineBindPoint::Compute);

			set0->Bind(m_Resources.LightCulling.Pipeline, m_Resources.LightCulling.CommandBuffer, PipelineBindPoint::Compute);
			set1->Bind(m_Resources.LightCulling.Pipeline, m_Resources.LightCulling.CommandBuffer, PipelineBindPoint::Compute);

			{
				APP_PROFILE_SCOPE("SceneRenderer::LightCulling::Dispatch");
				m_Resources.LightCulling.ComputeShader->Dispatch(m_Resources.LightCulling.CommandBuffer, tiles.x, tiles.y, 1);
			}

			m_Resources.LightCulling.CommandBuffer->End();
			m_Resources.LightCulling.CommandBuffer->Submit(Queue::Compute);
		});
    }

    void SceneRenderer::FinalShading()
    {
		// Final shading
		Renderer::Submit([this]()
		{
			APP_PROFILE_SCOPE("SceneRenderer::FinalShading");

			auto& set0 = m_Resources.Shading.DescriptorSets->GetSets(0);
			auto& set1 = m_Resources.Shading.DescriptorSets->GetSets(1)[0];

			m_Resources.Shading.RenderPass->Begin();

			m_Resources.Shading.Pipeline->Use(m_Resources.Shading.RenderPass->GetCommandBuffer());
			set1->Bind(m_Resources.Shading.Pipeline, m_Resources.Shading.RenderPass->GetCommandBuffer());

			auto view = m_Scene->GetRegistry(Project::Get()->GetState()).GetRegistry().view<MeshComponent>();

			size_t i = 0;
			for (auto& entity : view)
			{
				MeshComponent mesh = view.get<MeshComponent>(entity);

				m_Resources.ModelBuffer->Upload(set0[i], m_Resources.Shading.DescriptorSets->GetLayout(0).GetDescriptorByName("u_Model"), sizeof(ShaderResource::Model) * i);

				if (mesh.Material)
					mesh.Material->GetMaterial()->GetAlbedo()->Upload(set0[i], m_Resources.Shading.DescriptorSets->GetLayout(0).GetDescriptorByName("u_Albedo"));
				else
					Shared::WhiteTexture->Upload(set0[i], m_Resources.Shading.DescriptorSets->GetLayout(0).GetDescriptorByName("u_Albedo"));

				m_Resources.LightCulling.LightsBuffer->Upload(set0[i], m_Resources.Shading.DescriptorSets->GetLayout(0).GetDescriptorByName("u_Lights"));
				m_Resources.LightCulling.LightVisibilityBuffer->Upload(set0[i], m_Resources.Shading.DescriptorSets->GetLayout(0).GetDescriptorByName("u_Visibility"));

				set0[i]->Bind(m_Resources.Shading.Pipeline, m_Resources.Shading.RenderPass->GetCommandBuffer(), PipelineBindPoint::Graphics, { 0 });

				if (mesh.Mesh)
				{
					mesh.Mesh->GetMesh()->GetVertexBuffer()->Bind(m_Resources.Shading.RenderPass->GetCommandBuffer());
					mesh.Mesh->GetMesh()->GetIndexBuffer()->Bind(m_Resources.Shading.RenderPass->GetCommandBuffer());

					Renderer::DrawIndexed(m_Resources.Shading.RenderPass->GetCommandBuffer(), mesh.Mesh->GetMesh()->GetIndexBuffer());
				}

				i++;
			}

			m_Resources.Shading.RenderPass->End();
			m_Resources.Shading.RenderPass->Submit();
		});
    }

	glm::uvec2 SceneRenderer::GetTileCount()
	{
		glm::uvec2 tiles = {};
		tiles.x = (Track::Viewport::Width + TILE_SIZE - 1) / TILE_SIZE;
		tiles.y = (Track::Viewport::Height + TILE_SIZE - 1) / TILE_SIZE;
		return tiles;
	}

	glm::mat4 SceneRenderer::CalculateModelMatrix(const TransformComponent& component)
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, component.Position);
		matrix = glm::scale(matrix, component.Size);

		// Note(Jorben): There is probably a better way to do rotation
		matrix = glm::rotate(matrix, glm::radians(component.Rotation.x), { 1.0f, 0.0f, 0.0f });
		matrix = glm::rotate(matrix, glm::radians(component.Rotation.y), { 0.0f, 1.0f, 0.0f });
		matrix = glm::rotate(matrix, glm::radians(component.Rotation.z), { 0.0f, 0.0f, 1.0f });

		return matrix;
	}

	bool SceneRenderer::OnResize(WindowResizeEvent& e)
	{
		Resize(e.GetWidth(), e.GetHeight());
		return false;
	}

}