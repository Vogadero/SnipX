// 截图编辑器

#ifndef EDITOR_H
#define EDITOR_H

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>

using namespace Gdiplus;

class SnipXApp;

// 标注工具类型
enum AnnotationType
{
    ANNO_NONE = 0,
    ANNO_RECTANGLE,
    ANNO_ELLIPSE,
    ANNO_ARROW,
    ANNO_PENCIL,
    ANNO_TEXT,
    ANNO_LINE,
    ANNO_NUMBER,
    ANNO_HIGHLIGHT,
    ANNO_MOSAIC,
    ANNO_BLUR
};

// 箭头样式
enum ArrowStyle
{
    ARROW_STYLE_NORMAL = 0,
    ARROW_STYLE_DOUBLE_LINE,
    ARROW_STYLE_FILLED,
    ARROW_STYLE_OPEN
};

// 标注对象基类
class Annotation
{
public:
    virtual ~Annotation() {}
    virtual void Draw(Graphics* graphics) = 0;
    virtual bool HitTest(Point pt) = 0;
    /**
     * 按指定偏移量移动标注对象。
     *
     * @param dx 横向偏移像素。
     * @param dy 纵向偏移像素。
     */
    virtual void Move(int dx, int dy) = 0;
    /**
     * 获取标注对象在编辑器客户区坐标中的外接矩形。
     *
     * @return 标注对象当前边界。
     */
    virtual Rect GetBounds() const = 0;
    /**
     * 按新的外接矩形调整标注大小和位置。
     *
     * @param bounds 新的客户区坐标边界。
     */
    virtual void ResizeToBounds(const Rect& bounds) = 0;
    
    AnnotationType type;
    Color color;
    int strokeWidth;
    bool selected;
};

// 矩形标注
class RectangleAnnotation : public Annotation
{
public:
    Rect rect;
    void Draw(Graphics* graphics) override;
    bool HitTest(Point pt) override;
    void Move(int dx, int dy) override;
    Rect GetBounds() const override;
    void ResizeToBounds(const Rect& bounds) override;
};

// 椭圆标注
class EllipseAnnotation : public Annotation
{
public:
    Rect rect;
    void Draw(Graphics* graphics) override;
    bool HitTest(Point pt) override;
    void Move(int dx, int dy) override;
    Rect GetBounds() const override;
    void ResizeToBounds(const Rect& bounds) override;
};

// 箭头标注
class ArrowAnnotation : public Annotation
{
public:
    Point start;
    Point end;
    ArrowStyle arrowStyle;
    void Draw(Graphics* graphics) override;
    bool HitTest(Point pt) override;
    void Move(int dx, int dy) override;
    Rect GetBounds() const override;
    void ResizeToBounds(const Rect& bounds) override;
};

// 铅笔标注
class PencilAnnotation : public Annotation
{
public:
    std::vector<Point> points;
    void Draw(Graphics* graphics) override;
    bool HitTest(Point pt) override;
    void Move(int dx, int dy) override;
    Rect GetBounds() const override;
    void ResizeToBounds(const Rect& bounds) override;
};

// 文字标注
class TextAnnotation : public Annotation
{
public:
    Rect rect;
    std::wstring text;
    std::wstring fontName;
    int fontSize;
    void Draw(Graphics* graphics) override;
    bool HitTest(Point pt) override;
    void Move(int dx, int dy) override;
    Rect GetBounds() const override;
    void ResizeToBounds(const Rect& bounds) override;
};

// 序号标签标注
class NumberAnnotation : public Annotation
{
public:
    Rect rect;
    int number;
    void Draw(Graphics* graphics) override;
    bool HitTest(Point pt) override;
    void Move(int dx, int dy) override;
    Rect GetBounds() const override;
    void ResizeToBounds(const Rect& bounds) override;
};

// 高亮标注
class HighlightAnnotation : public Annotation
{
public:
    Rect rect;
    void Draw(Graphics* graphics) override;
    bool HitTest(Point pt) override;
    void Move(int dx, int dy) override;
    Rect GetBounds() const override;
    void ResizeToBounds(const Rect& bounds) override;
};

