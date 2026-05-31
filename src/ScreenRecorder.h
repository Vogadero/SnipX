// 录屏功能

#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>

using namespace Gdiplus;

class SnipXApp;

class ScreenRecorder
{
public:
    explicit ScreenRecorder(SnipXApp* pApp);
    ~ScreenRecorder();

    /**
     * 开始录制虚拟桌面画面为 PNG 帧序列。
     *
     * @return 成功创建录制会话并启动定时采集时返回 true。
     */
    bool Start();

    /**
     * 停止当前录制会话并释放采集窗口。
     */
    void Stop();

    /**
     * 返回当前是否正在录制。
     */
    bool IsRecording() const { return m_recording; }

    /**
     * 获取当前录制帧保存目录。
     */
    std::wstring GetOutputDirectory() const { return m_outputDirectory; }

    /**
     * 获取停止录制后生成的 GIF 文件路径。
     */
    std::wstring GetGifPath() const { return m_gifPath; }

private:
    static LRESULT CALLBACK RecorderWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * 创建用于接收定时器消息的隐藏窗口。
     *
     * @return 窗口创建成功时返回 true。
     */
    bool CreateRecorderWindow();

    /**
     * 销毁隐藏录制窗口。
     */
    void DestroyRecorderWindow();

    /**
     * 捕获当前虚拟桌面并保存为一帧 PNG 图片。
     *
     * @return 保存成功时返回 true。
     */
    bool CaptureFrame();

    /**
     * 创建本次录制帧输出目录。
     *
     * @return 成功创建或确认目录存在时返回 true。
     */
    bool PrepareOutputDirectory();

    /**
     * 将已采集的录屏帧序列导出为 GIF 文件。
     *
     * @return 成功生成 GIF 文件时返回 true。
     */
    bool ExportGif();

    SnipXApp* m_pApp;
    HWND m_hwnd;
    bool m_recording;
    int m_frameIndex;
    UINT_PTR m_timerId;
    std::wstring m_outputDirectory;
    std::wstring m_gifPath;
    std::vector<std::wstring> m_frameFiles;
};

#endif // SCREENRECORDER_H
