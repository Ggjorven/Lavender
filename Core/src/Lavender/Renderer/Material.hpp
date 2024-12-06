#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Image.hpp"

namespace Lavender
{

	struct MaterialSpecification
	{
	public:
		std::filesystem::path Albedo = {};
		// TODO: Add more
	};

	class Material
	{
	public:
		Material(const MaterialSpecification& specs);
		virtual ~Material();

		inline Ref<Image2D> GetAlbedo() { return m_Albedo; }

		static Ref<Material> Create(const MaterialSpecification& specs = MaterialSpecification());

	private:
		MaterialSpecification m_Specification = {};

		Ref<Image2D> m_Albedo = nullptr;
		// TODO: Add more
	};

}