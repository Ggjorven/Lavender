#include "lvpch.h"
#include "Material.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/WorkSpace/EngineTracker.hpp"
#include "Lavender/WorkSpace/SharedResources.hpp"

namespace Lavender
{

	Material::Material(const MaterialSpecification& specs)
		: m_Specification(specs)
	{
		if (!specs.Albedo.empty()) m_Albedo = Image2D::Create(ImageSpecification(specs.Albedo));
		else m_Albedo = Shared::WhiteTexture;
	}

	Material::~Material()
	{
	}

	Ref<Material> Material::Create(const MaterialSpecification& specs)
	{
		return RefHelper::Create<Material>(specs);
	}

}