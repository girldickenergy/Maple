#include "Widgets.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <map>

#include "StyleProvider.h"

bool Widgets::Tab(const char* label, void* icon, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    // Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
    ImGuiID id = window->GetID(label);
    ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrLineTextBaseOffset;
    ImGui::ItemSize(size, 0.0f);

    // Fill horizontal space
    // We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitly right-aligned sizes not visibly match other widgets.
    const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
    const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
    const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
    if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
        size.x = ImMax(label_size.x, max_x - min_x);

    // Text stays at the submission position, but bounding box may be extended on both sides
    const ImVec2 text_min = ImVec2(pos.x + size.y, pos.y + size.y / 2 - ImGui::CalcTextSize(label).y / 2);
    const ImVec2 text_max(min_x + size.x, pos.y + size.y);

    // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
    ImRect bb(min_x, pos.y, text_max.x, text_max.y);
    if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
    {
        const float spacing_x = span_all_columns ? 0.0f : style.ItemSpacing.x;
        const float spacing_y = style.ItemSpacing.y;
        const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
        const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
        bb.Min.x -= spacing_L;
        bb.Min.y -= spacing_U;
        bb.Max.x += (spacing_x - spacing_L);
        bb.Max.y += (spacing_y - spacing_U);
    }
    //if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

    // Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackground for every Selectable..
    const float backup_clip_rect_min_x = window->ClipRect.Min.x;
    const float backup_clip_rect_max_x = window->ClipRect.Max.x;
    if (span_all_columns)
    {
        window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
        window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
    }

    bool item_add;
    if (flags & ImGuiSelectableFlags_Disabled)
    {
        ImGuiItemFlags backup_item_flags = window->DC.ItemFlags;
        window->DC.ItemFlags |= ImGuiItemFlags_Disabled | ImGuiItemFlags_NoNavDefaultFocus;
        item_add = ImGui::ItemAdd(bb, id);
        window->DC.ItemFlags = backup_item_flags;
    }
    else
    {
        item_add = ImGui::ItemAdd(bb, id);
    }

    if (span_all_columns)
    {
        window->ClipRect.Min.x = backup_clip_rect_min_x;
        window->ClipRect.Max.x = backup_clip_rect_max_x;
    }

    if (!item_add)
        return false;

    // FIXME: We can standardize the behavior of those two, we could also keep the fast path of override ClipRect + full push on render only,
    // which would be advantageous since most selectable are not selected.
    if (span_all_columns && window->DC.CurrentColumns)
        ImGui::PushColumnsBackground();
    else if (span_all_columns && g.CurrentTable)
        ImGui::TablePushBackgroundChannel();

    // We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
    ImGuiButtonFlags button_flags = 0;
    if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
    if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
    if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
    if (flags & ImGuiSelectableFlags_Disabled) { button_flags |= ImGuiButtonFlags_Disabled; }
    if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
    if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

    if (flags & ImGuiSelectableFlags_Disabled)
        selected = false;

    const bool was_selected = selected;
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

    // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
    if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
    {
        if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
        {
            ImGui::SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, ImRect(bb.Min - window->Pos, bb.Max - window->Pos));
            g.NavDisableHighlight = true;
        }
    }
    if (pressed)
        ImGui::MarkItemEdited(id);

    if (flags & ImGuiSelectableFlags_AllowItemOverlap)
        ImGui::SetItemAllowOverlap();

    // In this branch, Selectable() cannot toggle the selection so this will never trigger.
    if (selected != was_selected) //-V547
        window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

    const float elapsed = ImGui::GetIO().DeltaTime * 1000.f;
    const float animationTime = 150.f;
	
    static std::map<ImGuiID, float> hoverAnimationMap;
    auto hoverAnimation = hoverAnimationMap.find(id);
    if (hoverAnimation == hoverAnimationMap.end())
    {
        hoverAnimationMap.insert({ id, 0.f });
        hoverAnimation = hoverAnimationMap.find(id);
    }

    if (hovered && hoverAnimation->second < 1.f)
        hoverAnimation->second += elapsed / animationTime;

    if (!hovered && hoverAnimation->second > 0.f)
        hoverAnimation->second -= elapsed / animationTime;

    hoverAnimation->second = ImClamp(hoverAnimation->second, 0.f, 1.f);

    static std::map<ImGuiID, float> holdAnimationMap;
    auto holdAnimation = holdAnimationMap.find(id);
    if (holdAnimation == holdAnimationMap.end())
    {
        holdAnimationMap.insert({ id, 0.f });
        holdAnimation = holdAnimationMap.find(id);
    }

    if (held && holdAnimation->second < 1.f)
        holdAnimation->second += elapsed / animationTime;

    if (!held && holdAnimation->second > 0.f)
        holdAnimation->second -= elapsed / animationTime;

    holdAnimation->second = ImClamp(holdAnimation->second, 0.f, 1.f);
	
    // Render
    if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
        hovered = true;

    ImColor col = ImLerp(selected ? style.Colors[ImGuiCol_Header] : ImVec4(style.Colors[ImGuiCol_Header].x, style.Colors[ImGuiCol_Header].y, style.Colors[ImGuiCol_Header].z, 0), style.Colors[ImGuiCol_HeaderHovered], hoverAnimation->second);
	if (held)
        col = ImLerp(col, style.Colors[ImGuiCol_HeaderActive], holdAnimation->second);
    else
        col = ImLerp(style.Colors[ImGuiCol_HeaderActive], col, 1.f - holdAnimation->second);

    ImGui::RenderFrame(bb.Min, bb.Max, col, false, style.FrameRounding);
    ImGui::RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

    if (span_all_columns && window->DC.CurrentColumns)
        ImGui::PopColumnsBackground();
    else if (span_all_columns && g.CurrentTable)
        ImGui::TablePopBackgroundChannel();

    window->DrawList->AddImage(icon, ImVec2(pos.x + size.y / 4, pos.y + size.y / 4), ImVec2(pos.x + size.y / 4 + size.y / 2, pos.y + size.y / 4 + size.y / 2), ImVec2(0, 0), ImVec2(1, 1), ImColor(StyleProvider::TextColour));

    if (flags & ImGuiSelectableFlags_Disabled) ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
    ImGui::RenderTextClipped(text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);
    if (flags & ImGuiSelectableFlags_Disabled) ImGui::PopStyleColor();

    // Automatically close popups
    if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
        ImGui::CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return pressed;
}

