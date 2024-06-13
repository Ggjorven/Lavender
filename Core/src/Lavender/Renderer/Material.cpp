#include "lvpch.h"
#include "Material.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/WorkSpace/EngineTracker.hpp"

namespace Lavender
{

	static Ref<Image2D> s_WhiteTexture = nullptr;

	Material::Material(const MaterialSpecification& specs)
		: m_Specification(specs)
	{
		if (!specs.Albedo.empty()) m_Albedo = Image2D::Create(ImageSpecification(specs.Albedo));
		else m_Albedo = s_WhiteTexture;

		// TODO: Add more
	}

	Material::~Material()
	{
	}

	Ref<Material> Material::Create(const MaterialSpecification& specs)
	{
		return RefHelper::Create<Material>(specs);
	}

	void Material::Init()
	{
		ImageSpecification white = ImageSpecification(1, 1, ImageUsageFlags::Colour | ImageUsageFlags::NoMipMaps | ImageUsageFlags::Sampled);
		s_WhiteTexture = Image2D::Create(white);
		uint32_t col = 0xFFFFFFFF;
		s_WhiteTexture->SetData((void*)&col, sizeof(uint32_t));
	}

	void Material::Destroy()
	{
		s_WhiteTexture.reset();
		s_WhiteTexture = nullptr;
	}

}