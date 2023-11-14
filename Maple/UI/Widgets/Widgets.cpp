#include "Widgets.h"

#include <map>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "xorstr.hpp"

#include "../StyleProvider.h"
#include "../../Utilities/Strings/StringUtilities.h"

const char* Widgets::patchFormatStringFloatToInt(const char* fmt)
{
    if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
        return "%d";
    const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
    if (fmt_end > fmt_start && fmt_end[-1] == 'f')
    {
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        if (fmt_start == fmt && fmt_end[0] == 0)
            return "%d";
        ImGuiContext& g = *GImGui;
        ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
        return g.TempBuffer;
#else
        IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
    }
    return fmt;
}

bool Widgets::items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}

float Widgets::calcMaxPopupHeightFromItemCount(int items_count)
{
    ImGuiContext& g = *GImGui;
    if (items_count <= 0)
        return FLT_MAX;
    return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool Widgets::Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
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
    const ImVec2 text_min = ImVec2(pos.x + style.FramePadding.x, pos.y);
    const ImVec2 text_max(min_x + size.x + style.FramePadding.x, pos.y + size.y);

    // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
    ImRect bb(min_x, pos.y, text_max.x - style.FramePadding.x, text_max.y);
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

    if (flags & ImGuiSelectableFlags_Disabled) ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
    ImGui::RenderTextClipped(text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);
    if (flags & ImGuiSelectableFlags_Disabled) ImGui::PopStyleColor();

    // Automatically close popups
    if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
        ImGui::CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return pressed;
}

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

    ImGui::BeginChild(StringUtilities::GenerateRandomString(16, StringUtilities::GenerateRandomString(4, label)).c_str(), ImVec2(size.x, size.y + titleBarHeight + StyleProvider::Padding.y * 2), false, ImGuiWindowFlags_NoBackground);
    {
        ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + titleBarHeight), ImColor(StyleProvider::MenuColourVeryDark), style.ChildRounding, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight);
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + titleBarHeight), ImVec2(pos.x + size.x, pos.y + titleBarHeight + size.y + StyleProvider::Padding.y * 2), ImColor(StyleProvider::MenuColourDark), style.ChildRounding, ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight);
        ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + titleBarHeight), ImVec2(pos.x + size.x, pos.y + titleBarHeight + StyleProvider::Padding.y), ImColor(StyleProvider::PanelGradientStartColour), ImColor(StyleProvider::PanelGradientStartColour), ImColor(StyleProvider::MenuColourDark), ImColor(StyleProvider::MenuColourDark));

        ImGui::SetCursorPos(ImVec2(size.x / 2 - labelSize.x / 2, titleBarHeight / 2 - labelSize.y / 2));
        ImGui::TextColored(StyleProvider::AccentColour, label);
        ImGui::PopFont();

        ImGui::SetCursorPos(ImVec2(StyleProvider::Padding.x, titleBarHeight + StyleProvider::Padding.y));
        ImGui::BeginChild(StringUtilities::GenerateRandomString(16, StringUtilities::GenerateRandomString(8, label)).c_str(), ImVec2(size.x - StyleProvider::Padding.x * 2, size.y), false, ImGuiWindowFlags_NoBackground);
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
    }
}

void Widgets::EndPanel()
{
    ImGui::EndChild();
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
    const ImVec2 overlapAllowance = checkMarkSize / 8;
    const ImVec2 checkboxSize = ImVec2(ImGui::GetFrameHeight() * 2 - overlapAllowance.x * 2, checkMarkSize.y - overlapAllowance.y * 2);

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect frame_bb(pos, pos + ImVec2(checkboxSize.x + overlapAllowance.x * 2, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(pos, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.f));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id))
    {
        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return false;
    }

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        ImGui::MarkItemEdited(id);
    }

    const float elapsed = ImGui::GetIO().DeltaTime * 1000.f;
    const float animationTime = 150.f;

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
        checkMarkColour = ImLerp(StyleProvider::CheckMarkHoveredColour, *v ? StyleProvider::CheckMarkColour : StyleProvider::CheckMarkActiveColour, *v ? positionAnimation->second : 1.f - positionAnimation->second);
    }
    else
    {
        checkboxColour = ImLerp(style.Colors[*v ? ImGuiCol_FrameBg : ImGuiCol_FrameBgActive], style.Colors[ImGuiCol_FrameBgHovered], colourAnimation->second);
        checkMarkColour = ImLerp(*v ? StyleProvider::CheckMarkColour : StyleProvider::CheckMarkActiveColour, StyleProvider::CheckMarkHoveredColour, colourAnimation->second);
    }

    window->DrawList->AddRectFilled(frame_bb.Min + overlapAllowance, frame_bb.Max - overlapAllowance, checkboxColour, 60.f);
    window->DrawList->AddCircleFilled(ImVec2(ImLerp(frame_bb.Min.x + checkMarkSize.x / 2, frame_bb.Max.x - checkMarkSize.x / 2, positionAnimation->second), frame_bb.Min.y + checkMarkSize.y / 2), checkMarkSize.y / 2, checkMarkColour, 36);

    bool mixed_value = (window->DC.ItemFlags & ImGuiItemFlags_MixedValue) != 0;
    ImVec2 label_pos = ImVec2(total_bb.Max.x - label_size.x, total_bb.Min.y + style.FramePadding.y);
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
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    ImGui::CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
    return pressed;
}