float Widgets::CalcPanelHeight(int widgetCount, int textCount, int spacingCount)
{
	const float totalWidgetHeight = ImGui::GetFrameHeight() * static_cast<float>(widgetCount);
	const float totalTextHeight = ImGui::GetCurrentContext()->FontSize * static_cast<float>(textCount);
	const float totalSpacingHeight = static_cast<float>(widgetCount + textCount + spacingCount - 1) * ImGui::GetStyle().ItemSpacing.y;
	
    return totalWidgetHeight + totalTextHeight + totalSpacingHeight;
}

void Widgets::BeginPanel(const char* label, const ImVec2& size)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::PushFont(StyleProvider::FontDefaultBold);
	
    const ImVec2 labelSize = ImGui::CalcTextSize(label);

    const float titleBarHeight = labelSize.y + StyleProvider::Padding.y * 2;

    const ImColor gradientStartColour = ImColor(StyleProvider::MenuColourVeryDark.x + 0.025f, StyleProvider::MenuColourVeryDark.y + 0.025f, StyleProvider::MenuColourVeryDark.z + 0.025f, StyleProvider::MenuColourVeryDark.w);

    ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + titleBarHeight), ImColor(StyleProvider::MenuColourVeryDark), style.ChildRounding, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight);
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + titleBarHeight), ImVec2(pos.x + size.x, pos.y + titleBarHeight + size.y + StyleProvider::Padding.y * 2), ImColor(StyleProvider::MenuColourDark), style.ChildRounding, ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight);
    ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + titleBarHeight), ImVec2(pos.x + size.x, pos.y + titleBarHeight + StyleProvider::Padding.y), gradientStartColour, gradientStartColour, ImColor(StyleProvider::MenuColourDark), ImColor(StyleProvider::MenuColourDark));
	
    ImGui::SetCursorScreenPos(ImVec2(pos.x + size.x / 2 - labelSize.x / 2, pos.y + titleBarHeight / 2 - labelSize.y / 2));
    ImGui::TextColored(StyleProvider::AccentColour, label);

    ImGui::PopFont();

    ImGui::SetCursorScreenPos(ImVec2(pos.x + StyleProvider::Padding.x, pos.y + titleBarHeight + StyleProvider::Padding.y));
    ImGui::BeginChild(label, ImVec2(size.x - StyleProvider::Padding.x * 2, size.y), false, ImGuiWindowFlags_NoBackground);
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
}

