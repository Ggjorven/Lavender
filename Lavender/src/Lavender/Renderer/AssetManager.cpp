#include "lvpch.h"
#include "AssetManager.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/Mesh.hpp"

namespace Lavender
{

	AssetManager::AssetManager(Ref<RenderPass> renderpass)
		: m_RenderPass(renderpass)
	{
		// Pipelines
		UniformLayout uniformLayout = {
			{ UniformDataType::Image, 0, 0, "u_Image", UniformElement::ShaderStage::Fragment },
			{ UniformDataType::UniformBuffer, 0, 1, "u_Camera", UniformElement::ShaderStage::Vertex }
		};

		PipelineSpecification specs = {};
		specs.Bufferlayout = MeshVertex::GetLayout();
		specs.Uniformlayout = uniformLayout;

		specs.Polygonmode = PipelineSpecification::PolygonMode::Fill;
		specs.LineWidth = 1.0f;
		specs.Cullingmode = PipelineSpecification::CullingMode::Back;

		ShaderCode code = {};
		code.VertexSPIRV = Shader::ReadSPIRVFile("assets/shaders/vert.spv");	// TODO: Use a better shader name
		code.FragmentSPIRV = Shader::ReadSPIRVFile("assets/shaders/frag.spv");	// TODO: Use a better shader name
		Ref<Shader> shader = Shader::Create(code);

		m_Pipelines[PipelineType::MeshAndImage] = Pipeline::Create(specs, shader, m_RenderPass);
		m_Pipelines[PipelineType::MeshAndImage]->Initialize();
	}

	AssetManager::~AssetManager()
	{
	}

	Ref<AssetManager> AssetManager::Create(Ref<RenderPass> renderpass)
	{
		return RefHelper::Create<AssetManager>(renderpass);
	}

}