bool Widgets::Button(const char* label, const ImVec2& size_arg)
{
    return ButtonEx(label, size_arg, ImGuiButtonFlags_None);
}

bool Widgets::SmallButton(const char* label)
{
    ImGuiContext& g = *GImGui;
    float backup_padding_y = g.Style.FramePadding.y;
    g.Style.FramePadding.y = 0.0f;
    bool pressed = ButtonEx(label, ImVec2(0, 0), ImGuiButtonFlags_AlignTextBaseLine);
    g.Style.FramePadding.y = backup_padding_y;
    return pressed;
}

bool Widgets::Hotkey(const char* label, int* k)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    char buf_display[64] = "None";

    bool active = false;
    if (*k != 0 && g.ActiveId != id)
        strcpy_s(buf_display, keyNames[*k]);
    else if (g.ActiveId == id)
    {
        active = true;
        strcpy_s(buf_display, "<Press a key>");
    }

    const ImVec2 label_size = ImGui::CalcTextSize(label);
    const ImVec2 hotkey_size = ImGui::CalcTextSize(buf_display, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + hotkey_size + ImVec2(style.FramePadding.x * 2, style.FramePadding.y * 2));
    const ImRect total_bb(window->DC.CursorPos, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.f));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;

    const bool focus_requested = ImGui::FocusableItemRegister(window, g.ActiveId == id);
    const bool hovered = ImGui::ItemHoverable(frame_bb, id);

    if (hovered)
    {
        ImGui::SetHoveredID(id);
        g.MouseCursor = ImGuiMouseCursor_TextInput;
    }

    const bool user_clicked = hovered && io.MouseClicked[0];

    if (focus_requested || user_clicked)
    {
        if (g.ActiveId != id)
        {
            memset(io.MouseDown, 0, sizeof(io.MouseDown));
            memset(io.KeysDown, 0, sizeof(io.KeysDown));
            *k = 0;
        }

        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (io.MouseClicked[0] && g.ActiveId == id)
        ImGui::ClearActiveID();

    bool value_changed = false;
    int key = *k;

    if (g.ActiveId == id)
    {
        for (auto i = 0; i <= 6; i++)
        {
            if (io.MouseDown[i] || GetAsyncKeyState(VK_XBUTTON1) || GetAsyncKeyState(VK_XBUTTON2))
            {
                switch (i)
                {
                case 0:
                    key = VK_LBUTTON;
                    break;
                case 1:
                    key = VK_RBUTTON;
                    break;
                case 2:
                    key = VK_MBUTTON;
                    break;
                }

                if (GetAsyncKeyState(VK_XBUTTON2))
                    key = VK_XBUTTON2;

                if (GetAsyncKeyState(VK_XBUTTON1))
                    key = VK_XBUTTON1;

                value_changed = true;
                ImGui::ClearActiveID();
            }
        }

        if (!value_changed)
        {
            for (auto i = VK_BACK; i <= VK_OEM_8; i++)
            {
                if (io.KeysDown[i])
                {
                    key = i;
                    value_changed = true;

                    ImGui::ClearActiveID();
                }
            }
        }

        if (ImGui::IsKeyPressedMap(ImGuiKey_Escape, false))
        {
            *k = 0;
            ImGui::ClearActiveID();
        }
        else
            *k = key;
    }

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

    static std::map<ImGuiID, float> activeAnimationMap;
    auto activeAnimation = activeAnimationMap.find(id);
    if (activeAnimation == activeAnimationMap.end())
    {
        activeAnimationMap.insert({ id, 0.f });
        activeAnimation = activeAnimationMap.find(id);
    }

    if (active && activeAnimation->second < 1.f)
        activeAnimation->second += elapsed / animationTime;

    if (!active && activeAnimation->second > 0.f)
        activeAnimation->second -= elapsed / animationTime;

    activeAnimation->second = ImClamp(activeAnimation->second, 0.f, 1.f);

    ImColor col = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hoverAnimation->second);
    if (active)
        col = ImLerp(col, style.Colors[ImGuiCol_FrameBgActive], activeAnimation->second);
    else
        col = ImLerp(style.Colors[ImGuiCol_FrameBgActive], col, 1.f - activeAnimation->second);

    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, col, style.FrameRounding);

    const ImVec2 label_pos = ImVec2(total_bb.Max.x - label_size.x, total_bb.Min.y + style.FramePadding.y);
    if (label_size.x > 0.0f)
        ImGui::RenderText(label_pos, label);

    ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, buf_display, NULL, &hotkey_size, ImVec2(0.5f, 0.5f), &frame_bb);

    return value_changed;
}

