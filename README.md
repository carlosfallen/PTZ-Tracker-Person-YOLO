# PTZ Tracker Pro v2.0 - YOLO Edition

Sistema profissional de rastreamento de pessoas com detecção YOLO de corpo inteiro.

## 🎯 Funcionalidades

✅ **Detecção YOLO** - YOLOv8 ONNX para corpo inteiro  
✅ **Interface Gráfica** - Qt6 moderna sem terminal  
✅ **Controle PTZ** - VISCA via serial  
✅ **Multi-thread** - Captura/processamento/render separados  
✅ **FPS Adaptativo** - Controle inteligente de performance  
✅ **Logs Integrados** - Painel de logs coloridos  
✅ **Threshold Ajustável** - Precisão configurável em tempo real  

## 📋 Requisitos

- Windows 10/11 64-bit
- Visual Studio 2022
- Qt 6.6.3+
- OpenCV 4.8.0+
- Modelo YOLO: yolov8n.onnx

## 🚀 Instalação

### 1. Instalar dependências

**Qt6:**
```
https://www.qt.io/download-open-source
→ Qt 6.6.3 + MSVC 2019 64-bit
```

**OpenCV:**
```
https://github.com/opencv/opencv/releases/download/4.8.0/opencv-4.8.0-windows.exe
→ Extrair para C:\opencv
→ Adicionar ao PATH: C:\opencv\build\x64\vc16\bin
```

**Modelo YOLO:**
```cmd
pip install ultralytics
python -c "from ultralytics import YOLO; YOLO('yolov8n.pt').export(format='onnx', imgsz=416)"
```

### 2. Compilar

```cmd
mkdir build && cd build

cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_PREFIX_PATH=C:/Qt/6.6.3/msvc2019_64 ^
  -DOpenCV_DIR=C:/opencv/build

cmake --build . --config Release
```

### 3. Executar

```cmd
cd Release
PTZTrackerPro.exe
```

## 🎮 Controles

**Fonte de Vídeo:**
- Webcam: Digite `0`, `1`, etc
- RTSP: `rtsp://ip:porta/stream`
- Arquivo: Selecione via botão

**PTZ:**
- Setas: Pan/Tilt
- +/-: Zoom
- Home: Posição inicial
- Menu VISCA: Configurações da câmera

**Precisão:**
- 0.1-0.3: Detecta tudo (muitos falsos positivos)
- 0.4-0.6: Balanceado (recomendado)
- 0.7-0.9: Apenas detecções muito confiáveis

## 📊 Performance

- **FPS**: 25-35 (CPU) / 60+ (GPU)
- **Latência**: < 50ms
- **Memória**: ~300MB

## 🔧 Troubleshooting

**Terminal aparece:**
- Verifique `WIN32` no CMakeLists.txt linha 39

**OpenCV não encontrado:**
- Confirme: `C:\opencv\build\OpenCVConfig.cmake` existe
- Use: `-DOpenCV_DIR=C:/opencv/build`

**Qt não encontrado:**
- Adicione ao PATH: `C:\Qt\6.6.3\msvc2019_64\bin`

**Modelo YOLO não carrega:**
- Coloque `yolov8n.onnx` na pasta do executável
- Verifique formato ONNX com: `imgsz=416`

## 📝 Licença

MIT License - Use livremente!
"# PTZ-Tracker-Person-YOLO" 
"# PTZ-Tracker-Person-YOLO" 
