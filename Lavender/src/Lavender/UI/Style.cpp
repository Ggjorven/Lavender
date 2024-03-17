#include "lvpch.h"
#include "Style.hpp"

#include "Lavender/Core/Logging.hpp"

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

	StyleList::StyleList(std::initializer_list<Style> elements)
		: m_Styles(elements)
	{
	}

	StyleList::StyleList(std::vector<Style> list)
		: m_Styles(list)
	{
	}

	void StyleList::Push()
	{
		for (auto& style : m_Styles)
			style.Push();
	}

	void StyleList::Pop()
	{
		ImGui::PopStyleVar((int)m_Styles.size());
	}

	StyleColour::StyleColour(StyleColourType type, glm::vec4 value)
		: Type(type), Value(value)
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

	StyleColourList::StyleColourList(std::initializer_list<StyleColour> elements)
		: m_Styles(elements)
	{
	}

	StyleColourList::StyleColourList(std::vector<StyleColour> list)
		: m_Styles(list)
	{
	}

	void StyleColourList::Push()
	{
		for (auto& style : m_Styles)
			style.Push();
	}

	void StyleColourList::Pop()
	{
		ImGui::PopStyleColor((int)m_Styles.size());
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
		: m_Style(std::make_optional(style))
	{
		m_Style.value().Push();
	}

	ScopedStyleList::ScopedStyleList(const StyleColourList& colour)
		: m_Colour(std::make_optional(colour))
	{
		m_Colour.value().Push();
	}

	ScopedStyleList::~ScopedStyleList()
	{
		if (m_Style.has_value())
			m_Style.value().Pop();
		else
			m_Colour.value().Pop();
	}

}