bool Widgets::InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;

    char buf[64];
    ImGui::DataTypeFormatString(buf, IM_ARRAYSIZE(buf), data_type, p_data, format);

    bool value_changed = false;
    if ((flags & (ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsScientific)) == 0)
        flags |= ImGuiInputTextFlags_CharsDecimal;
    flags |= ImGuiInputTextFlags_AutoSelectAll;
    flags |= ImGuiInputTextFlags_NoMarkEdited;  // We call MarkItemEdited() ourselves by comparing the actual data rather than the string.

    char initialTextA[512];
    g.InputTextState.InitialTextA.DecryptTo(initialTextA);

    if (p_step != NULL)
    {
        const float button_size = ImGui::GetFrameHeight();

        ImGui::BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
        ImGui::PushID(label);
        ImGui::SetNextItemWidth(ImMax(1.0f, ImGui::CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
        if (ImGui::InputText("", buf, IM_ARRAYSIZE(buf), flags)) // PushId(label) + "" gives us the expected ID from outside point of view
            value_changed = ImGui::DataTypeApplyOpFromText(buf, initialTextA, data_type, p_data, format);

        // Step buttons
        const ImVec2 backup_frame_padding = style.FramePadding;
        style.FramePadding.x = style.FramePadding.y;
        ImGuiButtonFlags button_flags = ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups;
        if (flags & ImGuiInputTextFlags_ReadOnly)
            button_flags |= ImGuiButtonFlags_Disabled;
        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("-", ImVec2(button_size, button_size), button_flags))
        {
            ImGui::DataTypeApplyOp(data_type, '-', p_data, p_data, g.IO.KeyCtrl && p_step_fast ? p_step_fast : p_step);
            value_changed = true;
        }
        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("+", ImVec2(button_size, button_size), button_flags))
        {
            ImGui::DataTypeApplyOp(data_type, '+', p_data, p_data, g.IO.KeyCtrl && p_step_fast ? p_step_fast : p_step);
            value_changed = true;
        }

        const char* label_end = ImGui::FindRenderedTextEnd(label);
        if (label != label_end)
        {
            ImGui::SameLine(0, style.ItemInnerSpacing.x);
            ImGui::TextEx(label, label_end);
        }
        style.FramePadding = backup_frame_padding;

        ImGui::PopID();
        ImGui::EndGroup();
    }
    else
    {
        if (ImGui::InputText(label, buf, IM_ARRAYSIZE(buf), flags))
            value_changed = ImGui::DataTypeApplyOpFromText(buf, initialTextA, data_type, p_data, format);
    }
    if (value_changed)
        ImGui::MarkItemEdited(window->DC.LastItemId);

    return value_changed;
}

