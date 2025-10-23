[Setup]
AppName=PTZ Tracker Pro
AppVersion=2.0
DefaultDirName={autopf}\PTZTrackerPro
DefaultGroupName=PTZ Tracker Pro
OutputDir=.
OutputBaseFilename=PTZTrackerPro_Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Files]
Source: "Release\*"; DestDir: "{app}"; Flags: recursesubdirs ignoreversion

[Icons]
Name: "{autoprograms}\PTZ Tracker Pro"; Filename: "{app}\PTZTrackerPro.exe"
Name: "{autodesktop}\PTZ Tracker Pro"; Filename: "{app}\PTZTrackerPro.exe"

[Run]
Filename: "{app}\PTZTrackerPro.exe"; Description: "Executar PTZ Tracker Pro"; Flags: postinstall nowait skipifsilent