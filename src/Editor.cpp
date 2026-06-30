// 截图编辑器实现

#include "Editor.h"
#include "SnipX.h"
#include "Localization.h"
#include "resource.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <shlobj.h>
#include <shlwapi.h>
#include <windowsx.h>


namespace
{
    const int TOOLBAR_X = 10;
    const int TOOLBAR_Y = 10;
    const int TOOL_BUTTON_WIDTH = 50;
    const int TOOL_BUTTON_HEIGHT = 40;
    const int TOOL_BUTTON_SPACING = 5;
    const int TOOL_BUTTON_COUNT = 10;
    const int COLOR_BOX_SIZE = 30;
    const int COLOR_BOX_OFFSET_X = TOOLBAR_X + TOOL_BUTTON_COUNT * (TOOL_BUTTON_WIDTH + TOOL_BUTTON_SPACING) + 10;
    const int COLOR_BOX_OFFSET_Y = TOOLBAR_Y + 5;
    const int STROKE_WIDTH_X = COLOR_BOX_OFFSET_X + 40;
    const int STROKE_WIDTH_Y = TOOLBAR_Y;
    const int STROKE_WIDTH_WIDTH = 90;
    const int STROKE_WIDTH_HEIGHT = TOOL_BUTTON_HEIGHT;
    const int COLOR_PANEL_X = COLOR_BOX_OFFSET_X;
    const int COLOR_PANEL_Y = COLOR_BOX_OFFSET_Y + COLOR_BOX_SIZE + 8;
    const int COLOR_PANEL_COLUMNS = 4;
    const int COLOR_PANEL_COLOR_SIZE = 22;
    const int COLOR_PANEL_SPACING = 6;
    const int COLOR_PANEL_COUNT = 8;
    const int STROKE_SLIDER_PANEL_X = STROKE_WIDTH_X;
    const int STROKE_SLIDER_PANEL_Y = STROKE_WIDTH_Y + STROKE_WIDTH_HEIGHT + 8;
    const int STROKE_SLIDER_PANEL_WIDTH = 140;
    const int STROKE_SLIDER_PANEL_HEIGHT = 54;
    const int STROKE_SLIDER_MIN_WIDTH = 1;
    const int STROKE_SLIDER_MAX_WIDTH = 12;
    const int STROKE_SLIDER_TRACK_X = STROKE_SLIDER_PANEL_X + 14;
    const int STROKE_SLIDER_TRACK_Y = STROKE_SLIDER_PANEL_Y + 30;
    const int STROKE_SLIDER_TRACK_WIDTH = STROKE_SLIDER_PANEL_WIDTH - 28;
    const int RESIZE_HANDLE_SIZE = 8;
    const int RESIZE_HANDLE_HALF = RESIZE_HANDLE_SIZE / 2;
    const int RESIZE_HIT_MARGIN = 6;

    struct ResizeHandleInfo
    {
        Rect rect;
        int index;
    };

    struct ToolButtonInfo
    {
        AnnotationType type;
        const WCHAR* name;
        const WCHAR* tooltip;
    };

    const ToolButtonInfo TOOL_BUTTONS[TOOL_BUTTON_COUNT] = {
        { ANNO_RECTANGLE, L"矩形", L"矩形工具" },
        { ANNO_ELLIPSE, L"椭圆", L"椭圆工具" },
        { ANNO_ARROW, L"箭头", L"箭头工具" },
        { ANNO_PENCIL, L"铅笔", L"铅笔工具" },
        { ANNO_TEXT, L"文字", L"文字工具" },
        { ANNO_LINE, L"直线", L"直线工具" },
        { ANNO_NUMBER, L"序号", L"序号标签工具" },
        { ANNO_HIGHLIGHT, L"高亮", L"半透明高亮工具" },
        { ANNO_MOSAIC, L"马赛克", L"马赛克工具" },
        { ANNO_BLUR, L"模糊", L"模糊工具" }
    };

    AnnotationType GetToolFromShortcut(WPARAM key)
    {
        switch (key)
        {
        case '1': return ANNO_RECTANGLE;
        case '2': return ANNO_ELLIPSE;
        case '3': return ANNO_ARROW;
        case '4': return ANNO_LINE;
        case '5': return ANNO_PENCIL;
        case '6': return ANNO_TEXT;
        case '7': return ANNO_NUMBER;
        case '8': return ANNO_HIGHLIGHT;
        case '9': return ANNO_MOSAIC;
        case '0': return ANNO_BLUR;
        default: return ANNO_NONE;
        }
    }

    const Color PRESET_COLORS[COLOR_PANEL_COUNT] = {
        Color(255, 255, 0, 0),
        Color(255, 255, 128, 0),
        Color(255, 255, 255, 0),
        Color(255, 0, 180, 0),
        Color(255, 0, 120, 215),
        Color(255, 128, 0, 255),
        Color(255, 255, 255, 255),
        Color(255, 0, 0, 0)
    };

    Rect GetColorPanelBounds()
    {
        int rows = (COLOR_PANEL_COUNT + COLOR_PANEL_COLUMNS - 1) / COLOR_PANEL_COLUMNS;
        int width = COLOR_PANEL_COLUMNS * COLOR_PANEL_COLOR_SIZE + (COLOR_PANEL_COLUMNS + 1) * COLOR_PANEL_SPACING;
        int height = rows * COLOR_PANEL_COLOR_SIZE + (rows + 1) * COLOR_PANEL_SPACING;
        return Rect(COLOR_PANEL_X, COLOR_PANEL_Y, width, height);
    }

    Rect GetPresetColorRect(int index)
    {
        int col = index % COLOR_PANEL_COLUMNS;
        int row = index / COLOR_PANEL_COLUMNS;
        return Rect(COLOR_PANEL_X + COLOR_PANEL_SPACING + col * (COLOR_PANEL_COLOR_SIZE + COLOR_PANEL_SPACING),
                    COLOR_PANEL_Y + COLOR_PANEL_SPACING + row * (COLOR_PANEL_COLOR_SIZE + COLOR_PANEL_SPACING),
                    COLOR_PANEL_COLOR_SIZE,
                    COLOR_PANEL_COLOR_SIZE);
    }

    Rect GetStrokeSliderPanelBounds()
    {
        return Rect(STROKE_SLIDER_PANEL_X, STROKE_SLIDER_PANEL_Y, STROKE_SLIDER_PANEL_WIDTH, STROKE_SLIDER_PANEL_HEIGHT);
    }

    int GetStrokeWidthFromSliderX(int x)
    {
        int clampedX = (std::max)(STROKE_SLIDER_TRACK_X, (std::min)(STROKE_SLIDER_TRACK_X + STROKE_SLIDER_TRACK_WIDTH, x));
        double ratio = (clampedX - STROKE_SLIDER_TRACK_X) / (double)STROKE_SLIDER_TRACK_WIDTH;
        int width = STROKE_SLIDER_MIN_WIDTH + (int)round(ratio * (STROKE_SLIDER_MAX_WIDTH - STROKE_SLIDER_MIN_WIDTH));
        return (std::max)(STROKE_SLIDER_MIN_WIDTH, (std::min)(STROKE_SLIDER_MAX_WIDTH, width));
    }

    int GetSliderXFromStrokeWidth(int width)
    {
        int clampedWidth = (std::max)(STROKE_SLIDER_MIN_WIDTH, (std::min)(STROKE_SLIDER_MAX_WIDTH, width));
        double ratio = (clampedWidth - STROKE_SLIDER_MIN_WIDTH) / (double)(STROKE_SLIDER_MAX_WIDTH - STROKE_SLIDER_MIN_WIDTH);
        return STROKE_SLIDER_TRACK_X + (int)round(ratio * STROKE_SLIDER_TRACK_WIDTH);
    }

    void DrawPopupPanel(Graphics* graphics, const Rect& bounds)
    {
        SolidBrush panelBrush(Color(245, 255, 255, 255));
        Pen borderPen(Color(255, 0, 120, 215), 1);
        graphics->FillRectangle(&panelBrush, bounds);
        graphics->DrawRectangle(&borderPen, bounds);
    }

    void DrawColorPanel(Graphics* graphics, const Color& currentColor)
    {
        Rect panelBounds = GetColorPanelBounds();
        DrawPopupPanel(graphics, panelBounds);
        Pen normalPen(Color(255, 180, 180, 180), 1);
        Pen selectedPen(Color(255, 0, 120, 215), 2);

        for (int i = 0; i < COLOR_PANEL_COUNT; i++)
        {
            Rect colorRect = GetPresetColorRect(i);
            SolidBrush colorBrush(PRESET_COLORS[i]);
            graphics->FillRectangle(&colorBrush, colorRect);
            bool selected = PRESET_COLORS[i].GetR() == currentColor.GetR() &&
                            PRESET_COLORS[i].GetG() == currentColor.GetG() &&
                            PRESET_COLORS[i].GetB() == currentColor.GetB();
            graphics->DrawRectangle(selected ? &selectedPen : &normalPen, colorRect);
        }
    }

    void DrawStrokeSlider(Graphics* graphics, int currentWidth, const Color& currentColor)
    {
        Rect panelBounds = GetStrokeSliderPanelBounds();
        DrawPopupPanel(graphics, panelBounds);
        Font font(L"Microsoft YaHei", 9);
        SolidBrush textBrush(Color(255, 35, 35, 35));
        WCHAR widthText[32];
        swprintf_s(widthText, L"粗细: %d px", currentWidth);
        graphics->DrawString(widthText, -1, &font, PointF((REAL)panelBounds.X + 12.0f, (REAL)panelBounds.Y + 8.0f), &textBrush);

        Pen trackPen(Color(255, 180, 180, 180), 3);
        trackPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
        graphics->DrawLine(&trackPen, STROKE_SLIDER_TRACK_X, STROKE_SLIDER_TRACK_Y,
                           STROKE_SLIDER_TRACK_X + STROKE_SLIDER_TRACK_WIDTH, STROKE_SLIDER_TRACK_Y);
        Pen previewPen(currentColor, (REAL)currentWidth);
        previewPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
        graphics->DrawLine(&previewPen, panelBounds.X + 86, panelBounds.Y + 18, panelBounds.X + 122, panelBounds.Y + 18);

        int thumbX = GetSliderXFromStrokeWidth(currentWidth);
        SolidBrush thumbBrush(Color(255, 0, 120, 215));
        graphics->FillEllipse(&thumbBrush, thumbX - 5, STROKE_SLIDER_TRACK_Y - 5, 10, 10);
    }



    Rect GetToolButtonRect(int index)
    {
        return Rect(TOOLBAR_X + index * (TOOL_BUTTON_WIDTH + TOOL_BUTTON_SPACING),
                    TOOLBAR_Y,
                    TOOL_BUTTON_WIDTH,
                    TOOL_BUTTON_HEIGHT);
    }

