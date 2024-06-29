#include "lvpch.h"
#include "UI.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/UI/Style.hpp"
#include "Lavender/UI/Draw.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace Lavender::UI
{

	void PushID()
	{
		UI::PushID(Utils::Random::Int());
	}

	void PushID(int32_t id) // Leaks memory somehow
	{
		ImGuiWindow& window = *ImGui::GetCurrentWindow();
		//if (window.IDStack.Size == 0) id = 0;

		//window.IDStack.push_back(id);
		ImGui::PushID(id);
	}

	void PopID() // Leaks memory somehow
	{
		ImGuiWindow& window = *ImGui::GetCurrentWindow();
		//if (window.IDStack.Size == 0) id = 0;

		//window.IDStack.pop_back();
		ImGui::PopID();
	}

	void ShiftCursor(float x, float y)
	{
		const ImVec2 cursor = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(cursor.x + x, cursor.y + y));
	}

	void ShiftCursorX(float distance)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + distance);
	}

	void ShiftCursorY(float distance)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + distance);
	}

	void SetCursorPos(const glm::vec2& position)
	{
		ImGui::SetCursorPos(ImVec2(position.x, position.y));
	}

	void HelpMarker(const std::string& desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	bool Checkbox(const std::string& label, bool& value)
	{
		return ImGui::Checkbox(label.c_str(), &value);
	}

	glm::vec2 GetContentRegionAvail()
	{
		auto result = ImGui::GetContentRegionAvail();
		return glm::vec2(result.x, result.y);
	}

	void SetContextFontSize(float fontSize)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* font = io.Fonts->Fonts[0]; // Assuming you are using the first font, change as needed

		static bool firstRun = true;
		static float defaultScale = 0.0f;
		if (firstRun)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			defaultScale = window->FontWindowScale;
			firstRun = false;
		}

		if (fontSize == 0.0f)
		{
			ImGui::SetWindowFontScale(defaultScale);
			return;
		}

		ImGui::SetWindowFontScale(fontSize);
	}

	void CustomColourPicker(const std::string& label, glm::vec4& colour, float size)
	{
		UI::SetContextFontSize(size);
		ImGui::ColorEdit4(fmt::format("##{0}", label).c_str(), glm::value_ptr(colour), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);
		UI::SetContextFontSize(0.0f);

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			ImGui::OpenPopup(fmt::format("{0}##ColorPickerPopup", label).c_str());
		}

		if (ImGui::BeginPopup(fmt::format("{0}##ColorPickerPopup", label).c_str()))
		{
			ImGui::ColorPicker4(label.c_str(), glm::value_ptr(colour));
			ImGui::EndPopup();
		}
	}

	bool BeginWindow(const std::string& name, WindowFlags flags)
	{
		if (flags & WindowFlags::NoTabBar)
		{
			ImGuiWindowClass window = {};
			window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
			ImGui::SetNextWindowClass(&window);
		}

		return ImGui::Begin(name.c_str(), (bool*)0, (ImGuiWindowFlags)flags);
	}

	bool BeginWindow(const std::string& name, bool& open, WindowFlags flags)
	{
		if (flags & WindowFlags::NoTabBar)
		{
			ImGuiWindowClass window = {};
			window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
			ImGui::SetNextWindowClass(&window);
		}

		return ImGui::Begin(name.c_str(), &open, (ImGuiWindowFlags)flags);
	}

	void EndWindow()
	{
		ImGui::End();
	}

	void BeginChild(const std::string& name, const glm::vec2& size, bool border, WindowFlags flags)
	{
		ImGui::BeginChild(name.c_str(), ImVec2(size.x, size.y), border, (ImGuiWindowFlags)flags);
	}

	void EndChild()
	{
		ImGui::EndChild();
	}

	bool Button(const std::string& name, const glm::vec2& size)
	{
		return ImGui::Button(name.c_str(), ImVec2(size.x, size.y));
	}

	bool TreeNode(const std::string& name, TreeNodeFlags flags)
	{
		return ImGui::TreeNodeEx(name.c_str(), (ImGuiTreeNodeFlags)flags);
	}

	void TreeNodePop()
	{
		ImGui::TreePop();
	}

	bool Tree(const std::string& name, TreeNodeFlags flags)
	{
		bool open = false;

		if (UI::TreeNode(name, flags))
		{
			open = true;
			UI::TreeNodePop();
		}
		UI::ShiftCursorY(-1.0f);

		return open;
	}

	bool Selectable(const std::string& name, bool* selected, const glm::vec2& size, SelectableFlags flags)
	{
		return ImGui::Selectable(name.c_str(), selected, (ImGuiSelectableFlags)flags, ImVec2(size.x, size.y));
	}

	bool DragFloat(const std::string& name, float& value, float speed, float minValue, float maxValue, const std::string& format, SliderFlags flags)
	{
		return ImGui::DragFloat(name.c_str(), &value, speed, minValue, maxValue, format.c_str(), (ImGuiSliderFlags)flags);
	}

	bool DragFloat2(const std::string& name, glm::vec2& value, float speed, float minValue, float maxValue, const std::string& format, SliderFlags flags)
	{
		return ImGui::DragFloat2(name.c_str(), glm::value_ptr(value), speed, minValue, maxValue, format.c_str(), (ImGuiSliderFlags)flags);
	}

	bool DragFloat3(const std::string& name, glm::vec3& value, float speed, float minValue, float maxValue, const std::string& format, SliderFlags flags)
	{
		return ImGui::DragFloat3(name.c_str(), glm::value_ptr(value), speed, minValue, maxValue, format.c_str(), (ImGuiSliderFlags)flags);
	}

	bool DragFloat4(const std::string& name, glm::vec4& value, float speed, float minValue, float maxValue, const std::string& format, SliderFlags flags)
	{
		return ImGui::DragFloat4(name.c_str(), glm::value_ptr(value), speed, minValue, maxValue, format.c_str(), (ImGuiSliderFlags)flags);
	}

	bool BeginTable(const std::string& name, uint32_t columns, UI::TableFlags flags)
	{
		return ImGui::BeginTable(name.c_str(), (int)columns, (ImGuiTableFlags)flags);
	}

	void EndTable()
	{
		ImGui::EndTable();
	}

	void TableSetColumnIndex(uint32_t index)
	{
		ImGui::TableSetColumnIndex((int)index);
	}

	void TableNextRow(TableRowFlags flags, float minHeight)
	{
		ImGui::TableNextRow((ImGuiTableRowFlags)flags, minHeight);
	}

	bool BeginCombo(const std::string& name, const std::string& previewValue, ComboFlags flags)
	{
		return ImGui::BeginCombo(name.c_str(), previewValue.c_str(), (ImGuiComboFlags)flags);
	}

	void EndCombo()
	{
		ImGui::EndCombo();
	}

	void TableSetupColumn(const std::string& name, TableColumnFlags flags, float widthOrWeight)
	{
		ImGui::TableSetupColumn(name.c_str(), (ImGuiTableColumnFlags)flags, widthOrWeight);
	}

	bool InputText(const std::string& label, char* buffer, size_t size, InputTextFlags flags)
	{
		return ImGui::InputText(label.c_str(), buffer, size, (ImGuiInputTextFlags)flags);
	}

	void BeginPropertyGrid(const std::string& id, uint32_t columns)
	{
		UI::Style(UI::StyleType::ItemSpacing, { 8.0f, 8.0f }).Push();
		UI::Style(UI::StyleType::FramePadding, { 4.0f, 4.0f }).Push();

		//UI::PushID();
		ImGui::Columns(columns, id.c_str());
	}

	void EndPropertyGrid(const std::string& id)
	{
		ImGui::Columns(1, id.c_str());
		UI::Draw::Underline();
		UI::ShiftCursorY(18.0f);
		
		//UI::PopID();
		UI::Style::PopStyles(2); // ItemSpacing, FramePadding
	}

	void BeginCustomProperty(const std::string& label, const std::string& helpText)
	{
		UI::ShiftCursor(10.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		//UI::ShiftCursorY(4.0f);
		//ImGui::PushItemWidth(-1);
	}

	void EndCustomProperty()
	{
		//ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();
	}

	bool Property(const std::string& label, float& value, float delta, float min, float max, const std::string& format, const std::string& helpText)
	{
		UI::ShiftCursor(10.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		bool modified = UI::DragFloat(fmt::format("##{0}", label).c_str(), value, delta, min, max, format);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return modified;
	}

	bool Property(const std::string& label, glm::vec2& value, float delta, float min, float max, const std::string& format, const std::string& helpText)
	{
		UI::ShiftCursor(10.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		bool modified = UI::DragFloat2(fmt::format("##{0}", label).c_str(), value, delta, min, max, format);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return modified;
	}

	bool Property(const std::string& label, glm::vec3& value, float delta, float min, float max, const std::string& format, const std::string& helpText)
	{
		UI::ShiftCursor(10.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		bool modified = UI::DragFloat3(fmt::format("##{0}", label).c_str(), value, delta, min, max, format);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return modified;
	}

	bool Property(const std::string& label, glm::vec4& value, float delta, float min, float max, const std::string& format, const std::string& helpText)
	{
		UI::ShiftCursor(10.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		bool modified = UI::DragFloat4(fmt::format("##{0}", label).c_str(), value, delta, min, max, format);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return modified;
	}

	bool Property(const std::string& label, Combo& value, const std::string& helpText)
	{
		UI::ShiftCursor(10.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		UI::ComboFlags flags = UI::ComboFlags::HeightRegular;
		value.Render(label, flags);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return false;
	}

	bool Property(const std::string& label, ClickAbleImage& value, const std::string& helpText)
	{
		UI::ShiftCursor(16.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		value.Render();

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return false;
	}

	bool Property(const std::string& label, ColourPicker& value, const std::string& helpText)
	{
		UI::ShiftCursor(16.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		UI::ShiftCursorX(6.0f); // TODO: Maybe change based on size
		value.Render();

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return false;
	}

	bool Property(const std::string& label, ColourPicker& picker, ClickAbleImage& image, const std::string& helpText)
	{
		UI::ShiftCursor(16.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		picker.Render();
		ImGui::PopItemWidth();

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		image.Render();
		ImGui::PopItemWidth();

		ImGui::NextColumn();
		UI::Draw::Underline();

		return false;
	}

	bool Property(const std::string& label, const std::string& buttonName, ButtonClickFn clickAction, const std::string& helpText)
	{
		UI::ShiftCursor(10.0f, 9.0f);
		UI::Text(label);

		if (!helpText.empty())
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		bool pressed = UI::Button(buttonName);
		if (pressed) clickAction();

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return pressed;
	}

	bool UniformProperty(const std::string& label, glm::vec3& value, bool& uniform, float delta, float min, float max, const std::string& format, const std::string& hoverText)
	{
		UI::ShiftCursor(10.0f, 9.0f);
		UI::Text(label);

		UI::SameLine();
		UI::ShiftCursorY(-3.0f);
		UI::Checkbox(fmt::format("##{0}", label).c_str(), uniform);

		if (std::strlen(hoverText.c_str()) != 0)
		{
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(hoverText.c_str());
		}

		ImGui::NextColumn();
		UI::ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		bool modified = UI::DragFloat3(fmt::format("##{0}", label).c_str(), value, delta, min, max, format);
		if (uniform)
		{
			if (value.x == value.y && value.y != value.z)
				value = glm::vec3(value.z, value.z, value.z);
			else if (value.x != value.y && value.y == value.z)
				value = glm::vec3(value.x, value.x, value.x);
			else
				value = glm::vec3(value.y, value.y, value.y);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return modified;
	}

	bool BeginMainMenuBar()
	{
		return ImGui::BeginMainMenuBar();
	}

	void EndMainMenuBar()
	{
		ImGui::EndMainMenuBar();
	}

	bool BeginMenu(const std::string& name)
	{
		return ImGui::BeginMenu(name.c_str());
	}

	bool MenuItem(const std::string& name, const std::string& shortcut)
	{
		return ImGui::MenuItem(name.c_str(), shortcut.c_str());
	}

	bool MenuItem(const std::string& name, bool& enabled, const std::string& shortcut)
	{
		return ImGui::MenuItem(name.c_str(), shortcut.c_str(), &enabled);
	}

	void EndMenu()
	{
		ImGui::EndMenu();
	}

	void SameLine()
	{
		ImGui::SameLine();
	}

	void Dummy(const glm::vec2& size)
	{
		ImGui::Dummy(ImVec2(size.x, size.y));
	}

	void FullScreenOverlay(const glm::vec4& colour)
	{
		auto style = UI::ScopedStyle({ UI::StyleColourType::WindowBg, colour });

		auto& window = Application::Get().GetWindow();
		ImGui::SetNextWindowSize(ImVec2((float)window.GetWidth(), (float)window.GetHeight()));
		ImGui::SetNextWindowPos(ImVec2((float)window.GetPositionX(), (float)window.GetPositionY()));

		ImGuiWindowClass imWindow = {};
		imWindow.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&imWindow);

		UI::BeginWindow("FullScreenOverlay##LavenderUI", UI::WindowFlags::NoSavedSettings | UI::WindowFlags::NoDecoration);
		UI::EndWindow();
	}

	void Combo::Add(const std::pair<std::string, SelectionFunc>& item)
	{
		Items.push_back(item);
	}

	void Combo::Render(const std::string& label, UI::ComboFlags flags)
	{
		if (UI::BeginCombo(fmt::format("##{0}{1}", label, "_LavenderUI").c_str(), Preview, flags))
		{
			for (auto& item : Items)
			{
				bool selected = item.first == Selected;
				if (UI::Selectable(item.first, &selected))
				{
					if (item.second)
						item.second();
				}
				if (selected) { Selected = item.first; }
			}
			UI::EndCombo();
		}
	}

	void ClickAbleImage::Render()
	{
		ImGui::Image(Image->GetTextureID(), ImVec2(Size.x, Size.y)/*, {0, 1}, {1, 0}*/);

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();

			// TODO: Make customizable?
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 17.5f);
			ImGui::TextUnformatted(Image->GetSpecification().Path.string().c_str());
			ImGui::PopTextWrapPos();
			ImGui::Image(Image->GetTextureID(), ImVec2(192, 192)/*, {0, 1}, {1, 0}*/);

			ImGui::EndTooltip();

			if (ImGui::IsItemClicked())
				Action();

			// Open Delete/Remove
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				ImGui::OpenPopup("Remove image");
		}

		// Actual Delete/Remove
		if (ImGui::BeginPopup("Remove image"))
		{
			if (ImGui::MenuItem("Remove"))
				Image = nullptr;

			ImGui::EndPopup();
		}
	}

	ColourPicker::ColourPicker(glm::vec4& colour)
		: Colour(colour)
	{
	}

	void ColourPicker::Render()
	{
		CustomColourPicker(Label, Colour, Size * 0.05625f);
	}

}