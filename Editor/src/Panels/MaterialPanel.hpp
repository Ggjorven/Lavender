#pragma once

#include <Lavender/Utils/Utils.hpp>
		 
#include <Lavender/Workspace/Project.hpp>
#include <Lavender/Workspace/Assets/Asset.hpp>
		 
#include <Lavender/Renderer/Image.hpp>

namespace Lavender
{

	class MaterialPanel
	{
	public:
		MaterialPanel(Ref<Project> project);
		virtual ~MaterialPanel();

		void RenderUI();

		inline void SetEnabled(bool state) { m_Enabled = state; }
		inline bool& GetEnabled() { return m_Enabled; }

		static Ref<MaterialPanel> Create(Ref<Project> project);

	private:
		bool CustomTree(const std::string& label);

	private:
		Ref<Project> m_Project = nullptr;
		
		Ref<Image2D> m_CheckerBoard = nullptr;

		AssetHandle m_SelectedAsset = AssetHandle::Empty;

		bool m_Enabled = false;
		uint32_t m_Width = 0, m_Height = 0;
	};

}