    Rect GetColorPickerRect()
    {
        return Rect(COLOR_BOX_OFFSET_X, COLOR_BOX_OFFSET_Y, COLOR_BOX_SIZE, COLOR_BOX_SIZE);
    }

    Rect GetStrokeWidthRect()
    {
        return Rect(STROKE_WIDTH_X, STROKE_WIDTH_Y, STROKE_WIDTH_WIDTH, STROKE_WIDTH_HEIGHT);
    }

    void DrawToolIcon(Graphics* graphics, AnnotationType tool, const Rect& rect, const Color& color)
    {
        Pen pen(color, 2.0f);
        SolidBrush brush(color);
        int cx = rect.X + rect.Width / 2;
        int cy = rect.Y + 15;
        Rect iconRect(rect.X + 15, rect.Y + 7, 20, 16);

        switch (tool)
        {
        case ANNO_RECTANGLE:
            graphics->DrawRectangle(&pen, iconRect);
            break;
        case ANNO_ELLIPSE:
            graphics->DrawEllipse(&pen, iconRect);
            break;
        case ANNO_ARROW:
            pen.SetEndCap(LineCapArrowAnchor);
            graphics->DrawLine(&pen, rect.X + 14, rect.Y + 22, rect.X + 35, rect.Y + 9);
            break;
        case ANNO_PENCIL:
            pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
            graphics->DrawLine(&pen, rect.X + 14, rect.Y + 22, rect.X + 20, rect.Y + 12);
            graphics->DrawLine(&pen, rect.X + 20, rect.Y + 12, rect.X + 28, rect.Y + 18);
            graphics->DrawLine(&pen, rect.X + 28, rect.Y + 18, rect.X + 36, rect.Y + 9);
            break;
        case ANNO_TEXT:
        {
            Font iconFont(L"Microsoft YaHei", 14, FontStyleBold);
            StringFormat format;
            format.SetAlignment(StringAlignmentCenter);
            format.SetLineAlignment(StringAlignmentCenter);
            RectF textRect((REAL)rect.X, (REAL)rect.Y + 3, (REAL)rect.Width, 24.0f);
            graphics->DrawString(L"T", -1, &iconFont, textRect, &format, &brush);
            break;
        }
        case ANNO_LINE:
            graphics->DrawLine(&pen, rect.X + 14, rect.Y + 22, rect.X + 36, rect.Y + 9);
            break;
        case ANNO_NUMBER:
        {
            Font iconFont(L"Microsoft YaHei", 12, FontStyleBold);
            StringFormat format;
            format.SetAlignment(StringAlignmentCenter);
            format.SetLineAlignment(StringAlignmentCenter);
            Rect circleRect(rect.X + 15, rect.Y + 6, 20, 20);
            graphics->DrawEllipse(&pen, circleRect);
            RectF textRect((REAL)circleRect.X, (REAL)circleRect.Y, (REAL)circleRect.Width, (REAL)circleRect.Height);
            graphics->DrawString(L"1", -1, &iconFont, textRect, &format, &brush);
            break;
        }
        case ANNO_HIGHLIGHT:
        {
            SolidBrush highlightBrush(Color(120, 255, 230, 0));
            graphics->FillRectangle(&highlightBrush, rect.X + 13, rect.Y + 10, 24, 12);
            graphics->DrawLine(&pen, rect.X + 13, rect.Y + 24, rect.X + 37, rect.Y + 24);
            break;
        }
        case ANNO_MOSAIC:
            for (int row = 0; row < 2; row++)
            {
                for (int col = 0; col < 3; col++)
                {
                    int shade = 90 + (row * 3 + col) * 20;
                    SolidBrush blockBrush(Color(255, shade, shade, shade));
                    graphics->FillRectangle(&blockBrush, rect.X + 14 + col * 7, rect.Y + 8 + row * 8, 6, 7);
                }
            }
            break;
        case ANNO_BLUR:
            graphics->FillEllipse(&brush, cx - 8, cy - 8, 16, 16);
            pen.SetColor(Color(120, color.GetR(), color.GetG(), color.GetB()));
            graphics->DrawEllipse(&pen, cx - 11, cy - 11, 22, 22);
            break;
        default:
            break;
        }
    }

    void DrawArrowHead(Graphics* graphics, const Pen& outlinePen, const SolidBrush& fillBrush, Point start, Point end, bool filled)
    {
        double angle = atan2((double)(end.Y - start.Y), (double)(end.X - start.X));
        double headLength = 14.0;
        double headAngle = 0.55;
        PointF head[3] = {
            PointF((REAL)end.X, (REAL)end.Y),
            PointF((REAL)(end.X - headLength * cos(angle - headAngle)), (REAL)(end.Y - headLength * sin(angle - headAngle))),
            PointF((REAL)(end.X - headLength * cos(angle + headAngle)), (REAL)(end.Y - headLength * sin(angle + headAngle)))
        };

        if (filled)
        {
            graphics->FillPolygon(&fillBrush, head, 3);
            graphics->DrawPolygon(&outlinePen, head, 3);
        }
        else
        {
            graphics->DrawLine(&outlinePen, head[0], head[1]);
            graphics->DrawLine(&outlinePen, head[0], head[2]);
        }
    }

    void DrawStyledArrow(Graphics* graphics, Point start, Point end, const Color& color, int strokeWidth, ArrowStyle style)
    {
        Pen pen(color, (REAL)strokeWidth);
        pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
        SolidBrush brush(color);

        if (style == ARROW_STYLE_NORMAL)
        {
            pen.SetEndCap(LineCapArrowAnchor);
            graphics->DrawLine(&pen, start, end);
            return;
        }

        if (style == ARROW_STYLE_DOUBLE_LINE)
        {
            Pen secondPen(color, (REAL)(std::max)(1, strokeWidth - 1));
            secondPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
            double dx = end.X - start.X;
            double dy = end.Y - start.Y;
            double len = sqrt(dx * dx + dy * dy);
            double ox = len > 0.0 ? -dy / len * 3.0 : 0.0;
            double oy = len > 0.0 ? dx / len * 3.0 : 0.0;
            Point s1(start.X + (int)round(ox), start.Y + (int)round(oy));
            Point e1(end.X + (int)round(ox), end.Y + (int)round(oy));
            Point s2(start.X - (int)round(ox), start.Y - (int)round(oy));
            Point e2(end.X - (int)round(ox), end.Y - (int)round(oy));
            graphics->DrawLine(&secondPen, s1, e1);
            graphics->DrawLine(&secondPen, s2, e2);
            DrawArrowHead(graphics, pen, brush, start, end, false);
            return;
        }

        graphics->DrawLine(&pen, start, end);
        DrawArrowHead(graphics, pen, brush, start, end, style == ARROW_STYLE_FILLED);
    }


    Rect NormalizeRect(const Rect& rect)
    {
        int x = rect.X;
        int y = rect.Y;
        int w = rect.Width;
        int h = rect.Height;

        if (w < 0)
        {
            x += w;
            w = -w;
        }
        if (h < 0)
        {
            y += h;
            h = -h;
        }

        return Rect(x, y, w, h);
    }

    Rect UnionRects(const Rect& first, const Rect& second)
    {
        Rect a = NormalizeRect(first);
        Rect b = NormalizeRect(second);
        int left = (std::min)(a.X, b.X);
        int top = (std::min)(a.Y, b.Y);
        int right = (std::max)(a.X + a.Width, b.X + b.Width);
        int bottom = (std::max)(a.Y + a.Height, b.Y + b.Height);
        return Rect(left, top, right - left, bottom - top);
    }

    Rect InflateRectBounds(const Rect& rect, int margin)
    {
        Rect normalized = NormalizeRect(rect);
        return Rect(normalized.X - margin,
                    normalized.Y - margin,
                    normalized.Width + margin * 2,
                    normalized.Height + margin * 2);
    }

    Rect ClipRectToBitmap(const Rect& rect, Bitmap* bitmap)
    {
        if (!bitmap)
            return Rect(0, 0, 0, 0);

        Rect normalized = NormalizeRect(rect);
        int left = (std::max)(0, normalized.X);
        int top = (std::max)(0, normalized.Y);
        int right = (std::min)(normalized.X + normalized.Width, (int)bitmap->GetWidth());
        int bottom = (std::min)(normalized.Y + normalized.Height, (int)bitmap->GetHeight());
        if (right <= left || bottom <= top)
            return Rect(0, 0, 0, 0);

        return Rect(left, top, right - left, bottom - top);
    }

    void DrawSelectionFrame(Graphics* graphics, const Rect& rect)
    {
        Pen borderPen(Color(255, 0, 120, 215), 2);
        graphics->DrawRectangle(&borderPen, rect);
    }

    void DrawResizeHandles(Graphics* graphics, const Rect& rect)
    {
        Rect normalized = NormalizeRect(rect);
        SolidBrush handleBrush(Color(255, 255, 255, 255));
        Pen handleBorder(Color(255, 0, 120, 215), 1);

        Point centers[8] = {
            Point(normalized.X, normalized.Y),
            Point(normalized.X + normalized.Width / 2, normalized.Y),
            Point(normalized.X + normalized.Width, normalized.Y),
            Point(normalized.X + normalized.Width, normalized.Y + normalized.Height / 2),
            Point(normalized.X + normalized.Width, normalized.Y + normalized.Height),
            Point(normalized.X + normalized.Width / 2, normalized.Y + normalized.Height),
            Point(normalized.X, normalized.Y + normalized.Height),
            Point(normalized.X, normalized.Y + normalized.Height / 2)
        };

        for (const auto& center : centers)
        {
            Rect handle(center.X - RESIZE_HANDLE_HALF, center.Y - RESIZE_HANDLE_HALF, RESIZE_HANDLE_SIZE, RESIZE_HANDLE_SIZE);
            graphics->FillRectangle(&handleBrush, handle);
            graphics->DrawRectangle(&handleBorder, handle);
        }
    }

