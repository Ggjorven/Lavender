#include "lvpch.h"
#include "FrameResources.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/Mesh.hpp"
#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/DescriptorSet.hpp"

#include "Lavender/Workspace/Project.hpp"

namespace Lavender
{

	static Project* s_ProjectReference = nullptr;

	Ref<Pipeline> FrameResources::s_Pipeline = nullptr;

	void FrameResources::Init(Project* project)
	{
		s_ProjectReference = project;

		CreatePipeline();
	}

	void FrameResources::Destroy()
	{
		s_Pipeline.reset();

		s_ProjectReference = nullptr;
	}

	void FrameResources::CreatePipeline()
	{
		ShaderCode code = {};
		code.VertexGLSL = Shader::ReadGLSLFile(Utils::ToolKit::GetEnv() + "/Editor/assets/shaders/shader.vert");
		code.FragmentGLSL = Shader::ReadGLSLFile(Utils::ToolKit::GetEnv() + "/Editor/assets/shaders/shader.frag");

		Ref<Shader> shader = Shader::Create(code);

		UniformLayout uniformLayout = {
			{ UniformDataType::Image, 0, 0, "u_AlbedoImage", UniformElement::ShaderStage::Fragment },
			{ UniformDataType::Image, 0, 1, "u_SpecularImage", UniformElement::ShaderStage::Fragment },
			{ UniformDataType::DynamicUniformBuffer, 0, 2, "u_Model", UniformElement::ShaderStage::Vertex },
			{ UniformDataType::DynamicUniformBuffer, 0, 3, "u_Material", UniformElement::ShaderStage::Fragment },

			{ UniformDataType::UniformBuffer, 1, 0, "u_Camera", UniformElement::ShaderStage::Vertex },
			{ UniformDataType::UniformBuffer, 1, 1, "u_LightSettings", UniformElement::ShaderStage::Fragment },
			{ UniformDataType::UniformBuffer, 1, 2, "u_SceneData", UniformElement::ShaderStage::Fragment }
		};

		PipelineSpecification pipelineSpecs = {};
		pipelineSpecs.Bufferlayout = MeshVertex::GetLayout();
		pipelineSpecs.Uniformlayout = uniformLayout;

		pipelineSpecs.Polygonmode = PipelineSpecification::PolygonMode::Fill;
		pipelineSpecs.LineWidth = 1.0f;
		pipelineSpecs.Cullingmode = PipelineSpecification::CullingMode::None;

		DescriptorSetGroup::DescriptorCount size = {};
		size.SetCount(0, Renderer::GetSpecification().PreAllocatedDescriptorSets);
		size.SetCount(1, 1);

		auto descriptorSets = DescriptorSetGroup::Create(uniformLayout, size);

		s_Pipeline = Pipeline::Create(pipelineSpecs, descriptorSets, shader, s_ProjectReference->GetViewport()->GetRenderPass());
		s_Pipeline->Initialize();
	}

}