// 马赛克标注
class MosaicAnnotation : public Annotation
{
public:
    Rect rect;
    Bitmap* sourceBitmap;
    Bitmap* cachedBitmap;
    Rect cachedRect;
    int sourceOffsetX;
    int sourceOffsetY;
    int blockSize;
    bool cacheDirty;

    MosaicAnnotation()
        : sourceBitmap(nullptr), cachedBitmap(nullptr), sourceOffsetX(0), sourceOffsetY(0), blockSize(10), cacheDirty(true) {}
    ~MosaicAnnotation() { if (sourceBitmap) delete sourceBitmap; if (cachedBitmap) delete cachedBitmap; }

    void Draw(Graphics* graphics) override;
    bool HitTest(Point pt) override;
    void Move(int dx, int dy) override;
    Rect GetBounds() const override;
    void ResizeToBounds(const Rect& bounds) override;
    /**
     * 清空已生成的马赛克缓存，下次绘制时按当前区域重建。
     */
    void InvalidateCache();
};

// 模糊标注
class BlurAnnotation : public Annotation
{
public:
    Rect rect;
    Bitmap* sourceBitmap;
    Bitmap* cachedBitmap;
    Rect cachedRect;
    int sourceOffsetX;
    int sourceOffsetY;
    bool cacheDirty;

    BlurAnnotation()
        : sourceBitmap(nullptr), cachedBitmap(nullptr), sourceOffsetX(0), sourceOffsetY(0), cacheDirty(true) {}
    ~BlurAnnotation() { if (sourceBitmap) delete sourceBitmap; if (cachedBitmap) delete cachedBitmap; }

    void Draw(Graphics* graphics) override;
    bool HitTest(Point pt) override;
    void Move(int dx, int dy) override;
    Rect GetBounds() const override;
    void ResizeToBounds(const Rect& bounds) override;
    /**
     * 清空已生成的模糊缓存，下次绘制时按当前区域重建。
     */
    void InvalidateCache();
};

// 编辑器主类
class Editor
{
public:
    Editor(SnipXApp* pApp);
    ~Editor();
    
    void Open(Bitmap* pBitmap);
    void Close();
    
private:
    static LRESULT CALLBACK EditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    void CreateEditorWindow();
    void DestroyEditorWindow();
    void DrawEditor(HDC hdc);
    void DrawToolbar(Graphics* graphics);
    void DrawBottomBar(Graphics* graphics);
    void SaveImage();
    void CopyToClipboard();
    /**
     * 将当前编辑结果保存为临时 PNG 文件并提示文件位置，作为图片上传/图床功能的本地导出入口。
     */
    void UploadImage();
    /**
     * 对当前编辑结果执行基础 OCR 占位识别并复制提示文本。
     */
    void RunOCR();
    /**
     * 创建包含截图原图和所有标注的最终位图，供保存和复制使用。
     *
     * @return 调用方负责释放的合成位图，失败时返回 nullptr。
     */
    Bitmap* CreateCompositedBitmap();
    /**
     * 保存最终图片到指定文件，并在成功后写入历史记录。
     *
     * @param filename 输出文件路径。
     * @param format 图片格式。
     */
    void SaveImageToFile(const std::wstring& filename, const std::wstring& format);
    
    // 标注操作

    void StartAnnotation(Point pt);
    void UpdateAnnotation(Point pt);
    void FinishAnnotation();
    void SelectAnnotation(Point pt);
    /**
     * 按鼠标位置尝试开始拖拽移动已有标注。
     *
     * @param pt 客户区坐标。
     * @return 命中并开始移动时返回 true。
     */
    bool StartMoveSelectedAnnotation(Point pt);
    /**
     * 根据当前鼠标位置移动已选中的标注对象。
     *
     * @param pt 客户区坐标。
     */
    void MoveSelectedAnnotation(Point pt);
    /**
     * 按鼠标位置尝试开始拖拽选中标注的缩放控制点。
     *
     * @param pt 客户区坐标。
     * @return 命中缩放控制点并进入缩放状态时返回 true。
     */
    bool StartResizeSelectedAnnotation(Point pt);
    /**
     * 根据当前鼠标位置调整已选中标注对象大小。
     *
     * @param pt 客户区坐标。
     */
    void ResizeSelectedAnnotation(Point pt);
    void DeleteSelectedAnnotation();
    void Undo();
    void Redo();
    
