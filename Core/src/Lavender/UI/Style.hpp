#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include <string>
#include <optional>
#include <initializer_list>

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace Lavender::UI
{

    enum class StyleType : uint8_t
    {
        Alpha,               
        DisabledAlpha,       
        WindowPadding,       
        WindowRounding,      
        WindowBorderSize,    
        WindowMinSize,       
        WindowTitleAlign,    
        ChildRounding,       
        ChildBorderSize,     
        PopupRounding,       
        PopupBorderSize,     
        FramePadding,        
        FrameRounding,       
        FrameBorderSize,     
        ItemSpacing,         
        ItemInnerSpacing,    
        IndentSpacing,       
        CellPadding,         
        ScrollbarSize,       
        ScrollbarRounding,   
        GrabMinSize,         
        GrabRounding,        
        TabRounding,         
        ButtonTextAlign,     
        SelectableTextAlign
    };

    struct Style
    {
    public:
        StyleType Type = {};

        std::optional<float> FloatValue = {};
        std::optional<glm::vec2> Vec2Value = {};

    public:
        Style() = default;
        Style(StyleType type, float value);
        Style(StyleType type, glm::vec2 value);
        virtual ~Style() = default;

        void Push();
        void Pop();

        static void PopStyles(uint32_t count);
    };

    enum class StyleColourType : uint8_t
    {
        Text,
        TextDisabled,
        WindowBg,              
        ChildBg,               
        PopupBg,               
        Border,
        BorderShadow,
        FrameBg,               
        FrameBgHovered,
        FrameBgActive,
        TitleBg,
        TitleBgActive,
        TitleBgCollapsed,
        MenuBarBg,
        ScrollbarBg,
        ScrollbarGrab,
        ScrollbarGrabHovered,
        ScrollbarGrabActive,
        CheckMark,
        SliderGrab,
        SliderGrabActive,
        Button,
        ButtonHovered,
        ButtonActive,
        Header,                
        HeaderHovered,
        HeaderActive,
        Separator,
        SeparatorHovered,
        SeparatorActive,
        ResizeGrip,
        ResizeGripHovered,
        ResizeGripActive,
        Tab,
        TabHovered,
        TabActive,
        TabUnfocused,
        TabUnfocusedActive,
        DockingPreview,        
        DockingEmptyBg,        
        PlotLines,
        PlotLinesHovered,
        PlotHistogram,
        PlotHistogramHovered,
        TableHeaderBg,         
        TableBorderStrong,     
        TableBorderLight,      
        TableRowBg,            
        TableRowBgAlt,         
        TextSelectedBg,
        DragDropTarget,
        NavHighlight,         
        NavWindowingHighlight,
        NavWindowingDimBg,
        ModalWindowDimBg    
    };

    struct StyleColour
    {
    public:
        StyleColourType Type = {};
        glm::vec4 Value = {};

    public:
        StyleColour() = default;
        StyleColour(StyleColourType type, glm::vec4 value);
        StyleColour(StyleColourType type, uint32_t value);
        virtual ~StyleColour() = default;

        void Push();
        void Pop();
    };

    struct StyleList
    {
    public:
        StyleList() = default;
        StyleList(std::initializer_list<Style> elements);
        StyleList(std::vector<Style> list);
        StyleList(std::initializer_list<StyleColour> elements);
        StyleList(std::vector<StyleColour> list);
        StyleList(const StyleList& list) = default;
        virtual ~StyleList() = default;

        void Push();
        void Pop();

        inline void Add(const Style& style) { m_Styles.value().push_back(style); }
        inline void Add(const StyleColour& style) { m_Colours.value().push_back(style); }
        inline void SetStyles(std::vector<Style> styles) { m_Styles = styles; }
        inline void SetStyles(std::vector<StyleColour> styles) { m_Colours = styles; }
        inline void SetStyles(std::initializer_list<Style> styles) { m_Styles = styles; }
        inline void SetStyles(std::initializer_list<StyleColour> styles) { m_Colours = styles; }

    private:
        std::optional<std::vector<Style>> m_Styles = { };
        std::optional<std::vector<StyleColour>> m_Colours = { };
    };

    

    struct ScopedStyle
    {
    public:
        ScopedStyle(const Style& style);
        ScopedStyle(const StyleColour& colour);
        virtual ~ScopedStyle();

    private:
        std::optional<Style> m_Style = {};
        std::optional<StyleColour> m_Colour = {};
    };

    struct ScopedStyleList
    {
    public:
        ScopedStyleList(const StyleList& style);
        virtual ~ScopedStyleList();

    private:
        StyleList m_Style = {};
    };

}