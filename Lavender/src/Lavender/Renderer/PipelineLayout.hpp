#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <initializer_list>
#include <unordered_set>

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	enum class DataType : uint8_t
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};
	uint32_t DataTypeSize(DataType type);

	struct BufferElement
	{
	public:
		std::string Name;
		uint32_t Location;
		DataType Type;
		uint32_t Size;
		size_t Offset;

		BufferElement() = default;
		BufferElement(DataType type, uint32_t location, const std::string& name);
		virtual ~BufferElement() = default;

		uint32_t GetComponentCount() const;
	};

	struct BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements);
		virtual ~BufferLayout() = default;

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride();

	private:
		std::vector<BufferElement> m_Elements = { };
		uint32_t m_Stride = 0;
	};



	typedef uint32_t SetID;

	enum class UniformDataType : uint8_t
	{
		None = 0, UniformBuffer, Image
	};

	// Note(Jorben): To use multiple descriptor set you need have Vulkan selected as the API
	struct UniformElement
	{
	public:
		enum class ShaderStage : uint8_t
		{
			None = 0, Vertex = BIT(0), Fragment = BIT(1)
		};
		// Bitwise operations are created below

	public:
		std::string Name = "Empty";
		SetID Set = 0;
		uint32_t Binding = 0;
		UniformDataType Type = UniformDataType::None;
		uint32_t Count = 1;
		ShaderStage Stage = ShaderStage::None;

		UniformElement() = default;
		UniformElement(UniformDataType type, SetID set, uint32_t binding, const std::string& name, ShaderStage stage, uint32_t count = 1);
		virtual ~UniformElement() = default;
	};

	class UniformLayout
	{
	public:
		UniformLayout() = default;
		UniformLayout(const std::initializer_list<UniformElement>& elements);
		virtual ~UniformLayout() = default;

		inline const std::unordered_map<uint32_t, std::vector<UniformElement>>& GetElements() const { return m_Elements; }

		UniformElement GetElementByName(SetID set, const std::string& name);

		uint32_t UniqueCount(SetID set) const;
		std::unordered_set<UniformDataType> UniqueTypes(SetID set) const;
		uint32_t AmountOf(SetID set, UniformDataType type) const;

	private:
		// First element is the Set, second are the elements in that Set
		std::unordered_map<SetID, std::vector<UniformElement>> m_Elements = { }; 
	};
	DEFINE_BITWISE_OPS(UniformElement::ShaderStage)



	// TODO: Add more customizability
	struct PipelineSpecification
	{
	public:
		enum class CullingMode : uint8_t
		{
			None = LV_MAX_UINT8, Front = 1, Back, FrontAndBack
		};

		enum class PolygonMode : uint8_t
		{
			None = LV_MAX_UINT8, Fill = 0, Line = 1
		};

	public:
		BufferLayout Bufferlayout = {};
		UniformLayout Uniformlayout = {};

		PolygonMode Polygonmode = PolygonMode::Fill;
		float LineWidth = 1.0f;
		CullingMode Cullingmode = CullingMode::Front;

		bool Blending = false;

	public:
		PipelineSpecification() = default;
		PipelineSpecification(BufferLayout bufferLayout, UniformLayout uniformLayout);
		virtual ~PipelineSpecification() = default;
	};

	/*
	Shader Stages:

Vertex Shader
Tessellation Control Shader
Tessellation Evaluation Shader
Geometry Shader
Fragment Shader
Compute Shader
Vertex Input:

Vertex Input Bindings (VkVertexInputBindingDescription)
Vertex Input Attributes (VkVertexInputAttributeDescription)
Input Assembly:

Primitive Topology (VkPrimitiveTopology)
Primitive Restart
Viewport and Scissor:

Viewport (VkViewport)
Scissor (VkRect2D)
Rasterization:

Polygon Mode (VkPolygonMode)
Culling Mode (VkCullModeFlags)
Front Face Winding Order (VkFrontFace)
Line Width
Multisampling:

Sample Count (VkSampleCountFlagBits)
Depth and Stencil:

Depth Test Enable/Disable
Depth Write Enable/Disable
Depth Compare Operation (VkCompareOp)
Depth Bounds Test Enable/Disable
Stencil Test Enable/Disable
Stencil Operations (VkStencilOpState)
Color Blend:

Blend Enable/Disable for each attachment
Blend Factors (VkBlendFactor)
Blend Operations (VkBlendOp)
Color Mask for each attachment
Dynamic State:

Dynamic Viewport
Dynamic Scissor
Dynamic Line Width
Dynamic Depth Bias
Dynamic Blend Constants
Dynamic Depth Bounds
Dynamic Stencil Compare Mask
Dynamic Stencil Write Mask
Dynamic Stencil Reference
Pipeline Layout:

Descriptor Set Layouts
Push Constants Ranges
Subpass:

Render Pass
Subpass Index
Tessellation:

Tessellation Patch Control Points
Tessellation Factors Buffer
Vertex Post-processing:

Primitive Restart Enable/Disable
Transform Feedback Enable/Disable
Stream Output Targets
Pipeline Derivatives:

Base Pipeline Handle
Base Pipeline Index
Pipeline Cache:

Pipeline Cache Object
Pipeline Creation Flags:

Allow Derivatives
Derivative
Mutable Aliases
	*/
}