    // UI 交互
    bool HitTestToolbar(Point pt, AnnotationType& tool);
    bool HitTestColorPicker(Point pt);
    bool HitTestStrokeWidth(Point pt);
    bool HitTestBottomBar(Point pt, int& action);
    /**
     * 命中测试预设颜色面板并返回被点击的颜色。
     *
     * @param pt 客户区坐标。
     * @param color 命中的预设颜色。
     * @return 命中颜色块时返回 true。
     */
    bool HitTestColorPanel(Point pt, Color& color);
    /**
     * 命中测试粗细滑块并按点击位置换算线宽。
     *
     * @param pt 客户区坐标。
     * @param width 命中的线宽值。
     * @return 命中滑块区域时返回 true。
     */
    bool HitTestStrokeSlider(Point pt, int& width);
    /**
     * 切换颜色选择器面板显示状态。
     */
    void ToggleColorPanel();
    /**
     * 切换粗细滑块面板显示状态。
     */
    void ToggleStrokeSlider();
    /**
     * 根据鼠标位置更新工具栏、底部栏等 UI 悬停状态。
     *
     * @param pt 客户区坐标。
     * @return 悬停状态发生变化时返回 true。
     */
    bool UpdateHoverState(Point pt);
    /**
     * 绘制当前悬停控件的简易提示文本。
     *
     * @param graphics GDI+ 绘图对象。
     */
    void DrawHoverTooltip(Graphics* graphics);
    /**
     * 获取当前悬停控件的提示文本。
     *
     * @return 提示文本指针，无悬停提示时返回 nullptr。
     */
    const WCHAR* GetHoverTooltipText() const;
    /**
     * 打开系统颜色选择对话框并更新当前标注颜色。
     */
    void ChooseCurrentColor();
    /**
     * 在预设线宽之间循环切换当前标注粗细。
     */
    void CycleStrokeWidth();
    /**
     * 循环切换箭头工具样式。
     */
    void CycleArrowStyle();
    /**
     * 失效指定标注区域并附带边距，减少拖拽、缩放和绘制时的整窗刷新。
     *
     * @param bounds 标注在客户区坐标中的外接矩形。
     */
    void InvalidateAnnotationRect(const Rect& bounds);
    /**
     * 同时失效两个标注区域，用于移动、缩放前后位置合并刷新。
     *
     * @param oldBounds 修改前外接矩形。
     * @param newBounds 修改后外接矩形。
     */
    void InvalidateAnnotationTransition(const Rect& oldBounds, const Rect& newBounds);

    SnipXApp* m_pApp;
    HWND m_hwnd;
    Bitmap* m_pBitmap;
    
    // 标注工具
    AnnotationType m_currentTool;
    Color m_currentColor;
    int m_currentStrokeWidth;
    int m_nextNumberLabel;
    ArrowStyle m_currentArrowStyle;
    
    std::vector<Annotation*> m_annotations;
    std::vector<Annotation*> m_undoStack;
    
    // 当前正在绘制的标注
    Annotation* m_currentAnnotation;
    Annotation* m_selectedAnnotation;
    
    // 交互状态
    bool m_drawing;
    bool m_movingAnnotation;
    bool m_resizingAnnotation;
    AnnotationType m_hoverTool;
    int m_hoverBottomAction;
    bool m_hoverColorPicker;
    bool m_hoverStrokeWidth;
    bool m_showColorPanel;
    bool m_showStrokeSlider;
    Point m_hoverPoint;
    int m_resizeHandle;
    Rect m_resizeStartBounds;
    Point m_startPoint;
    Point m_lastPoint;
    
    // UI 布局
    int m_toolbarHeight;
    int m_bottomBarHeight;
    int m_imageX;
    int m_imageY;
};

#endif // EDITOR_H