    bool HitResizeHandle(const Rect& rect, Point pt, int& handleIndex)
    {
        Rect normalized = NormalizeRect(rect);
        ResizeHandleInfo handles[8] = {
            { Rect(normalized.X - RESIZE_HIT_MARGIN, normalized.Y - RESIZE_HIT_MARGIN, RESIZE_HIT_MARGIN * 2, RESIZE_HIT_MARGIN * 2), 0 },
            { Rect(normalized.X + normalized.Width / 2 - RESIZE_HIT_MARGIN, normalized.Y - RESIZE_HIT_MARGIN, RESIZE_HIT_MARGIN * 2, RESIZE_HIT_MARGIN * 2), 1 },
            { Rect(normalized.X + normalized.Width - RESIZE_HIT_MARGIN, normalized.Y - RESIZE_HIT_MARGIN, RESIZE_HIT_MARGIN * 2, RESIZE_HIT_MARGIN * 2), 2 },
            { Rect(normalized.X + normalized.Width - RESIZE_HIT_MARGIN, normalized.Y + normalized.Height / 2 - RESIZE_HIT_MARGIN, RESIZE_HIT_MARGIN * 2, RESIZE_HIT_MARGIN * 2), 3 },
            { Rect(normalized.X + normalized.Width - RESIZE_HIT_MARGIN, normalized.Y + normalized.Height - RESIZE_HIT_MARGIN, RESIZE_HIT_MARGIN * 2, RESIZE_HIT_MARGIN * 2), 4 },
            { Rect(normalized.X + normalized.Width / 2 - RESIZE_HIT_MARGIN, normalized.Y + normalized.Height - RESIZE_HIT_MARGIN, RESIZE_HIT_MARGIN * 2, RESIZE_HIT_MARGIN * 2), 5 },
            { Rect(normalized.X - RESIZE_HIT_MARGIN, normalized.Y + normalized.Height - RESIZE_HIT_MARGIN, RESIZE_HIT_MARGIN * 2, RESIZE_HIT_MARGIN * 2), 6 },
            { Rect(normalized.X - RESIZE_HIT_MARGIN, normalized.Y + normalized.Height / 2 - RESIZE_HIT_MARGIN, RESIZE_HIT_MARGIN * 2, RESIZE_HIT_MARGIN * 2), 7 }
        };

        for (const auto& handle : handles)
        {
            if (handle.rect.Contains(pt))
            {
                handleIndex = handle.index;
                return true;
            }
        }

        return false;
    }

    void ApplyResizeHandle(Rect& rect, int handleIndex, Point current)
    {
        int left = rect.X;
        int top = rect.Y;
        int right = rect.X + rect.Width;
        int bottom = rect.Y + rect.Height;

        switch (handleIndex)
        {
        case 0:
            left = current.X;
            top = current.Y;
            break;
        case 1:
            top = current.Y;
            break;
        case 2:
            right = current.X;
            top = current.Y;
            break;
        case 3:
            right = current.X;
            break;
        case 4:
            right = current.X;
            bottom = current.Y;
            break;
        case 5:
            bottom = current.Y;
            break;
        case 6:
            left = current.X;
            bottom = current.Y;
            break;
        case 7:
            left = current.X;
            break;
        }

        int minSize = 2;
        if (right - left < minSize)
        {
            if (handleIndex == 0 || handleIndex == 6 || handleIndex == 7)
                left = right - minSize;
            else
                right = left + minSize;
        }
        if (bottom - top < minSize)
        {
            if (handleIndex == 0 || handleIndex == 1 || handleIndex == 2)
                top = bottom - minSize;
            else
                bottom = top + minSize;
        }


        rect.X = (std::min)(left, right);
        rect.Y = (std::min)(top, bottom);
        rect.Width = abs(right - left);
        rect.Height = abs(bottom - top);
    }


}

// RectangleAnnotation 实现
void RectangleAnnotation::Draw(Graphics* graphics)
{
    Pen pen(color, (REAL)strokeWidth);
    graphics->DrawRectangle(&pen, rect);
    if (selected)
    {
        DrawSelectionFrame(graphics, rect);
        DrawResizeHandles(graphics, rect);
    }
}

bool RectangleAnnotation::HitTest(Point pt)
{
    return rect.Contains(pt);
}

void RectangleAnnotation::Move(int dx, int dy)
{
    rect.X += dx;
    rect.Y += dy;
}

Rect RectangleAnnotation::GetBounds() const
{
    return NormalizeRect(rect);
}

void RectangleAnnotation::ResizeToBounds(const Rect& bounds)
{
    rect = NormalizeRect(bounds);
}

// NumberAnnotation 实现
void NumberAnnotation::Draw(Graphics* graphics)
{
    Rect bounds = GetBounds();
    SolidBrush fillBrush(Color(255, 255, 208, 64));
    SolidBrush textBrush(Color(255, 60, 60, 60));
    Pen borderPen(Color(255, 255, 160, 0), 2);
    Font font(L"Microsoft YaHei", 11, FontStyleBold);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    graphics->FillEllipse(&fillBrush, bounds);
    graphics->DrawEllipse(&borderPen, bounds);

    WCHAR numberText[16];
    swprintf_s(numberText, L"%d", number);
    RectF textRect((REAL)bounds.X, (REAL)bounds.Y, (REAL)bounds.Width, (REAL)bounds.Height);
    graphics->DrawString(numberText, -1, &font, textRect, &format, &textBrush);
    if (selected)
    {
        DrawSelectionFrame(graphics, bounds);
        DrawResizeHandles(graphics, bounds);
    }
}

bool NumberAnnotation::HitTest(Point pt)
{
    return GetBounds().Contains(pt);
}

void NumberAnnotation::Move(int dx, int dy)
{
    rect.X += dx;
    rect.Y += dy;
}

Rect NumberAnnotation::GetBounds() const
{
    return NormalizeRect(rect);
}

void NumberAnnotation::ResizeToBounds(const Rect& bounds)
{
    rect = NormalizeRect(bounds);
}

// HighlightAnnotation 实现
void HighlightAnnotation::Draw(Graphics* graphics)
{
    Rect bounds = GetBounds();
    SolidBrush brush(Color(120, 255, 255, 0));
    Pen pen(Color(180, 255, 215, 0), 2);
    graphics->FillRectangle(&brush, bounds);
    graphics->DrawRectangle(&pen, bounds);
    if (selected)
    {
        DrawSelectionFrame(graphics, bounds);
        DrawResizeHandles(graphics, bounds);
    }
}

bool HighlightAnnotation::HitTest(Point pt)
{
    return GetBounds().Contains(pt);
}

void HighlightAnnotation::Move(int dx, int dy)
{
    rect.X += dx;
    rect.Y += dy;
}

Rect HighlightAnnotation::GetBounds() const
{
    return NormalizeRect(rect);
}

void HighlightAnnotation::ResizeToBounds(const Rect& bounds)
{
    rect = NormalizeRect(bounds);
}





// EllipseAnnotation 实现
void EllipseAnnotation::Draw(Graphics* graphics)
{
    Pen pen(color, (REAL)strokeWidth);
    graphics->DrawEllipse(&pen, rect);
    if (selected)
    {
        DrawSelectionFrame(graphics, rect);
        DrawResizeHandles(graphics, rect);
    }
}

bool EllipseAnnotation::HitTest(Point pt)
{
    return rect.Contains(pt);
}

void EllipseAnnotation::Move(int dx, int dy)
{
    rect.X += dx;
    rect.Y += dy;
}

Rect EllipseAnnotation::GetBounds() const
{
    return NormalizeRect(rect);
}

void EllipseAnnotation::ResizeToBounds(const Rect& bounds)
{
    rect = NormalizeRect(bounds);
}

// ArrowAnnotation 实现
void ArrowAnnotation::Draw(Graphics* graphics)
{
    if (type == ANNO_ARROW)
    {
        DrawStyledArrow(graphics, start, end, color, strokeWidth, arrowStyle);
    }
    else
    {
        Pen pen(color, (REAL)strokeWidth);
        graphics->DrawLine(&pen, start, end);
    }

    if (selected)
    {
        Rect bounds = GetBounds();
        DrawSelectionFrame(graphics, bounds);
        DrawResizeHandles(graphics, bounds);
    }
}

bool ArrowAnnotation::HitTest(Point pt)
{
    // 计算点到线段的距离
    float dx = (float)(end.X - start.X);
    float dy = (float)(end.Y - start.Y);
    float len = sqrt(dx * dx + dy * dy);
    
    if (len < 1.0f)
        return false;
    
    float t = ((pt.X - start.X) * dx + (pt.Y - start.Y) * dy) / (len * len);
    t = (std::max)(0.0f, (std::min)(1.0f, t));
    
    float nearX = start.X + t * dx;
    float nearY = start.Y + t * dy;
    
    float dist = sqrt((pt.X - nearX) * (pt.X - nearX) + (pt.Y - nearY) * (pt.Y - nearY));
    
    return dist < strokeWidth + 5;
}

void ArrowAnnotation::Move(int dx, int dy)
{
    start.X += dx;
    start.Y += dy;
    end.X += dx;
    end.Y += dy;
}

Rect ArrowAnnotation::GetBounds() const
{
    int left = (std::min)(start.X, end.X);
    int top = (std::min)(start.Y, end.Y);
    int width = abs(end.X - start.X);
    int height = abs(end.Y - start.Y);
    if (width == 0) width = 1;
    if (height == 0) height = 1;
    return Rect(left, top, width, height);
}

void ArrowAnnotation::ResizeToBounds(const Rect& bounds)
{
    Rect normalized = NormalizeRect(bounds);
    start = Point(normalized.X, normalized.Y);
    end = Point(normalized.X + normalized.Width, normalized.Y + normalized.Height);
}

// PencilAnnotation 实现
void PencilAnnotation::Draw(Graphics* graphics)
{
    if (points.size() < 2)
        return;
    
    Pen pen(color, (REAL)strokeWidth);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    
    for (size_t i = 1; i < points.size(); i++)
    {
        graphics->DrawLine(&pen, points[i - 1], points[i]);
    }

    if (selected)
    {
        DrawSelectionFrame(graphics, GetBounds());
        DrawResizeHandles(graphics, GetBounds());
    }
}

bool PencilAnnotation::HitTest(Point pt)
{
    // 检测点是否在路径附近
    for (size_t i = 1; i < points.size(); i++)
    {
        float dx = (float)(points[i].X - points[i-1].X);
        float dy = (float)(points[i].Y - points[i-1].Y);
        float len = sqrt(dx * dx + dy * dy);
        
        if (len < 1.0f)
            continue;
        
        float t = ((pt.X - points[i-1].X) * dx + (pt.Y - points[i-1].Y) * dy) / (len * len);
        t = (std::max)(0.0f, (std::min)(1.0f, t));
        
        float nearX = points[i-1].X + t * dx;
        float nearY = points[i-1].Y + t * dy;
        
        float dist = sqrt((pt.X - nearX) * (pt.X - nearX) + (pt.Y - nearY) * (pt.Y - nearY));
        
        if (dist < strokeWidth + 5)
            return true;
    }
    
    return false;
}

void PencilAnnotation::Move(int dx, int dy)
{
    for (auto& point : points)
    {
        point.X += dx;
        point.Y += dy;
    }
}

