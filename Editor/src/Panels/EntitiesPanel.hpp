#pragma once

#include <Lavender/Utils/Utils.hpp>
#include <Lavender/Utils/UUID.hpp>

#include <Lavender/Workspace/Project.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>
#include <Lavender/UI/Colours.hpp>

#include <Lavender/Renderer/Image.hpp>

#include <Lavender/ECS/Components.hpp>

namespace Lavender
{

	class EntitiesPanel
	{
	public:
		enum class ComponentUsage // TODO: Add more
		{
			None = 0, Opened = BIT(0),
			Remove = BIT(1)
		};

	public:
		EntitiesPanel(Ref<Project> project);
		virtual ~EntitiesPanel();

		void RenderUI();

		static Ref<EntitiesPanel> Create(Ref<Project> project);

		template<typename TComponent>
		ComponentUsage BeginECSComponent(Ref<Image2D> icon = nullptr);

	private:
		Ref<Project> m_Project = nullptr;

		UUID m_SelectedUUID = {};
	};

	DEFINE_BITWISE_OPS(EntitiesPanel::ComponentUsage)

	template<typename TComponent>
	EntitiesPanel::ComponentUsage EntitiesPanel::BeginECSComponent(Ref<Image2D> icon) // TODO: Add options for removing and stuff + add icons.
	{
		ComponentUsage usage = ComponentUsage::None;

		UI::ScopedStyleList styles = {{
			{ UI::StyleType::FrameRounding, 1.0f },
			{ UI::StyleType::FramePadding, { 15.0f, 6.0f } },
			{ UI::StyleType::FrameBorderSize, 1.0f }
		}};

		UI::ScopedStyleList colours = {{
			{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			{ UI::StyleColourType::Border, UI::Colours::BackgroundDark },
		}};

		UI::TreeNodeFlags treenodeFlags = UI::TreeNodeFlags::Framed | UI::TreeNodeFlags::SpanAvailWidth | UI::TreeNodeFlags::FramePadding | UI::TreeNodeFlags::DefaultOpen;
		if (UI::TreeNode(ComponentToString<TComponent>().c_str(), treenodeFlags))
		{
			usage = usage | ComponentUsage::Opened;
			UI::TreeNodePop();
		}
		UI::ShiftCursorY(-1.0f);

		return usage;
	}

}