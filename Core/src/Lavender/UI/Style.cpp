#include "lvpch.h"
#include "Style.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/UI/Colours.hpp"

namespace Lavender::UI
{

	Style::Style(StyleType type, float value)
		: Type(type), FloatValue(value)
	{
	}

	Style::Style(StyleType type, glm::vec2 value)
		: Type(type), Vec2Value(value)
	{
	}

	void Style::Push()
	{
		if (FloatValue.has_value())
			ImGui::PushStyleVar((ImGuiStyleVar)Type, FloatValue.value());
		else
			ImGui::PushStyleVar((ImGuiStyleVar)Type, ImVec2(Vec2Value.value().x, Vec2Value.value().y));
	}

	void Style::Pop()
	{
		ImGui::PopStyleVar();
	}

	void Style::PopStyles(uint32_t count)
	{
		ImGui::PopStyleVar((int)count);
	}

	StyleColour::StyleColour(StyleColourType type, glm::vec4 value)
		: Type(type), Value(value)
	{
	}

	StyleColour::StyleColour(StyleColourType type, uint32_t value)
		: Type(type), Value(UI::Colours::ConvertU32Colour(value))
	{

	}

	void StyleColour::Push()
	{
		ImGui::PushStyleColor((ImGuiCol)Type, ImVec4(Value.r, Value.g, Value.b, Value.a));
	}

	void StyleColour::Pop()
	{
		ImGui::PopStyleColor();
	}

	StyleList::StyleList(std::initializer_list<Style> elements)
		: m_Styles(elements)
	{
	}

	StyleList::StyleList(std::vector<Style> list)
		: m_Styles(list)
	{
	}

	StyleList::StyleList(std::initializer_list<StyleColour> elements)
		: m_Colours(elements)
	{
	}

	StyleList::StyleList(std::vector<StyleColour> list)
		: m_Colours(list)
	{
	}

	void StyleList::Push()
	{
		if (m_Styles.has_value())
		{
			for (auto& style : m_Styles.value())
				style.Push();
		}
		else if (m_Colours.has_value())
		{
			for (auto& style : m_Colours.value())
				style.Push();
		}
	}

	void StyleList::Pop()
	{
		if (m_Styles.has_value())
			ImGui::PopStyleVar((int)m_Styles.value().size());
		else if (m_Colours.has_value())
			ImGui::PopStyleColor((int)m_Colours.value().size());
	}

	ScopedStyle::ScopedStyle(const Style& style)
		: m_Style(std::make_optional(style))
	{
		m_Style.value().Push();
	}

	ScopedStyle::ScopedStyle(const StyleColour& colour)
		: m_Colour(std::make_optional(colour))
	{
		m_Colour.value().Push();
	}

	ScopedStyle::~ScopedStyle()
	{
		if (m_Style.has_value())
			m_Style.value().Pop();
		else
			m_Colour.value().Pop();
	}

	ScopedStyleList::ScopedStyleList(const StyleList& style)
		: m_Style(style)
	{
		m_Style.Push();
	}

	ScopedStyleList::~ScopedStyleList()
	{
		m_Style.Pop();
	}

}