Rect PencilAnnotation::GetBounds() const
{
    if (points.empty())
        return Rect(0, 0, 0, 0);

    int left = points[0].X;
    int top = points[0].Y;
    int right = points[0].X;
    int bottom = points[0].Y;

    for (const auto& point : points)
    {
        left = (std::min)(left, point.X);
        top = (std::min)(top, point.Y);
        right = (std::max)(right, point.X);
        bottom = (std::max)(bottom, point.Y);
    }

    if (right - left == 0) right = left + 1;
    if (bottom - top == 0) bottom = top + 1;
    return Rect(left, top, right - left, bottom - top);
}

void PencilAnnotation::ResizeToBounds(const Rect& bounds)
{
    Rect oldBounds = GetBounds();
    Rect normalized = NormalizeRect(bounds);
    if (oldBounds.Width <= 0 || oldBounds.Height <= 0)
        return;

    double scaleX = normalized.Width / (double)oldBounds.Width;
    double scaleY = normalized.Height / (double)oldBounds.Height;
    for (auto& point : points)
    {
        point.X = normalized.X + (int)((point.X - oldBounds.X) * scaleX);
        point.Y = normalized.Y + (int)((point.Y - oldBounds.Y) * scaleY);
    }
}

// Editor 实现
Editor::Editor(SnipXApp* pApp)
    : m_pApp(pApp)
    , m_hwnd(NULL)
    , m_pBitmap(nullptr)
    , m_currentTool(ANNO_RECTANGLE)
    , m_currentStrokeWidth(2)
    , m_nextNumberLabel(1)
    , m_currentArrowStyle(ARROW_STYLE_NORMAL)
    , m_currentAnnotation(nullptr)
    , m_selectedAnnotation(nullptr)
    , m_drawing(false)
    , m_movingAnnotation(false)
    , m_resizingAnnotation(false)
    , m_hoverTool(ANNO_NONE)
    , m_hoverBottomAction(-1)
    , m_hoverColorPicker(false)
    , m_hoverStrokeWidth(false)
    , m_showColorPanel(false)
    , m_showStrokeSlider(false)
    , m_resizeHandle(-1)
    , m_toolbarHeight(60)
    , m_bottomBarHeight(50)
    , m_imageX(10)
    , m_imageY(70)
{
    m_currentColor = Color(255, 255, 0, 0);
    m_startPoint = Point(0, 0);
    m_lastPoint = Point(0, 0);
}


Editor::~Editor()
{
    Close();
}

void Editor::Open(Bitmap* pBitmap)
{
    if (!pBitmap)
        return;
    
    m_pBitmap = pBitmap;
    CreateEditorWindow();
}

void Editor::Close()
{
    DestroyEditorWindow();
    
    // 清理标注
    for (auto anno : m_annotations)
    {
        delete anno;
    }
    m_annotations.clear();
    
    for (auto anno : m_undoStack)
    {
        delete anno;
    }
    m_undoStack.clear();
    
    if (m_pBitmap)
    {
        delete m_pBitmap;
        m_pBitmap = nullptr;
    }
}

void Editor::CreateEditorWindow()
{
    if (!m_pBitmap)
        return;
    
    // 注册窗口类
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = EditorWndProc;
    wc.hInstance = m_pApp->GetInstance();
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"SnipXEditorWindow";
    RegisterClassExW(&wc);
    
    // 计算窗口大小（图片大小 + 工具栏）
    int imgWidth = m_pBitmap->GetWidth();
    int imgHeight = m_pBitmap->GetHeight();
    int toolbarHeight = 60;
    int bottomBarHeight = 50;
    
    int widthRectRight = STROKE_WIDTH_X + STROKE_WIDTH_WIDTH + 20;
    int winWidth = (std::max)(imgWidth + 20, widthRectRight);
    int winHeight = imgHeight + toolbarHeight + bottomBarHeight + 20;
    
    // 居中显示
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - winWidth) / 2;
    int y = (screenHeight - winHeight) / 2;
    
    m_hwnd = CreateWindowExW(WS_EX_TOPMOST,
                            L"SnipXEditorWindow", L"SnipX 编辑器",
                            WS_POPUP | WS_CAPTION | WS_SYSMENU,
                            x, y, winWidth, winHeight,
                            NULL, NULL, m_pApp->GetInstance(), this);
    
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
}

void Editor::DestroyEditorWindow()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    }
}

void Editor::InvalidateAnnotationRect(const Rect& bounds)
{
    if (!m_hwnd)
        return;

    Rect padded = InflateRectBounds(bounds, 16);
    RECT rc = { padded.X, padded.Y, padded.X + padded.Width, padded.Y + padded.Height };
    InvalidateRect(m_hwnd, &rc, FALSE);
}

void Editor::InvalidateAnnotationTransition(const Rect& oldBounds, const Rect& newBounds)
{
    InvalidateAnnotationRect(UnionRects(oldBounds, newBounds));
}

void Editor::DrawEditor(HDC hdc)
{
    if (!m_pBitmap)
        return;
    
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    
    // 绘制背景
    SolidBrush bgBrush(Color(255, 240, 240, 240));
    graphics.FillRectangle(&bgBrush, 0, 0, rc.right, rc.bottom);
    
    // 绘制工具栏
    DrawToolbar(&graphics);
    
    // 绘制图片
    graphics.DrawImage(m_pBitmap, m_imageX, m_imageY);
    
    // 绘制已完成的标注
    for (auto anno : m_annotations)
    {
        anno->Draw(&graphics);
    }
    
    // 绘制当前正在绘制的标注
    if (m_currentAnnotation && m_drawing)
    {
        m_currentAnnotation->Draw(&graphics);
    }
    
    // 绘制底部操作栏
    DrawBottomBar(&graphics);

    // 绘制工具栏弹出面板，确保浮层不会被截图图片或标注覆盖。
    if (m_showColorPanel)
    {
        DrawColorPanel(&graphics, m_currentColor);
    }
    if (m_showStrokeSlider)
    {
        DrawStrokeSlider(&graphics, m_currentStrokeWidth, m_currentColor);
    }

    // 绘制悬停提示，确保不会被图片或底部栏覆盖。
    DrawHoverTooltip(&graphics);
}

void Editor::DrawToolbar(Graphics* graphics)
{
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    
    // 工具栏背景
    SolidBrush toolbarBrush(Color(255, 248, 250, 252));
    graphics->FillRectangle(&toolbarBrush, 0, 0, rc.right, m_toolbarHeight);
    Pen bottomLinePen(Color(255, 226, 232, 240), 1);
    graphics->DrawLine(&bottomLinePen, 0, m_toolbarHeight - 1, rc.right, m_toolbarHeight - 1);

    Font font(L"Microsoft YaHei UI", 8);
    SolidBrush textBrush(Color(255, 17, 24, 39));
    SolidBrush selectedBrush(Color(255, 219, 234, 254));
    SolidBrush hoverBrush(Color(255, 234, 242, 255));
    SolidBrush normalBrush(Color(255, 255, 255, 255));
    Pen borderPen(Color(255, 226, 232, 240), 1);
    Pen activeBorderPen(Color(255, 59, 130, 246), 2);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    
    for (int i = 0; i < TOOL_BUTTON_COUNT; i++)
    {
        Rect btnRect = GetToolButtonRect(i);
        bool selected = TOOL_BUTTONS[i].type == m_currentTool;
        bool hovered = TOOL_BUTTONS[i].type == m_hoverTool;
        SolidBrush* bgBrush = selected ? &selectedBrush : (hovered ? &hoverBrush : &normalBrush);
        graphics->FillRectangle(bgBrush, btnRect);
        graphics->DrawRectangle(selected ? &activeBorderPen : &borderPen, btnRect);
        DrawToolIcon(graphics, TOOL_BUTTONS[i].type, btnRect, Color(255, 45, 45, 45));
        RectF textRect((REAL)btnRect.X, (REAL)btnRect.Y + 24, (REAL)btnRect.Width, 14.0f);
        graphics->DrawString(TOOL_BUTTONS[i].name, -1, &font, textRect, &format, &textBrush);
    }
    
    // 绘制颜色选择器
    Rect colorRect = GetColorPickerRect();
    SolidBrush colorBgBrush(m_hoverColorPicker ? Color(255, 236, 244, 255) : Color(255, 255, 255, 255));
    Rect colorOuter(colorRect.X - 4, colorRect.Y - 4, colorRect.Width + 8, colorRect.Height + 8);
    graphics->FillRectangle(&colorBgBrush, colorOuter);
    graphics->DrawRectangle(m_hoverColorPicker ? &activeBorderPen : &borderPen, colorOuter);
    SolidBrush colorBrush(m_currentColor);
    graphics->FillRectangle(&colorBrush, colorRect);
    graphics->DrawRectangle(&borderPen, colorRect);
    
    // 绘制粗细选择
    Rect widthRect = GetStrokeWidthRect();
    SolidBrush widthBgBrush(m_hoverStrokeWidth ? Color(255, 236, 244, 255) : Color(255, 255, 255, 255));
    graphics->FillRectangle(&widthBgBrush, widthRect);
    graphics->DrawRectangle(m_hoverStrokeWidth ? &activeBorderPen : &borderPen, widthRect);
    Pen previewPen(m_currentColor, (REAL)m_currentStrokeWidth);
    previewPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    graphics->DrawLine(&previewPen, widthRect.X + 12, widthRect.Y + 14, widthRect.X + widthRect.Width - 12, widthRect.Y + 14);
    WCHAR widthText[32];
    swprintf_s(widthText, L"粗细 %d", m_currentStrokeWidth);
    RectF widthTextRect((REAL)widthRect.X, (REAL)widthRect.Y + 20, (REAL)widthRect.Width, 18.0f);
    graphics->DrawString(widthText, -1, &font, widthTextRect, &format, &textBrush);
}


