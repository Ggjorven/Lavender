#include "lvpch.h"
#include "SceneRenderer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/Pipeline.hpp"

#include "Lavender/ECS/Components.hpp"

namespace Lavender
{

	SceneRenderer::SceneRenderer(Scene* scene)
		: m_Scene(scene)
	{
	}

	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		: SceneRenderer(scene.get())
	{
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::Render(const Camera& camera, Ref<RenderCommandBuffer> cmdBuffer)
	{
		auto& registry = m_Scene->GetCollection()->GetMainRegistry()->GetRegistry();

		auto view = registry.view<MeshComponent>();
		for (auto& entity : view)
		{
			MeshComponent& mesh = view.get<MeshComponent>(entity);

			mesh.MeshObject.GetVertexBuffer()->Bind(cmdBuffer);
			mesh.MeshObject.GetIndexBuffer()->Bind(cmdBuffer);

			if (mesh.Image)
			{
				auto pipeline = m_Scene->GetViewport()->GetPipeline();
				auto element = pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_Image");
				mesh.Image->Upload(pipeline, element);
				Renderer::Wait();
			}

			Renderer::DrawIndexed(cmdBuffer, mesh.MeshObject.GetIndexBuffer());
		}
	}

}