bool Widgets::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = ImGui::CalcItemWidth();

    const ImVec2 sliderGrabSize = ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight());
    const ImVec2 overlapAllowance = sliderGrabSize / 8;

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
        format = patchFormatStringFloatToInt(format);

    const bool hovered = ImGui::ItemHoverable(frame_bb, id);
    const bool clicked = (hovered && g.IO.MouseClicked[0]);
    if (clicked || g.NavActivateId == id || g.NavInputId == id)
    {
        ImGui::SetActiveID(id, window);
        ImGui::SetFocusID(id, window);
        ImGui::FocusWindow(window);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
    }

    // Slider behavior
    ImRect grab_bb;
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 0);
    const bool value_changed = ImGui::SliderBehavior(ImRect(frame_bb.Min + ImVec2(sliderGrabSize.x / 2 - overlapAllowance.x, overlapAllowance.y), frame_bb.Max - ImVec2(sliderGrabSize.x / 2 - overlapAllowance.x, overlapAllowance.y)), id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
    ImGui::PopStyleVar();

    if (value_changed)
        ImGui::MarkItemEdited(id);

    ImVec2 grabPos = grab_bb.GetCenter();
    grabPos = ImClamp(grabPos, frame_bb.Min + ImVec2(sliderGrabSize.x / 2, 0), frame_bb.Max - ImVec2(sliderGrabSize.x / 2, 0));

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

    ImColor sliderColour = ImColor(ImLerp(style.Colors[ImGuiCol_FrameBgActive], style.Colors[ImGuiCol_FrameBgHovered], hoverAnimation->second));
    ImColor sliderFillColour = ImColor(ImLerp(StyleProvider::SliderColour, StyleProvider::SliderHoveredColour, hoverAnimation->second));
    ImColor sliderGrabColour = ImColor(ImLerp(StyleProvider::SliderGrabColour, StyleProvider::SliderGrabHoveredColour, hoverAnimation->second));

    // Draw frame
    ImGui::RenderNavHighlight(frame_bb, id);
    ImGui::RenderFrame(frame_bb.Min + overlapAllowance, frame_bb.Max - overlapAllowance, sliderColour, true, g.Style.FrameRounding);
    ImGui::RenderFrame(frame_bb.Min + overlapAllowance, ImVec2(grabPos.x, frame_bb.Max.y - overlapAllowance.y), sliderFillColour, true, g.Style.FrameRounding);

    // Render grab
    window->DrawList->AddCircleFilled(grabPos, sliderGrabSize.y / 2, sliderGrabColour, 36);

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return value_changed;
}

bool Widgets::SliderInt(const char* label, int* v, int v_min, int v_max, int step, int stepFast, const char* format, ImGuiSliderFlags flags)
{
    const float totalWidth = ImGui::GetWindowWidth() * 0.5f - ImGui::GetStyle().ItemInnerSpacing.x;
    const float sliderWidth = totalWidth * 0.6f;
    const float inputWidth = totalWidth - sliderWidth;

    ImGui::PushItemWidth(sliderWidth);
    const bool sliderValueChanged = SliderScalar(("###slider-" + std::string(label)).c_str(), ImGuiDataType_S32, v, &v_min, &v_max, format, flags);

    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::PushItemWidth(inputWidth);
    const bool inputValueChanged = InputScalar(label, ImGuiDataType_S32, v, &step, &stepFast, format);

    if ((flags & ImGuiSliderFlags_AlwaysClamp) != 0)
        *v = ImClamp(*v, v_min, v_max);

    return sliderValueChanged || inputValueChanged;
}

bool Widgets::SliderFloat(const char* label, float* v, float v_min, float v_max, float step, float stepFast, const char* format, ImGuiSliderFlags flags)
{
    const float totalWidth = ImGui::GetWindowWidth() * 0.5f - ImGui::GetStyle().ItemInnerSpacing.x;
    const float sliderWidth = totalWidth * 0.6f;
    const float inputWidth = totalWidth - sliderWidth;

    ImGui::PushItemWidth(sliderWidth);
    const bool sliderValueChanged = SliderScalar(("###slider-" + std::string(label)).c_str(), ImGuiDataType_Float, v, &v_min, &v_max, format, flags);

    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::PushItemWidth(inputWidth);
    const bool inputValueChanged = InputScalar(label, ImGuiDataType_Float, v, &step, &stepFast, format);

    if ((flags & ImGuiSliderFlags_AlwaysClamp) != 0)
        *v = ImClamp(*v, v_min, v_max);

    return sliderValueChanged || inputValueChanged;
}

