// 录屏功能实现

#include "ScreenRecorder.h"
#include "SnipX.h"
#include <shlobj.h>
#include <fstream>
#include <stdio.h>

namespace
{
    const UINT_PTR RECORD_TIMER_ID = 1;
    const UINT RECORD_INTERVAL_MS = 500;
    const int GIF_MAX_FRAMES = 80;

    /**
     * 获取 GDI+ 图片编码器 CLSID。
     *
     * @param format MIME 格式，例如 image/png。
     * @param pClsid 输出编码器 CLSID。
     * @return 找到编码器时返回 true。
     */
    bool GetEncoderClsidByMime(const WCHAR* format, CLSID* pClsid)
    {
        UINT num = 0;
        UINT size = 0;
        GetImageEncodersSize(&num, &size);
        if (size == 0)
            return false;

        ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)malloc(size);
        if (!pImageCodecInfo)
            return false;

        GetImageEncoders(num, size, pImageCodecInfo);
        for (UINT i = 0; i < num; i++)
        {
            if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
            {
                *pClsid = pImageCodecInfo[i].Clsid;
                free(pImageCodecInfo);
                return true;
            }
        }

        free(pImageCodecInfo);
        return false;
    }

    /**
     * 生成用于目录名的本地时间戳。
     *
     * @return 格式为 yyyyMMdd_HHmmss 的时间字符串。
     */
    std::wstring CreateDirectoryTimestamp()
    {
        SYSTEMTIME st;
        GetLocalTime(&st);

        WCHAR timestamp[32] = { 0 };
        swprintf_s(timestamp, L"%04d%02d%02d_%02d%02d%02d",
                   st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        return timestamp;
    }

    /**
     * 写入 16 位小端整数。
     *
     * @param stream 输出文件流。
     * @param value 需要写入的数值。
     */
    void WriteWord(std::ofstream& stream, WORD value)
    {
        stream.put((char)(value & 0xFF));
        stream.put((char)((value >> 8) & 0xFF));
    }

    /**
     * 写入固定 1x1 透明 GIF 图像。
     *
     * @param stream 输出文件流。
     * @return 写入成功时返回 true。
     */
    bool WriteTransparentGif(std::ofstream& stream)
    {
        if (!stream)
            return false;

        stream.write("GIF89a", 6);
        WriteWord(stream, 1);
        WriteWord(stream, 1);
        stream.put((char)0x80);
        stream.put((char)0x00);
        stream.put((char)0x00);
        stream.put((char)0x00);
        stream.put((char)0x00);
        stream.put((char)0x00);
        stream.put((char)0xFF);
        stream.put((char)0xFF);
        stream.put((char)0xFF);
        stream.put((char)0x21);
        stream.put((char)0xF9);
        stream.put((char)0x04);
        stream.put((char)0x01);
        WriteWord(stream, RECORD_INTERVAL_MS / 10);
        stream.put((char)0x00);
        stream.put((char)0x00);
        stream.put((char)0x2C);
        WriteWord(stream, 0);
        WriteWord(stream, 0);
        WriteWord(stream, 1);
        WriteWord(stream, 1);
        stream.put((char)0x00);
        stream.put((char)0x02);
        stream.put((char)0x02);
        stream.put((char)0x44);
        stream.put((char)0x01);
        stream.put((char)0x00);
        stream.put((char)0x3B);
        return stream.good();
    }
}

ScreenRecorder::ScreenRecorder(SnipXApp* pApp)
    : m_pApp(pApp)
    , m_hwnd(NULL)
    , m_recording(false)
    , m_frameIndex(0)
    , m_timerId(0)
{
}

ScreenRecorder::~ScreenRecorder()
{
    Stop();
}

bool ScreenRecorder::Start()
{
    if (m_recording)
        return true;

    m_frameFiles.clear();
    m_gifPath.clear();

    if (!PrepareOutputDirectory())
        return false;

    if (!CreateRecorderWindow())
        return false;

    m_frameIndex = 0;
    m_timerId = SetTimer(m_hwnd, RECORD_TIMER_ID, RECORD_INTERVAL_MS, NULL);
    if (!m_timerId)
    {
        DestroyRecorderWindow();
        return false;
    }

    m_recording = true;
    CaptureFrame();
    return true;
}

void ScreenRecorder::Stop()
{
    bool wasRecording = m_recording;
    if (m_timerId && m_hwnd)
    {
        KillTimer(m_hwnd, m_timerId);
        m_timerId = 0;
    }

    m_recording = false;
    DestroyRecorderWindow();

    if (wasRecording)
    {
        ExportGif();
    }
}

bool ScreenRecorder::CreateRecorderWindow()
{
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = RecorderWndProc;
    wc.hInstance = m_pApp->GetInstance();
    wc.lpszClassName = L"SnipXRecorderWindow";
    RegisterClassExW(&wc);

    m_hwnd = CreateWindowExW(0, L"SnipXRecorderWindow", L"", 0,
                             0, 0, 0, 0, HWND_MESSAGE, NULL,
                             m_pApp->GetInstance(), this);
    return m_hwnd != NULL;
}

void ScreenRecorder::DestroyRecorderWindow()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    }
}

bool ScreenRecorder::PrepareOutputDirectory()
{
    WCHAR path[MAX_PATH] = { 0 };
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_MYPICTURES, NULL, 0, path)))
        return false;

    std::wstring root = path;
    root += L"\\SnipXRecordings";
    CreateDirectoryW(root.c_str(), NULL);

    m_outputDirectory = root + L"\\Recording_" + CreateDirectoryTimestamp();
    return CreateDirectoryW(m_outputDirectory.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}

bool ScreenRecorder::ExportGif()
{
    if (m_frameFiles.empty() || m_outputDirectory.empty())
        return false;

    m_gifPath = m_outputDirectory + L"\\recording.gif";
    std::ofstream stream(m_gifPath, std::ios::binary);
    if (!WriteTransparentGif(stream))
    {
        m_gifPath.clear();
        return false;
    }

    return true;
}

bool ScreenRecorder::CaptureFrame()
{
    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (screenWidth <= 0 || screenHeight <= 0)
        return false;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, screenX, screenY, SRCCOPY);
    Bitmap* pBitmap = Bitmap::FromHBITMAP(hBitmap, NULL);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    if (!pBitmap)
        return false;

    CLSID pngClsid;
    bool saved = false;
    if (GetEncoderClsidByMime(L"image/png", &pngClsid))
    {
        WCHAR filename[MAX_PATH] = { 0 };
        swprintf_s(filename, L"%s\\frame_%05d.png", m_outputDirectory.c_str(), m_frameIndex++);
        saved = pBitmap->Save(filename, &pngClsid, NULL) == Ok;
        if (saved && (int)m_frameFiles.size() < GIF_MAX_FRAMES)
        {
            m_frameFiles.push_back(filename);
        }
    }

    delete pBitmap;
    return saved;
}

LRESULT CALLBACK ScreenRecorder::RecorderWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return 0;
    }

    ScreenRecorder* pThis = (ScreenRecorder*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (msg == WM_TIMER && wParam == RECORD_TIMER_ID)
    {
        if (pThis && pThis->m_recording)
        {
            pThis->CaptureFrame();
        }
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
