cmake_minimum_required(VERSION 3.15)
project(SnipX VERSION 1.1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 平台特定设置
if(WIN32)
    # 设置为Windows GUI应用
    set(CMAKE_WIN32_EXECUTABLE ON)
    add_compile_definitions(SNIPX_PLATFORM_WINDOWS=1)
    
    # 优化选项
    if(MSVC)
        # MSVC编译器优化；静态链接运行库，避免发布的便携版依赖目标机器安装 VC++ Runtime。
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        add_compile_options(/O2 /GL /EHsc /W3)
        add_link_options(/LTCG /OPT:REF /OPT:ICF /SUBSYSTEM:WINDOWS)
    else()
        # MinGW编译器优化
        add_compile_options(-O3 -s -static)
        add_link_options(-mwindows -static)
    endif()
elseif(APPLE)
    add_compile_definitions(SNIPX_PLATFORM_MACOS=1)
    message(STATUS "SnipX macOS target: core platform layer is enabled; Win32 UI modules require future native UI port.")
elseif(UNIX)
    add_compile_definitions(SNIPX_PLATFORM_LINUX=1)
    message(STATUS "SnipX Linux target: core platform layer is enabled; Win32 UI modules require future native UI port.")
endif()

# 源文件
set(SOURCES
    src/main.cpp
    src/SnipX.cpp
    src/Config.cpp
    src/HotkeyManager.cpp
    src/TrayIcon.cpp
    src/ScreenCapture.cpp
    src/ScreenRecorder.cpp
    src/Localization.cpp
    src/Platform.cpp
    src/Logger.cpp
    src/Editor.cpp
    src/ColorPicker.cpp
    src/PinWindow.cpp
    src/SettingsDialog.cpp
)

# 头文件
set(HEADERS
    src/SnipX.h
    src/Config.h
    src/HotkeyManager.h
    src/TrayIcon.h
    src/ScreenCapture.h
    src/ScreenRecorder.h
    src/Localization.h
    src/Platform.h
    src/Logger.h
    src/MemoryChecker.h
    src/PerformanceMonitor.h
    src/UpdateChecker.h
    src/Version.h
    src/Editor.h
    src/ColorPicker.h
    src/PinWindow.h
    src/SettingsDialog.h
    src/resource.h
)

# 资源文件
if(WIN32)
    set(RESOURCES
        res/SnipX.rc
    )
endif()

# 创建可执行文件
add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接库
if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        gdiplus
        shell32
        user32
        gdi32
        ole32
        dwmapi
        shlwapi
        advapi32
        comctl32

        comdlg32
        wininet
        psapi
    )
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
