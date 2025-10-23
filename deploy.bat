@echo off
echo Copiando dependencias...

copy C:\opencv\build\x64\vc16\bin\opencv_world480.dll Release\
copy yolov8n.onnx Release\

C:\Qt\6.9.3\msvc2022_64\bin\windeployqt.exe Release\PTZTrackerPro.exe

echo.
echo Pronto! Execute: Release\PTZTrackerPro.exe
pause