void Editor::DrawBottomBar(Graphics* graphics)
{
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    
    int bottomY = rc.bottom - m_bottomBarHeight;
    
    // 底部栏背景
    SolidBrush bottomBrush(Color(255, 248, 250, 252));
    graphics->FillRectangle(&bottomBrush, 0, bottomY, rc.right, m_bottomBarHeight);
    Pen topLinePen(Color(255, 226, 232, 240), 1);
    graphics->DrawLine(&topLinePen, 0, bottomY, rc.right, bottomY);

    // 绘制按钮
    Font font(L"Microsoft YaHei UI", 10);
    SolidBrush textBrush(Color(255, 17, 24, 39));
    SolidBrush normalBrush(Color(255, 255, 255, 255));
    SolidBrush hoverBrush(Color(255, 234, 242, 255));
    Pen borderPen(Color(255, 226, 232, 240), 1);
    Pen hoverBorderPen(Color(255, 59, 130, 246), 2);
    
    int x = 10;
    int y = bottomY + 10;
    int btnWidth = 80;
    int btnHeight = 30;
    int spacing = 10;
    
    const WCHAR* buttons[] = {
        L10n(L"保存(S)", L"Save(S)"),
        L10n(L"复制(C)", L"Copy(C)"),
        L10n(L"贴图", L"Pin"),
        L10n(L"OCR", L"OCR"),
        L10n(L"上传", L"Upload"),
        L10n(L"关闭(Esc)", L"Close(Esc)")
    };
    
    for (int i = 0; i < 6; i++)
    {
        Rect btnRect(x, y, btnWidth, btnHeight);
        bool hovered = m_hoverBottomAction == i;
        graphics->FillRectangle(hovered ? &hoverBrush : &normalBrush, btnRect);
        graphics->DrawRectangle(hovered ? &hoverBorderPen : &borderPen, btnRect);
        
        RectF textRect((REAL)x, (REAL)y, (REAL)btnWidth, (REAL)btnHeight);
        StringFormat format;
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentCenter);
        graphics->DrawString(buttons[i], -1, &font, textRect, &format, &textBrush);
        
        x += btnWidth + spacing;
    }
}


void Editor::SaveImage()
{
    Config* cfg = m_pApp->GetConfig();
    
    // 如果启用自动保存
    if (cfg->IsAutoSave())
    {
        std::wstring autoSavePath = cfg->GetEffectiveAutoSavePath();
        if (autoSavePath.empty())
        {
            MessageBoxW(m_hwnd, L10n(L"自动保存路径为空，请先在设置中选择保存目录。", L"Auto-save path is empty. Please choose a save folder in settings first."), L"SnipX", MB_ICONERROR);
            return;
        }

        if (!PathFileExistsW(autoSavePath.c_str()))
        {
            SHCreateDirectoryExW(m_hwnd, autoSavePath.c_str(), NULL);
        }

        if (!PathFileExistsW(autoSavePath.c_str()))
        {
            MessageBoxW(m_hwnd, L10n(L"自动保存目录不存在或无法创建。", L"Auto-save folder does not exist or cannot be created."), L"SnipX", MB_ICONERROR);
            return;
        }

        // 生成文件名
        SYSTEMTIME st;
        GetLocalTime(&st);

        WCHAR filename[MAX_PATH];
        swprintf_s(filename, L"%s\\%s_%04d%02d%02d_%02d%02d%02d.%s",
                  autoSavePath.c_str(),
                  cfg->GetFileNamePrefix().c_str(),
                  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
                  cfg->GetDefaultFormat().c_str());

        SaveImageToFile(filename, cfg->GetDefaultFormat());
        return;
    }

    
    // 创建保存对话框
    WCHAR filename[MAX_PATH] = L"";
    
    // 生成默认文件名（时间戳）
    SYSTEMTIME st;
    GetLocalTime(&st);
    swprintf_s(filename, L"%s_%04d%02d%02d_%02d%02d%02d.png",
              cfg->GetFileNamePrefix().c_str(),
              st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFilter = L"PNG 图片 (*.png)\0*.png\0JPEG 图片 (*.jpg)\0*.jpg\0BMP 图片 (*.bmp)\0*.bmp\0所有文件 (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = cfg->GetDefaultPath().c_str();
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = L"png";
    
    if (GetSaveFileNameW(&ofn))
    {
        // 确定格式
        std::wstring path = filename;
        std::wstring format = L"png";
        
        if (path.find(L".jpg") != std::wstring::npos || path.find(L".jpeg") != std::wstring::npos)
            format = L"jpeg";
        else if (path.find(L".bmp") != std::wstring::npos)
            format = L"bmp";
        
        SaveImageToFile(path, format);
    }
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;
    UINT size = 0;
    
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;
    
    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;
    
    GetImageEncoders(num, size, pImageCodecInfo);
    
    for (UINT i = 0; i < num; i++)
    {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return i;
        }
    }
    
    free(pImageCodecInfo);
    return -1;
}

void Editor::SaveImageToFile(const std::wstring& filename, const std::wstring& format)
{
    Bitmap* finalBitmap = CreateCompositedBitmap();
    if (!finalBitmap)
        return;
    
    // 获取编码器
    CLSID encoderClsid;
    Status saveStatus = GenericError;
    if (format == L"jpeg")
    {
        if (GetEncoderClsid(L"image/jpeg", &encoderClsid) >= 0)
        {
            // 设置 JPEG 质量
            EncoderParameters encoderParams;
            encoderParams.Count = 1;
            encoderParams.Parameter[0].Guid = EncoderQuality;
            encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
            encoderParams.Parameter[0].NumberOfValues = 1;
            ULONG quality = m_pApp->GetConfig()->GetJpgQuality();
            encoderParams.Parameter[0].Value = &quality;
            
            saveStatus = finalBitmap->Save(filename.c_str(), &encoderClsid, &encoderParams);
        }
    }
    else if (format == L"bmp")
    {
        if (GetEncoderClsid(L"image/bmp", &encoderClsid) >= 0)
        {
            saveStatus = finalBitmap->Save(filename.c_str(), &encoderClsid, NULL);
        }
    }
    else
    {
        if (GetEncoderClsid(L"image/png", &encoderClsid) >= 0)
        {
            saveStatus = finalBitmap->Save(filename.c_str(), &encoderClsid, NULL);
        }
    }
    
    delete finalBitmap;
    if (saveStatus != Ok)
    {
        MessageBoxW(m_hwnd, L10n(L"保存失败，请检查路径和文件格式。", L"Save failed. Please check the path and file format."), L"SnipX", MB_ICONERROR);
        return;
    }

    Config* cfg = m_pApp->GetConfig();
    cfg->AddHistoryItem(filename);
    cfg->Save();
    
    MessageBoxW(m_hwnd, L"保存成功！", L"SnipX", MB_ICONINFORMATION);
}


void Editor::CopyToClipboard()
{
    Bitmap* finalBitmap = CreateCompositedBitmap();
    if (!finalBitmap)
        return;
    
    HBITMAP hBitmap = NULL;
    finalBitmap->GetHBITMAP(Color(255, 255, 255), &hBitmap);
    delete finalBitmap;

    if (!hBitmap)
        return;
    
    if (OpenClipboard(m_hwnd))
    {
        EmptyClipboard();
        SetClipboardData(CF_BITMAP, hBitmap);
        CloseClipboard();
        
        MessageBoxW(m_hwnd, L10n(L"已复制到剪贴板！", L"Copied to clipboard!"), L"SnipX", MB_ICONINFORMATION);
    }
    else
    {
        DeleteObject(hBitmap);
    }
}