void Widgets::EndPanel()
{
    ImGui::EndChild();
}

bool Widgets::Checkbox(const char* label, bool* v)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    const ImVec2 checkMarkSize = ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight());
    const ImVec2 overlapAllowance = checkMarkSize / 6;
    const ImVec2 checkboxSize = ImVec2(ImGui::GetFrameHeight() * 2 - overlapAllowance.x * 2, checkMarkSize.y - overlapAllowance.y * 2);

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, pos + ImVec2(checkboxSize.x + overlapAllowance.x * 2 + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id))
    {
        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return false;
    }

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        ImGui::MarkItemEdited(id);
    }

    const float elapsed = ImGui::GetIO().DeltaTime * 1000.f;
    const float animationTime = 250.f;

    static std::map<ImGuiID, float> positionAnimationMap;
    auto positionAnimation = positionAnimationMap.find(id);
    if (positionAnimation == positionAnimationMap.end())
    {
        positionAnimationMap.insert({ id, 0.f });
        positionAnimation = positionAnimationMap.find(id);
    }
	
    if (*v && positionAnimation->second < 1.f)
        positionAnimation->second += elapsed / animationTime;
	
    if (!*v && positionAnimation->second > 0.f)
        positionAnimation->second -= elapsed / animationTime;

    positionAnimation->second = ImClamp(positionAnimation->second, 0.f, 1.f);

    static std::map<ImGuiID, float> colourAnimationMap;
    auto colourAnimation = colourAnimationMap.find(id);
    if (colourAnimation == colourAnimationMap.end())
    {
        colourAnimationMap.insert({ id, 0.f });
        colourAnimation = colourAnimationMap.find(id);
    }

    if (positionAnimation->second == 0.f || positionAnimation->second == 1.f)
    {
        if (hovered && colourAnimation->second < 1.f)
            colourAnimation->second += elapsed / animationTime;

        if (!hovered && colourAnimation->second > 0.f)
            colourAnimation->second -= elapsed / animationTime;

        colourAnimation->second = ImClamp(colourAnimation->second, 0.f, 1.f);
    }
    else
    {
        colourAnimation->second = 0.f; //ignore colour animation while position animation is running
    }

    ImColor checkboxColour;
    ImColor checkMarkColour;
    if (positionAnimation->second > 0.f && positionAnimation->second < 1.f)
    {
        checkboxColour = ImLerp(style.Colors[ImGuiCol_FrameBgHovered], style.Colors[*v ? ImGuiCol_FrameBg : ImGuiCol_FrameBgActive], *v ? positionAnimation->second : 1.f - positionAnimation->second);
        checkMarkColour = ImLerp(StyleProvider::CheckMarkHoveredColour, *v? StyleProvider::CheckMarkColour : StyleProvider::CheckMarkActiveColour, *v ? positionAnimation->second : 1.f - positionAnimation->second);
    }
    else
    {
        checkboxColour = ImLerp(style.Colors[*v ? ImGuiCol_FrameBg : ImGuiCol_FrameBgActive], style.Colors[ImGuiCol_FrameBgHovered], colourAnimation->second);
        checkMarkColour = ImLerp(*v ? StyleProvider::CheckMarkColour : StyleProvider::CheckMarkActiveColour, StyleProvider::CheckMarkHoveredColour, colourAnimation->second);
    }
	
    window->DrawList->AddRectFilled(total_bb.Min + overlapAllowance, total_bb.Min + overlapAllowance + checkboxSize, checkboxColour, 60.f);
    window->DrawList->AddCircleFilled(ImVec2(total_bb.Min.x + checkMarkSize.x / 2 + ImLerp(0.f, checkboxSize.x - overlapAllowance.x - checkMarkSize.x / 2, positionAnimation->second), total_bb.Min.y + checkMarkSize.y / 2), checkMarkSize.y / 2, checkMarkColour, 36);

    bool mixed_value = (window->DC.ItemFlags & ImGuiItemFlags_MixedValue) != 0;
    ImVec2 label_pos = ImVec2(total_bb.Min.x + overlapAllowance.x * 2 + checkboxSize.x + style.ItemInnerSpacing.x, total_bb.Min.y + style.FramePadding.y);
    if (g.LogEnabled)
	    ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
    if (label_size.x > 0.0f)
	    ImGui::RenderText(label_pos, label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

bool Widgets::ButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;
    bool hovered, held;
    const bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

    const float elapsed = ImGui::GetIO().DeltaTime * 1000.f;
    const float animationTime = 150.f;

    static std::map<ImGuiID, float> hoverAnimationMap;
    auto hoverAnimation = hoverAnimationMap.find(id);
    if (hoverAnimation == hoverAnimationMap.end())
    {
        hoverAnimationMap.insert({ id, 0.f });
        hoverAnimation = hoverAnimationMap.find(id);
    }

    if (hovered && hoverAnimation->second < 1.f)
        hoverAnimation->second += elapsed / animationTime;

    if (!hovered && hoverAnimation->second > 0.f)
        hoverAnimation->second -= elapsed / animationTime;

    hoverAnimation->second = ImClamp(hoverAnimation->second, 0.f, 1.f);

    static std::map<ImGuiID, float> holdAnimationMap;
    auto holdAnimation = holdAnimationMap.find(id);
    if (holdAnimation == holdAnimationMap.end())
    {
        holdAnimationMap.insert({ id, 0.f });
        holdAnimation = holdAnimationMap.find(id);
    }

    if (held && holdAnimation->second < 1.f)
        holdAnimation->second += elapsed / animationTime;

    if (!held && holdAnimation->second > 0.f)
        holdAnimation->second -= elapsed / animationTime;

    holdAnimation->second = ImClamp(holdAnimation->second, 0.f, 1.f);

    // Render
    ImGui::RenderNavHighlight(bb, id);

    ImColor col = ImLerp(style.Colors[ImGuiCol_Button], style.Colors[ImGuiCol_ButtonHovered], hoverAnimation->second);
    if (held)
        col = ImLerp(col, style.Colors[ImGuiCol_ButtonActive], holdAnimation->second);
    else
        col = ImLerp(style.Colors[ImGuiCol_ButtonActive], col, 1.f - holdAnimation->second);

    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

    if (g.LogEnabled)
	    ImGui::LogSetNextTextDecoration("[", "]");

    ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    // Automatically close popups
    if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
	    ImGui::CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
    return pressed;
}

bool Widgets::Button(const char* label, const ImVec2& size_arg)
{
    return ButtonEx(label, size_arg, ImGuiButtonFlags_None);
}
