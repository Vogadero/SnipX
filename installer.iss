; SnipX 安装程序脚本
; 使用 Inno Setup 编译
; 下载地址: https://jrsoftware.org/isinfo.php

#define MyAppName "SnipX"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "SnipX Team"
#define MyAppURL "https://github.com/yourusername/SnipX"
#define MyAppExeName "SnipX.exe"

[Setup]
; 应用程序信息
AppId={{8F3C4D2E-1A5B-4C7D-9E8F-2A3B4C5D6E7F}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=LICENSE.txt
InfoBeforeFile=README.md
OutputDir=release
OutputBaseFilename=SnipX-{#MyAppVersion}-Setup
SetupIconFile=res\icon.ico
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern

; 系统要求
MinVersion=6.1sp1
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

; 权限
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog

[Languages]
Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1; Check: not IsAdminInstallMode
Name: "autostart"; Description: "开机自动启动"; GroupDescription: "其他选项:"; Flags: unchecked

[Files]
Source: "bin\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "USER_MANUAL.md"; DestDir: "{app}"; DestName: "用户手册.md"; Flags: ignoreversion
Source: "CHANGELOG.md"; DestDir: "{app}"; DestName: "更新日志.md"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\用户手册"; Filename: "{app}\用户手册.md"
Name: "{group}\卸载 {#MyAppName}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Registry]
; 开机自启动
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "{#MyAppName}"; ValueData: """{app}\{#MyAppExeName}"""; Flags: uninsdeletevalue; Tasks: autostart

; 应用程序信息
Root: HKCU; Subkey: "Software\{#MyAppName}"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\{#MyAppName}\Settings"; Flags: uninsdeletekey

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{userappdata}\SnipX"

[Code]
// 检查是否已安装
function InitializeSetup(): Boolean;
var
  ResultCode: Integer;
  UninstallString: String;
begin
  Result := True;
  
  // 检查是否已安装
  if RegQueryStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#SetupSetting("AppId")}_is1', 'UninstallString', UninstallString) or
     RegQueryStringValue(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#SetupSetting("AppId")}_is1', 'UninstallString', UninstallString) then
  begin
    if MsgBox('检测到已安装 {#MyAppName}，是否卸载旧版本？', mbConfirmation, MB_YESNO) = IDYES then
    begin
      // 执行卸载
      Exec(RemoveQuotes(UninstallString), '/SILENT', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
      Result := True;
    end
    else
    begin
      Result := False;
    end;
  end;
end;

// 安装完成后的操作
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // 创建配置目录
    CreateDir(ExpandConstant('{userappdata}\SnipX'));
  end;
end;

// 卸载前的操作
function InitializeUninstall(): Boolean;
var
  ResultCode: Integer;
begin
  Result := True;
  
  // 询问是否保留配置
  if MsgBox('是否保留配置文件和截图？', mbConfirmation, MB_YESNO or MB_DEFBUTTON2) = IDNO then
  begin
    // 删除配置目录
    DelTree(ExpandConstant('{userappdata}\SnipX'), True, True, True);
  end;
end;

[CustomMessages]
chinesesimplified.LaunchProgram=启动 %1
english.LaunchProgram=Launch %1