bool Widgets::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
    // Always consume the SetNextWindowSizeConstraint() call in our early return paths
    ImGuiContext& g = *GImGui;
    bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
    g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? ImVec2(0, 0) : ImVec2(ImGui::GetFrameHeight() / 1.5, ImGui::GetFrameHeight() / 1.5 * 0.625f);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    const float expected_w = ImGui::CalcItemWidth();
    const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size.x : expected_w;
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
    bool popup_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

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

    const ImColor frame_col = ImLerp(style.Colors[ImGuiCol_FrameBg], style.Colors[ImGuiCol_FrameBgHovered], hoverAnimation->second);
    const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size.x);
    ImGui::RenderNavHighlight(frame_bb, id);
    if (!(flags & ImGuiComboFlags_NoPreview))
        window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, frame_col, style.FrameRounding, ImDrawFlags_RoundCornersAll);
    if (!(flags & ImGuiComboFlags_NoArrowButton))
    {
        window->DrawList->AddImage(StyleProvider::ChevronIconTexture, ImVec2(frame_bb.Max.x - arrow_size.x - (frame_bb.Max.y - frame_bb.Min.y - arrow_size.y) / 2, frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2 - arrow_size.y / 2), ImVec2(frame_bb.Max.x - (frame_bb.Max.y - frame_bb.Min.y - arrow_size.y) / 2, frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2 + arrow_size.y / 2), ImVec2(0, 0), ImVec2(1, 1), ImColor(StyleProvider::TextColour));
    }
    ImGui::RenderFrameBorder(frame_bb.Min, frame_bb.Max, style.FrameRounding);
    if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
    {
        ImVec2 preview_pos = frame_bb.Min + style.FramePadding;
        if (g.LogEnabled)
            ImGui::LogSetNextTextDecoration("{", "}");
        ImGui::RenderTextClipped(preview_pos, ImVec2(value_x2, frame_bb.Max.y), preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));
    }
    if (label_size.x > 0)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    if ((pressed || g.NavActivateId == id) && !popup_open)
    {
        if (window->DC.NavLayerCurrent == 0)
            window->NavLastIds[0] = id;

        ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);
        popup_open = true;
    }

    if (!popup_open)
        return false;

    if (has_window_size_constraint)
    {
        g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
    }
    else
    {
        if ((flags & ImGuiComboFlags_HeightMask_) == 0)
            flags |= ImGuiComboFlags_HeightRegular;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));    // Only one
        int popup_max_height_in_items = -1;
        if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
        else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
        else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
        ImGui::SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, calcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
    }

    char name[16];
    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

    // Position the window given a custom constraint (peak into expected window size so we can position it)
    // This might be easier to express with an hypothetical SetNextWindowPosConstraints() function.
    if (ImGuiWindow* popup_window = ImGui::FindWindowByName(name))
        if (popup_window->WasActive)
        {
            // Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
            ImVec2 size_expected = ImGui::CalcWindowNextAutoFitSize(popup_window);
            if (flags & ImGuiComboFlags_PopupAlignLeft)
                popup_window->AutoPosLastDirection = ImGuiDir_Left; // "Below, Toward Left"
            else
                popup_window->AutoPosLastDirection = ImGuiDir_Down; // "Below, Toward Right (default)"
            ImRect r_outer = ImGui::GetWindowAllowedExtentRect(popup_window);
            ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
            ImGui::SetNextWindowPos(pos + ImVec2(0, style.ItemInnerSpacing.y));
        }

    // We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

    // Horizontally align ourselves with the framed text
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, StyleProvider::Padding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImGui::GetColorU32(ImGuiCol_FrameBgActive));
    bool ret = ImGui::Begin(name, NULL, window_flags);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    if (!ret)
    {
        ImGui::EndPopup();
        IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
        return false;
    }
    return true;
}

bool Widgets::Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items)
{
    ImGuiContext& g = *GImGui;

    // Call the getter to obtain the preview string which is a parameter to BeginCombo()
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        items_getter(data, *current_item, &preview_value);

    // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, calcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

    if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
        return false;

    // Display items
    // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        ImGui::PushID((void*)(intptr_t)i);
        const bool item_selected = (i == *current_item);
        const char* item_text;
        if (!items_getter(data, i, &item_text))
            item_text = "*Unknown item*";
        if (Selectable(item_text, item_selected, ImGuiSelectableFlags_SpanAllColumns))
        {
            value_changed = true;
            *current_item = i;
        }
        if (item_selected)
            ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }

    ImGui::EndCombo();
    if (value_changed)
        ImGui::MarkItemEdited(g.CurrentWindow->DC.LastItemId);

    return value_changed;
}