void Editor::UploadImage()
{
    WCHAR tempPath[MAX_PATH] = { 0 };
    if (!GetTempPathW(MAX_PATH, tempPath))
        return;

    SYSTEMTIME st;
    GetLocalTime(&st);
    WCHAR filename[MAX_PATH] = { 0 };
    swprintf_s(filename, L"%sSnipX_upload_%04d%02d%02d_%02d%02d%02d.png",
              tempPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    SaveImageToFile(filename, L"png");
    std::wstring message = L10n(L"图片上传占位已完成。\n当前版本已导出待上传图片：\n", L"Image upload placeholder completed.\nThe image has been exported for upload:\n");
    message += filename;
    MessageBoxW(m_hwnd, message.c_str(), L"SnipX", MB_ICONINFORMATION);
}

void Editor::RunOCR()
{
    const WCHAR* text = L10n(L"OCR 文字识别占位：当前截图已准备完成，后续可接入 Windows OCR 或第三方 OCR 引擎。",
                            L"OCR placeholder: the current screenshot is ready. Windows OCR or a third-party OCR engine can be integrated later.");
    if (OpenClipboard(m_hwnd))
    {
        EmptyClipboard();
        size_t bytes = (wcslen(text) + 1) * sizeof(WCHAR);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (hMem)
        {
            void* pMem = GlobalLock(hMem);
            if (pMem)
            {
                memcpy(pMem, text, bytes);
                GlobalUnlock(hMem);
                SetClipboardData(CF_UNICODETEXT, hMem);
            }
            else
            {
                GlobalFree(hMem);
            }
        }
        CloseClipboard();
    }
    MessageBoxW(m_hwnd, text, L"SnipX OCR", MB_ICONINFORMATION);
}

Bitmap* Editor::CreateCompositedBitmap()
{
    if (!m_pBitmap)
        return nullptr;

    int width = m_pBitmap->GetWidth();
    int height = m_pBitmap->GetHeight();
    Bitmap* finalBitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics* g = Graphics::FromImage(finalBitmap);
    if (!g)
    {
        delete finalBitmap;
        return nullptr;
    }

    g->SetSmoothingMode(SmoothingModeAntiAlias);
    g->DrawImage(m_pBitmap, 0, 0);

    for (auto anno : m_annotations)
    {
        anno->Draw(g);
    }

    delete g;
    return finalBitmap;
}

LRESULT CALLBACK Editor::EditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return 0;
    }
    
    Editor* pThis = (Editor*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        // 双缓冲绘制
        RECT rc;
        GetClientRect(hwnd, &rc);
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        
        if (pThis)
        {
            pThis->DrawEditor(hdcMem);
        }
        
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
        
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        
        EndPaint(hwnd, &ps);
        return 0;
    }
    
    case WM_LBUTTONDOWN:
    {
        if (pThis)
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            
            Point clientPt(pt.x, pt.y);

            Color panelColor;
            if (pThis->HitTestColorPanel(clientPt, panelColor))
            {
                pThis->m_currentColor = panelColor;
                pThis->m_showColorPanel = false;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            int sliderWidth = 0;
            if (pThis->HitTestStrokeSlider(clientPt, sliderWidth))
            {
                pThis->m_currentStrokeWidth = sliderWidth;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            // 检查是否点击工具栏
            AnnotationType tool;
            if (pThis->HitTestToolbar(clientPt, tool))
            {
                if (tool == ANNO_ARROW && pThis->m_currentTool == ANNO_ARROW)
                {
                    pThis->CycleArrowStyle();
                }
                else
                {
                    pThis->m_currentTool = tool;
                }
                pThis->m_showColorPanel = false;
                pThis->m_showStrokeSlider = false;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            if (pThis->HitTestColorPicker(clientPt))
            {
                pThis->ToggleColorPanel();
                pThis->m_showStrokeSlider = false;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            if (pThis->HitTestStrokeWidth(clientPt))
            {
                pThis->ToggleStrokeSlider();
                pThis->m_showColorPanel = false;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            
            // 检查是否点击底部栏
            int action;
            if (pThis->HitTestBottomBar(Point(pt.x, pt.y), action))
            {
                switch (action)
                {
                case 0: pThis->SaveImage(); break;
                case 1: pThis->CopyToClipboard(); break;
                case 2: 
                {
                    Bitmap* pinBitmap = pThis->CreateCompositedBitmap();
                    if (pinBitmap)
                    {
                        pThis->m_pApp->CreatePinWindow(pinBitmap);
                    }
                    break;
                }
                case 3: pThis->RunOCR(); break;
                case 4: pThis->UploadImage(); break;
                case 5: pThis->Close(); break;
                }
                return 0;
            }
            
            pThis->m_showColorPanel = false;
            pThis->m_showStrokeSlider = false;

            if (pThis->StartResizeSelectedAnnotation(clientPt))
            {
                SetCapture(hwnd);
                if (pThis->m_selectedAnnotation)
                    pThis->InvalidateAnnotationRect(pThis->m_selectedAnnotation->GetBounds());
                return 0;
            }

            if (pThis->StartMoveSelectedAnnotation(clientPt))
            {
                SetCapture(hwnd);
                if (pThis->m_selectedAnnotation)
                    pThis->InvalidateAnnotationRect(pThis->m_selectedAnnotation->GetBounds());
                return 0;
            }

            // 开始绘制标注
            Point imgPt(pt.x - pThis->m_imageX, pt.y - pThis->m_imageY);
            if (pThis->m_pBitmap &&
                imgPt.X >= 0 && imgPt.Y >= 0 && 
                imgPt.X < pThis->m_pBitmap->GetWidth() && 
                imgPt.Y < pThis->m_pBitmap->GetHeight())
            {
                pThis->StartAnnotation(imgPt);
                SetCapture(hwnd);
            }
        }
        return 0;
    }
    
    case WM_MOUSEMOVE:
    {
        if (pThis)
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            Point clientPt(pt.x, pt.y);
            if (pThis->m_resizingAnnotation)
            {
                Rect oldBounds = pThis->m_selectedAnnotation ? pThis->m_selectedAnnotation->GetBounds() : Rect();
                pThis->ResizeSelectedAnnotation(clientPt);
                if (pThis->m_selectedAnnotation)
                    pThis->InvalidateAnnotationTransition(oldBounds, pThis->m_selectedAnnotation->GetBounds());
            }
            else if (pThis->m_movingAnnotation)
            {
                Rect oldBounds = pThis->m_selectedAnnotation ? pThis->m_selectedAnnotation->GetBounds() : Rect();
                pThis->MoveSelectedAnnotation(clientPt);
                if (pThis->m_selectedAnnotation)
                    pThis->InvalidateAnnotationTransition(oldBounds, pThis->m_selectedAnnotation->GetBounds());
            }
            else if (pThis->m_drawing)
            {
                Rect oldBounds = pThis->m_currentAnnotation ? pThis->m_currentAnnotation->GetBounds() : Rect();
                Point imgPt(pt.x - pThis->m_imageX, pt.y - pThis->m_imageY);
                pThis->UpdateAnnotation(imgPt);
                if (pThis->m_currentAnnotation)
                    pThis->InvalidateAnnotationTransition(oldBounds, pThis->m_currentAnnotation->GetBounds());
            }
            else if (pThis->UpdateHoverState(clientPt))
            {
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;
    }
    
    case WM_LBUTTONUP:
    {
        if (pThis)
        {
            if (pThis->m_resizingAnnotation)
            {
                pThis->m_resizingAnnotation = false;
                pThis->m_resizeHandle = -1;
                ReleaseCapture();
                if (pThis->m_selectedAnnotation)
                    pThis->InvalidateAnnotationRect(pThis->m_selectedAnnotation->GetBounds());
            }
            else if (pThis->m_movingAnnotation)
            {
                pThis->m_movingAnnotation = false;
                ReleaseCapture();
                if (pThis->m_selectedAnnotation)
                    pThis->InvalidateAnnotationRect(pThis->m_selectedAnnotation->GetBounds());
            }
            else if (pThis->m_drawing)
            {
                Rect dirtyBounds = pThis->m_currentAnnotation ? pThis->m_currentAnnotation->GetBounds() : Rect();
                pThis->FinishAnnotation();
                ReleaseCapture();
                pThis->InvalidateAnnotationRect(dirtyBounds);
            }
        }
        return 0;
    }

    case WM_LBUTTONDBLCLK:
    {
        if (pThis)
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            Point imgPt(pt.x - pThis->m_imageX, pt.y - pThis->m_imageY);
            if (pThis->m_pBitmap &&
                imgPt.X >= 0 && imgPt.Y >= 0 &&
                imgPt.X < pThis->m_pBitmap->GetWidth() &&
                imgPt.Y < pThis->m_pBitmap->GetHeight())
            {
                pThis->CopyToClipboard();
                pThis->Close();
            }
        }
        return 0;
    }
    
    case WM_KEYDOWN:
    {
        if (wParam == VK_ESCAPE)
        {
            if (pThis)
            {
                pThis->Close();
            }
        }
        else if (wParam == 'S' && GetKeyState(VK_CONTROL) < 0)
        {
            if (pThis)
            {
                pThis->SaveImage();
            }
        }
        else if (wParam == 'C' && GetKeyState(VK_CONTROL) < 0)
        {
            if (pThis)
            {
                pThis->CopyToClipboard();
            }
        }
        else if (wParam == 'Z' && GetKeyState(VK_CONTROL) < 0)
        {
            if (pThis)
            {
                pThis->Undo();
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        else if (wParam == 'Y' && GetKeyState(VK_CONTROL) < 0)
        {
            if (pThis)
            {
                pThis->Redo();
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        else if (wParam == VK_DELETE || wParam == VK_BACK)
        {
            if (pThis)
            {
                pThis->DeleteSelectedAnnotation();
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        else
        {
            AnnotationType shortcutTool = GetToolFromShortcut(wParam);
            if (pThis && shortcutTool != ANNO_NONE)
            {
                pThis->m_currentTool = shortcutTool;
                pThis->m_showColorPanel = false;
                pThis->m_showStrokeSlider = false;
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;
    }
    
    case WM_DESTROY:
    {
        return 0;
    }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Editor::StartAnnotation(Point pt)
{
    m_drawing = true;
    m_startPoint = pt;
    m_lastPoint = pt;
    
    // 创建新标注对象
    switch (m_currentTool)
    {
    case ANNO_RECTANGLE:
    {
        RectangleAnnotation* anno = new RectangleAnnotation();
        anno->type = ANNO_RECTANGLE;
        anno->color = m_currentColor;
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->rect = Rect(pt.X, pt.Y, 0, 0);
        m_currentAnnotation = anno;
        break;
    }
    case ANNO_ELLIPSE:
    {
        EllipseAnnotation* anno = new EllipseAnnotation();
        anno->type = ANNO_ELLIPSE;
        anno->color = m_currentColor;
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->rect = Rect(pt.X, pt.Y, 0, 0);
        m_currentAnnotation = anno;
        break;
    }
    case ANNO_ARROW:
    {
        ArrowAnnotation* anno = new ArrowAnnotation();
        anno->type = ANNO_ARROW;
        anno->color = m_currentColor;
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->arrowStyle = m_currentArrowStyle;
        anno->start = pt;
        anno->end = pt;
        m_currentAnnotation = anno;
        break;
    }
    case ANNO_PENCIL:
    {
        PencilAnnotation* anno = new PencilAnnotation();
        anno->type = ANNO_PENCIL;
        anno->color = m_currentColor;
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->points.push_back(pt);
        m_currentAnnotation = anno;
        break;
    }
    case ANNO_LINE:
    {
        ArrowAnnotation* anno = new ArrowAnnotation();
        anno->type = ANNO_LINE;
        anno->color = m_currentColor;
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->arrowStyle = ARROW_STYLE_NORMAL;
        anno->start = pt;
        anno->end = pt;
        m_currentAnnotation = anno;
        break;
    }
    case ANNO_NUMBER:
    {
        NumberAnnotation* anno = new NumberAnnotation();
        anno->type = ANNO_NUMBER;
        anno->color = m_currentColor;
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->rect = Rect(pt.X + m_imageX - 12, pt.Y + m_imageY - 12, 24, 24);
        anno->number = m_nextNumberLabel++;
        m_currentAnnotation = anno;
        m_drawing = false;
        m_annotations.push_back(m_currentAnnotation);
        m_currentAnnotation = nullptr;
        break;
    }
    case ANNO_HIGHLIGHT:
    {
        HighlightAnnotation* anno = new HighlightAnnotation();
        anno->type = ANNO_HIGHLIGHT;
        anno->color = Color(120, 255, 255, 0);
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->rect = Rect(pt.X, pt.Y, 0, 0);
        m_currentAnnotation = anno;
        break;
    }
    case ANNO_TEXT:
    {
        TextAnnotation* anno = new TextAnnotation();
        anno->type = ANNO_TEXT;
        anno->color = m_currentColor;
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->rect = Rect(pt.X + m_imageX, pt.Y + m_imageY, 100, 30);
        anno->text = L"文字";
        anno->fontName = L"Microsoft YaHei";
        anno->fontSize = 12;
        m_currentAnnotation = anno;
        m_drawing = false;  // 文字工具不需要拖拽
        m_annotations.push_back(m_currentAnnotation);
        m_currentAnnotation = nullptr;
        break;
    }
    case ANNO_MOSAIC:
    {
        MosaicAnnotation* anno = new MosaicAnnotation();
        anno->type = ANNO_MOSAIC;
        anno->color = m_currentColor;
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->rect = Rect(pt.X, pt.Y, 0, 0);
        anno->blockSize = 10;
        anno->sourceBitmap = m_pBitmap->Clone(0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight(), PixelFormat32bppARGB);
        anno->sourceOffsetX = m_imageX;
        anno->sourceOffsetY = m_imageY;
        m_currentAnnotation = anno;
        break;
    }
    case ANNO_BLUR:
    {
        BlurAnnotation* anno = new BlurAnnotation();
        anno->type = ANNO_BLUR;
        anno->color = m_currentColor;
        anno->strokeWidth = m_currentStrokeWidth;
        anno->selected = false;
        anno->rect = Rect(pt.X, pt.Y, 0, 0);
        anno->sourceBitmap = m_pBitmap->Clone(0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight(), PixelFormat32bppARGB);
        anno->sourceOffsetX = m_imageX;
        anno->sourceOffsetY = m_imageY;
        m_currentAnnotation = anno;
        break;
    }
    default:
        m_currentAnnotation = nullptr;
        break;
    }
}

void Editor::UpdateAnnotation(Point pt)
{
    if (!m_currentAnnotation)
        return;
    
    m_lastPoint = pt;
    
    switch (m_currentAnnotation->type)
    {
    case ANNO_RECTANGLE:
    {
        RectangleAnnotation* anno = (RectangleAnnotation*)m_currentAnnotation;
        int x = (std::min)(m_startPoint.X, pt.X);
        int y = (std::min)(m_startPoint.Y, pt.Y);
        int w = abs(pt.X - m_startPoint.X);
        int h = abs(pt.Y - m_startPoint.Y);
        anno->rect = Rect(x + m_imageX, y + m_imageY, w, h);
        break;
    }
    case ANNO_ELLIPSE:
    {
        EllipseAnnotation* anno = (EllipseAnnotation*)m_currentAnnotation;
        int x = (std::min)(m_startPoint.X, pt.X);
        int y = (std::min)(m_startPoint.Y, pt.Y);
        int w = abs(pt.X - m_startPoint.X);
        int h = abs(pt.Y - m_startPoint.Y);
        anno->rect = Rect(x + m_imageX, y + m_imageY, w, h);
        break;
    }
    case ANNO_ARROW:
    case ANNO_LINE:
    {
        ArrowAnnotation* anno = (ArrowAnnotation*)m_currentAnnotation;
        anno->start = Point(m_startPoint.X + m_imageX, m_startPoint.Y + m_imageY);
        anno->end = Point(pt.X + m_imageX, pt.Y + m_imageY);
        break;
    }
    case ANNO_PENCIL:
    {
        PencilAnnotation* anno = (PencilAnnotation*)m_currentAnnotation;
        anno->points.push_back(Point(pt.X + m_imageX, pt.Y + m_imageY));
        break;
    }
    case ANNO_HIGHLIGHT:
    {
        HighlightAnnotation* anno = (HighlightAnnotation*)m_currentAnnotation;
        int x = (std::min)(m_startPoint.X, pt.X);
        int y = (std::min)(m_startPoint.Y, pt.Y);
        int w = abs(pt.X - m_startPoint.X);
        int h = abs(pt.Y - m_startPoint.Y);
        anno->rect = Rect(x + m_imageX, y + m_imageY, w, h);
        break;
    }
    case ANNO_MOSAIC:
    {
        MosaicAnnotation* anno = (MosaicAnnotation*)m_currentAnnotation;
        int x = (std::min)(m_startPoint.X, pt.X);
        int y = (std::min)(m_startPoint.Y, pt.Y);
        int w = abs(pt.X - m_startPoint.X);
        int h = abs(pt.Y - m_startPoint.Y);
        anno->rect = Rect(x + m_imageX, y + m_imageY, w, h);
        break;
    }
    case ANNO_BLUR:
    {
        BlurAnnotation* anno = (BlurAnnotation*)m_currentAnnotation;
        int x = (std::min)(m_startPoint.X, pt.X);
        int y = (std::min)(m_startPoint.Y, pt.Y);
        int w = abs(pt.X - m_startPoint.X);
        int h = abs(pt.Y - m_startPoint.Y);
        anno->rect = Rect(x + m_imageX, y + m_imageY, w, h);
        break;
    }
    }
}

void Editor::FinishAnnotation()
{
    if (m_currentAnnotation)
    {
        m_annotations.push_back(m_currentAnnotation);
        m_currentAnnotation = nullptr;
        
        // 清空重做栈
        for (auto anno : m_undoStack)
        {
            delete anno;
        }
        m_undoStack.clear();
    }
    
    m_drawing = false;
}

void Editor::SelectAnnotation(Point pt)
{
    m_selectedAnnotation = nullptr;
    for (auto anno : m_annotations)
    {
        anno->selected = false;
    }
    
    // 从后往前查找（后面的在上层）
    for (int i = (int)m_annotations.size() - 1; i >= 0; i--)
    {
        if (m_annotations[i]->HitTest(pt))
        {
            m_selectedAnnotation = m_annotations[i];
            m_selectedAnnotation->selected = true;
            break;
        }
    }
}

bool Editor::StartMoveSelectedAnnotation(Point pt)
{
    SelectAnnotation(pt);
    if (!m_selectedAnnotation)
        return false;

    m_movingAnnotation = true;
    m_drawing = false;
    m_lastPoint = pt;
    return true;
}

void Editor::MoveSelectedAnnotation(Point pt)
{
    if (!m_selectedAnnotation || !m_movingAnnotation)
        return;

    int dx = pt.X - m_lastPoint.X;
    int dy = pt.Y - m_lastPoint.Y;
    if (dx == 0 && dy == 0)
        return;

    m_selectedAnnotation->Move(dx, dy);
    m_lastPoint = pt;
}

bool Editor::StartResizeSelectedAnnotation(Point pt)
{
    if (!m_selectedAnnotation || !m_selectedAnnotation->selected)
        return false;

    int handleIndex = -1;
    Rect bounds = m_selectedAnnotation->GetBounds();
    if (!HitResizeHandle(bounds, pt, handleIndex))
        return false;

    m_resizingAnnotation = true;
    m_movingAnnotation = false;
    m_drawing = false;
    m_resizeHandle = handleIndex;
    m_resizeStartBounds = bounds;
    m_lastPoint = pt;
    return true;
}

void Editor::ResizeSelectedAnnotation(Point pt)
{
    if (!m_selectedAnnotation || !m_resizingAnnotation)
        return;

    Rect resizedBounds = m_resizeStartBounds;
    ApplyResizeHandle(resizedBounds, m_resizeHandle, pt);
    m_selectedAnnotation->ResizeToBounds(resizedBounds);
    m_lastPoint = pt;
}

void Editor::DeleteSelectedAnnotation()
{
    if (m_selectedAnnotation)
    {
        auto it = std::find(m_annotations.begin(), m_annotations.end(), m_selectedAnnotation);
        if (it != m_annotations.end())
        {
            m_annotations.erase(it);
            delete m_selectedAnnotation;
            m_selectedAnnotation = nullptr;
        }
    }
}

void Editor::Undo()
{
    if (!m_annotations.empty())
    {
        Annotation* anno = m_annotations.back();
        m_annotations.pop_back();
        m_undoStack.push_back(anno);
    }
}

void Editor::Redo()
{
    if (!m_undoStack.empty())
    {
        Annotation* anno = m_undoStack.back();
        m_undoStack.pop_back();
        m_annotations.push_back(anno);
    }
}

bool Editor::HitTestToolbar(Point pt, AnnotationType& tool)
{
    if (pt.Y < TOOLBAR_Y || pt.Y > TOOLBAR_Y + TOOL_BUTTON_HEIGHT)
        return false;
    
    for (int i = 0; i < TOOL_BUTTON_COUNT; i++)
    {
        Rect btnRect = GetToolButtonRect(i);
        if (btnRect.Contains(pt))
        {
            tool = TOOL_BUTTONS[i].type;
            return true;
        }
    }
    
    return false;
}

bool Editor::HitTestColorPicker(Point pt)
{
    return pt.X >= COLOR_BOX_OFFSET_X && pt.X <= COLOR_BOX_OFFSET_X + COLOR_BOX_SIZE &&
           pt.Y >= COLOR_BOX_OFFSET_Y && pt.Y <= COLOR_BOX_OFFSET_Y + COLOR_BOX_SIZE;
}

bool Editor::HitTestStrokeWidth(Point pt)
{
    return pt.X >= STROKE_WIDTH_X && pt.X <= STROKE_WIDTH_X + STROKE_WIDTH_WIDTH &&
           pt.Y >= STROKE_WIDTH_Y && pt.Y <= STROKE_WIDTH_Y + STROKE_WIDTH_HEIGHT;
}

bool Editor::HitTestColorPanel(Point pt, Color& color)
{
    if (!m_showColorPanel || !GetColorPanelBounds().Contains(pt))
        return false;

    for (int i = 0; i < COLOR_PANEL_COUNT; i++)
    {
        if (GetPresetColorRect(i).Contains(pt))
        {
            color = PRESET_COLORS[i];
            return true;
        }
    }

    return false;
}

bool Editor::HitTestStrokeSlider(Point pt, int& width)
{
    if (!m_showStrokeSlider || !GetStrokeSliderPanelBounds().Contains(pt))
        return false;

    width = GetStrokeWidthFromSliderX(pt.X);
    return true;
}

void Editor::ToggleColorPanel()
{
    m_showColorPanel = !m_showColorPanel;
}

void Editor::ToggleStrokeSlider()
{
    m_showStrokeSlider = !m_showStrokeSlider;
}

bool Editor::UpdateHoverState(Point pt)
{
    bool changed = false;
    AnnotationType hoverTool = ANNO_NONE;
    bool hoverColor = false;
    bool hoverStroke = false;
    int hoverBottom = -1;

    if (pt.Y >= TOOLBAR_Y && pt.Y <= TOOLBAR_Y + TOOL_BUTTON_HEIGHT)
    {
        for (int i = 0; i < TOOL_BUTTON_COUNT; i++)
        {
            Rect btnRect = GetToolButtonRect(i);
            if (btnRect.Contains(pt))
            {
                hoverTool = TOOL_BUTTONS[i].type;
                break;
            }
        }
    }

    hoverColor = GetColorPickerRect().Contains(pt);
    hoverStroke = GetStrokeWidthRect().Contains(pt);

    RECT rc;
    GetClientRect(m_hwnd, &rc);
    int bottomY = rc.bottom - m_bottomBarHeight;
    if (pt.Y >= bottomY + 10 && pt.Y <= bottomY + 40)
    {
        int x = 10;
        int btnWidth = 80;
        int spacing = 10;
        for (int i = 0; i < 4; i++)
        {
            if (pt.X >= x && pt.X <= x + btnWidth)
            {
                hoverBottom = i;
                break;
            }
            x += btnWidth + spacing;
        }
    }

    if (m_hoverTool != hoverTool)
    {
        m_hoverTool = hoverTool;
        changed = true;
    }
    if (m_hoverColorPicker != hoverColor)
    {
        m_hoverColorPicker = hoverColor;
        changed = true;
    }
    if (m_hoverStrokeWidth != hoverStroke)
    {
        m_hoverStrokeWidth = hoverStroke;
        changed = true;
    }
    if (m_hoverBottomAction != hoverBottom)
    {
        m_hoverBottomAction = hoverBottom;
        changed = true;
    }

    m_hoverPoint = pt;
    return changed;
}

const WCHAR* Editor::GetHoverTooltipText() const
{
    if (m_hoverTool != ANNO_NONE)
    {
        for (int i = 0; i < TOOL_BUTTON_COUNT; i++)
        {
            if (TOOL_BUTTONS[i].type == m_hoverTool)
                return TOOL_BUTTONS[i].tooltip;
        }
    }

    if (m_hoverColorPicker)
        return L"点击展开颜色选择面板";
    if (m_hoverStrokeWidth)
        return L"点击展开粗细滑块";
    if (m_hoverBottomAction >= 0)
    {
        static const WCHAR* tips[] = { L"保存到文件", L"复制到剪贴板", L"贴图到桌面", L"关闭编辑器" };
        return tips[m_hoverBottomAction];
    }

    return nullptr;
}

void Editor::DrawHoverTooltip(Graphics* graphics)
{
    const WCHAR* tip = GetHoverTooltipText();
    if (!tip)
        return;

    Font font(L"Microsoft YaHei UI", 8);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    SolidBrush bgBrush(Color(205, 17, 24, 39));
    Pen borderPen(Color(160, 148, 163, 184), 1);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    RectF layoutRect((REAL)m_hoverPoint.X + 12.0f, (REAL)m_hoverPoint.Y + 16.0f, 126.0f, 22.0f);
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    if (layoutRect.GetRight() > rc.right - 4)
        layoutRect.X = (REAL)rc.right - layoutRect.Width - 4.0f;
    if (layoutRect.GetBottom() > rc.bottom - 4)
        layoutRect.Y = (REAL)m_hoverPoint.Y - layoutRect.Height - 12.0f;
    if (layoutRect.X < 4.0f)
        layoutRect.X = 4.0f;
    if (layoutRect.Y < 4.0f)
        layoutRect.Y = 4.0f;

    graphics->FillRectangle(&bgBrush, layoutRect);
    graphics->DrawRectangle(&borderPen, layoutRect);
    graphics->DrawString(tip, -1, &font, layoutRect, &format, &textBrush);
}

void Editor::ChooseCurrentColor()
{
    static COLORREF customColors[16] = { 0 };
    CHOOSECOLORW cc = { 0 };
    COLORREF initialColor = RGB(m_currentColor.GetR(), m_currentColor.GetG(), m_currentColor.GetB());

    cc.lStructSize = sizeof(CHOOSECOLORW);
    cc.hwndOwner = m_hwnd;
    cc.rgbResult = initialColor;
    cc.lpCustColors = customColors;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColorW(&cc))
    {
        m_currentColor = Color(255, GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
    }
}

void Editor::CycleStrokeWidth()
{
    const int widths[] = { 1, 2, 4, 6, 8, 10 };
    const int widthCount = sizeof(widths) / sizeof(widths[0]);

    for (int i = 0; i < widthCount; i++)
    {
        if (m_currentStrokeWidth == widths[i])
        {
            m_currentStrokeWidth = widths[(i + 1) % widthCount];
            return;
        }
    }

    m_currentStrokeWidth = widths[0];
}

void Editor::CycleArrowStyle()
{
    int nextStyle = ((int)m_currentArrowStyle + 1) % 4;
    m_currentArrowStyle = (ArrowStyle)nextStyle;
}

bool Editor::HitTestBottomBar(Point pt, int& action)
{
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    
    int bottomY = rc.bottom - m_bottomBarHeight;
    
    if (pt.Y < bottomY + 10 || pt.Y > bottomY + 40)
        return false;
    
    int x = 10;
    int btnWidth = 80;
    int spacing = 10;

    for (int i = 0; i < 6; i++)
    {
        if (pt.X >= x && pt.X <= x + btnWidth)
        {
            action = i;
            return true;
        }
        x += btnWidth + spacing;
    }
    
    return false;
}

// TextAnnotation 实现
void TextAnnotation::Draw(Graphics* graphics)
{
    if (text.empty())
        return;
    
    Font font(fontName.c_str(), (REAL)fontSize);
    SolidBrush brush(color);
    
    // 绘制文字背景
    SolidBrush bgBrush(Color(200, 255, 255, 255));
    graphics->FillRectangle(&bgBrush, rect);
    
    // 绘制文字
    StringFormat format;
    format.SetAlignment(StringAlignmentNear);
    format.SetLineAlignment(StringAlignmentNear);
    
    RectF rectF((REAL)rect.X, (REAL)rect.Y, (REAL)rect.Width, (REAL)rect.Height);
    graphics->DrawString(text.c_str(), -1, &font, rectF, &format, &brush);
    
    // 绘制边框
    if (selected)
    {
        DrawSelectionFrame(graphics, rect);
        DrawResizeHandles(graphics, rect);
    }
}

bool TextAnnotation::HitTest(Point pt)
{
    return rect.Contains(pt);
}

void TextAnnotation::Move(int dx, int dy)
{
    rect.X += dx;
    rect.Y += dy;
}

Rect TextAnnotation::GetBounds() const
{
    return NormalizeRect(rect);
}

void TextAnnotation::ResizeToBounds(const Rect& bounds)
{
    rect = NormalizeRect(bounds);
}

// MosaicAnnotation 实现
void MosaicAnnotation::Draw(Graphics* graphics)
{
    Rect bounds = NormalizeRect(rect);
    if (!sourceBitmap || bounds.Width <= 0 || bounds.Height <= 0)
        return;

    Rect sourceRect = ClipRectToBitmap(Rect(bounds.X - sourceOffsetX, bounds.Y - sourceOffsetY, bounds.Width, bounds.Height), sourceBitmap);
    if (sourceRect.Width <= 0 || sourceRect.Height <= 0)
        return;

    Rect drawRect(sourceRect.X + sourceOffsetX, sourceRect.Y + sourceOffsetY, sourceRect.Width, sourceRect.Height);
    if (cacheDirty || !cachedBitmap || cachedRect.X != drawRect.X || cachedRect.Y != drawRect.Y ||
        cachedRect.Width != drawRect.Width || cachedRect.Height != drawRect.Height)
    {
        if (cachedBitmap)
        {
            delete cachedBitmap;
            cachedBitmap = nullptr;
        }

        cachedBitmap = new Bitmap(drawRect.Width, drawRect.Height, PixelFormat32bppARGB);
        Graphics* cacheGraphics = Graphics::FromImage(cachedBitmap);
        if (cacheGraphics)
        {
            for (int y = 0; y < drawRect.Height; y += blockSize)
            {
                for (int x = 0; x < drawRect.Width; x += blockSize)
                {
                    int bw = (std::min)(blockSize, drawRect.Width - x);
                    int bh = (std::min)(blockSize, drawRect.Height - y);
                    int cx = sourceRect.X + x + bw / 2;
                    int cy = sourceRect.Y + y + bh / 2;

                    Color pixelColor;
                    sourceBitmap->GetPixel(cx, cy, &pixelColor);
                    SolidBrush brush(pixelColor);
                    cacheGraphics->FillRectangle(&brush, x, y, bw, bh);
                }
            }
            delete cacheGraphics;
        }

        cachedRect = drawRect;
        cacheDirty = false;
    }

    if (cachedBitmap)
        graphics->DrawImage(cachedBitmap, cachedRect);

    if (selected)
    {
        DrawSelectionFrame(graphics, bounds);
        DrawResizeHandles(graphics, bounds);
    }
}

bool MosaicAnnotation::HitTest(Point pt)
{
    return rect.Contains(pt);
}

void MosaicAnnotation::Move(int dx, int dy)
{
    rect.X += dx;
    rect.Y += dy;
    InvalidateCache();
}

Rect MosaicAnnotation::GetBounds() const
{
    return NormalizeRect(rect);
}

void MosaicAnnotation::ResizeToBounds(const Rect& bounds)
{
    rect = NormalizeRect(bounds);
    InvalidateCache();
}

void MosaicAnnotation::InvalidateCache()
{
    cacheDirty = true;
}

// BlurAnnotation 实现
void BlurAnnotation::Draw(Graphics* graphics)
{
    Rect bounds = NormalizeRect(rect);
    if (!sourceBitmap || bounds.Width <= 0 || bounds.Height <= 0)
        return;

    Rect sourceRect = ClipRectToBitmap(Rect(bounds.X - sourceOffsetX, bounds.Y - sourceOffsetY, bounds.Width, bounds.Height), sourceBitmap);
    if (sourceRect.Width <= 0 || sourceRect.Height <= 0)
        return;

    Rect drawRect(sourceRect.X + sourceOffsetX, sourceRect.Y + sourceOffsetY, sourceRect.Width, sourceRect.Height);
    if (cacheDirty || !cachedBitmap || cachedRect.X != drawRect.X || cachedRect.Y != drawRect.Y ||
        cachedRect.Width != drawRect.Width || cachedRect.Height != drawRect.Height)
    {
        if (cachedBitmap)
        {
            delete cachedBitmap;
            cachedBitmap = nullptr;
        }

        int smallWidth = (std::max)(1, sourceRect.Width / 10);
        int smallHeight = (std::max)(1, sourceRect.Height / 10);
        Bitmap smallBitmap(smallWidth, smallHeight, PixelFormat32bppARGB);
        Graphics* smallGraphics = Graphics::FromImage(&smallBitmap);
        if (smallGraphics)
        {
            smallGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
            smallGraphics->DrawImage(sourceBitmap,
                                     Rect(0, 0, smallWidth, smallHeight),
                                     sourceRect.X, sourceRect.Y, sourceRect.Width, sourceRect.Height,
                                     UnitPixel);
            delete smallGraphics;
        }

        cachedBitmap = new Bitmap(drawRect.Width, drawRect.Height, PixelFormat32bppARGB);
        Graphics* cacheGraphics = Graphics::FromImage(cachedBitmap);
        if (cacheGraphics)
        {
            cacheGraphics->SetInterpolationMode(InterpolationModeNearestNeighbor);
            cacheGraphics->DrawImage(&smallBitmap, Rect(0, 0, drawRect.Width, drawRect.Height));
            delete cacheGraphics;
        }

        cachedRect = drawRect;
        cacheDirty = false;
    }

    if (cachedBitmap)
        graphics->DrawImage(cachedBitmap, cachedRect);

    if (selected)
    {
        DrawSelectionFrame(graphics, bounds);
        DrawResizeHandles(graphics, bounds);
    }
}

bool BlurAnnotation::HitTest(Point pt)
{
    return rect.Contains(pt);
}

void BlurAnnotation::Move(int dx, int dy)
{
    rect.X += dx;
    rect.Y += dy;
    InvalidateCache();
}

Rect BlurAnnotation::GetBounds() const
{
    return NormalizeRect(rect);
}

void BlurAnnotation::ResizeToBounds(const Rect& bounds)
{
    rect = NormalizeRect(bounds);
    InvalidateCache();
}

void BlurAnnotation::InvalidateCache()
{
    cacheDirty = true;
}