bool Widgets::Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items)
{
    const bool value_changed = Combo(label, current_item, items_ArrayGetter, (void*)items, items_count, popup_max_height_in_items);
    return value_changed;
}

void Widgets::Tooltip(const char* text)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(text);
    ImGui::PushFont(StyleProvider::FontSmallBold);
	ImVec2 label_size = ImGui::CalcTextSize("?", NULL, true);
    ImGui::PopFont();

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImGui::CalcItemSize(ImVec2(0.f, 0.f), label_size.y + style.FramePadding.x, label_size.y + style.FramePadding.y);

    const ImRect bb(pos + ImVec2(0, ImGui::GetFrameHeight() / 2.f - size.y / 2), pos + ImVec2(size.x, ImGui::GetFrameHeight() / 2.f + size.y / 2));
    ImGui::ItemSize(size, style.FramePadding.y / 2.f);
    if (!ImGui::ItemAdd(bb, id))
        return;

    bool hovered, held;
    const bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

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

    ImGui::RenderFrame(bb.Min, bb.Max, col, true, 100);

    if (g.LogEnabled)
        ImGui::LogSetNextTextDecoration("[", "]");

    ImGui::PushFont(StyleProvider::FontSmallBold);
    ImGui::RenderTextClipped(bb.Min + style.FramePadding / 2.f, bb.Max - style.FramePadding / 2.f, "?", NULL, &label_size, style.ButtonTextAlign, &bb);
    ImGui::PopFont();

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    ImGui::CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);

    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImGui::GetColorU32(ImGuiCol_FrameBg));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, StyleProvider::Padding);
        ImGui::BeginTooltip();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PushTextWrapPos((std::min)(ImGui::CalcTextSize(text).x, (StyleProvider::MainMenuSize.x - StyleProvider::MainMenuSideBarSize.x) * 0.75f));
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void Widgets::HitErrorBar(int window)
{
    const float hitWindowWidth = (ImGui::GetWindowSize().x * 0.5f) / 6.0f;

    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCurrentWindow()->DC.CursorPos, ImVec2(ImGui::GetCurrentWindow()->DC.CursorPos.x + ImGui::GetWindowSize().x * 0.5f, ImGui::GetCurrentWindow()->DC.CursorPos.y + ImGui::GetFrameHeight()), ImColor(218, 174, 70, 255), ImGui::GetStyle().FrameRounding);
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetCurrentWindow()->DC.CursorPos.x + hitWindowWidth, ImGui::GetCurrentWindow()->DC.CursorPos.y), ImVec2(ImGui::GetCurrentWindow()->DC.CursorPos.x + ImGui::GetWindowSize().x * 0.5f - hitWindowWidth, ImGui::GetCurrentWindow()->DC.CursorPos.y + ImGui::GetFrameHeight()), ImColor(87, 227, 19, 255));
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetCurrentWindow()->DC.CursorPos.x + hitWindowWidth * 2, ImGui::GetCurrentWindow()->DC.CursorPos.y), ImVec2(ImGui::GetCurrentWindow()->DC.CursorPos.x + ImGui::GetWindowSize().x * 0.5f - hitWindowWidth * 2, ImGui::GetCurrentWindow()->DC.CursorPos.y + ImGui::GetFrameHeight()), ImColor(50, 188, 231, 255));
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetCurrentWindow()->DC.CursorPos.x + ((ImGui::GetWindowSize().x * 0.5f) / 2.0f) - hitWindowWidth * (static_cast<float>(window) / 100.0f), ImGui::GetCurrentWindow()->DC.CursorPos.y + ImGui::GetStyle().FramePadding.y), ImVec2(ImGui::GetCurrentWindow()->DC.CursorPos.x + ((ImGui::GetWindowSize().x * 0.5f) / 2.0f) + hitWindowWidth * (static_cast<float>(window) / 100.0f), ImGui::GetCurrentWindow()->DC.CursorPos.y + ImGui::GetFrameHeight() - ImGui::GetStyle().FramePadding.y), ImColor(StyleProvider::AccentColour), ImGui::GetStyle().FrameRounding);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y);
}