<div align="center">

# 🎯 PTZ Tracker Person YOLO

### Sistema Inteligente de Rastreamento Automático de Pessoas

[![C++](https://img.shields.io/badge/C++-17+-00599C?style=for-the-badge&logo=cplusplus)](https://isocpp.org/)
[![YOLO](https://img.shields.io/badge/YOLO-Detection-00FFFF?style=for-the-badge)](https://github.com/ultralytics/yolov5)
[![CMake](https://img.shields.io/badge/CMake-3.10+-064F8C?style=for-the-badge&logo=cmake)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

*Rastreamento em tempo real com controle PTZ via USB/UVC + Detecção YOLO*

[🚀 Começar](#-instalação) • [📖 Documentação](#-funcionalidades) • [⚙️ Configuração](#️-parâmetros-recomendados) • [🐛 Problemas](#-resolução-de-problemas)

</div>

---

## 📋 Índice

- [Visão Geral](#-visão-geral)
- [Tecnologias](#-tecnologias)
- [Funcionalidades](#-funcionalidades)
- [Arquitetura](#-arquitetura-do-sistema)
- [Instalação](#-instalação)
- [Uso](#-uso)
- [Configuração Avançada](#️-configuração-avançada)
- [Resolução de Problemas](#-resolução-de-problemas)
- [Roadmap](#-roadmap)
- [Contribuindo](#-contribuindo)

---

## 🌟 Visão Geral

**PTZ Tracker Person YOLO** é um sistema avançado de rastreamento automático que combina a precisão da detecção de objetos YOLO com controle inteligente de câmeras PTZ (Pan-Tilt-Zoom). Desenvolvido em C++ para máxima performance, o sistema detecta e acompanha pessoas em tempo real, mantendo-as centralizadas no enquadramento através de movimentos suaves e adaptativos.

### 🎬 Demonstração Visual
```
┌─────────────────────────────────────────┐
│  🎥 Câmera PTZ                          │
│                                         │
│     ┌───────────────────────┐          │
│     │   [👤]  ←Pessoa       │          │
│     │     ↓                 │          │
│     │  Rastreamento         │          │
│     │  Centralizado         │          │
│     └───────────────────────┘          │
│           ↓                             │
│     Pan: ◄─────►  Tilt: ▲              │
│                         ▼              │
└─────────────────────────────────────────┘
```

---

## 🛠 Tecnologias

<table>
<tr>
<td width="50%">

### Core
- **C++17+** - Performance e eficiência
- **CMake 3.10+** - Sistema de build moderno
- **YOLO** - Detecção de objetos em tempo real

</td>
<td width="50%">

### Dependências
- **ONNX Runtime** ou **TensorRT** - Inferência otimizada
- **Driver UVC** - Controle de câmera USB
- **OpenCV** *(opcional)* - Processamento de imagem

</td>
</tr>
</table>

---

## ✨ Funcionalidades

### 🎯 Detecção e Rastreamento
- ✅ Detecção da classe "person" em tempo real
- ✅ Cálculo automático do centroide da bounding-box
- ✅ Rastreamento suave com filtro de ruído
- ✅ Predição de movimento para antecipação

### 🎮 Controle PTZ Inteligente
- ✅ Movimentos suaves sem overshoot
- ✅ Velocidade adaptativa baseada na distância do alvo
- ✅ Zona morta (deadband) configurável para estabilidade
- ✅ Limites físicos respeitados automaticamente

### 🔄 Modos de Operação
- ✅ **Tracking Mode** - Acompanhamento ativo do alvo
- ✅ **Scan Mode** - Busca automática quando alvo é perdido
- ✅ **Standby Mode** - Posição de repouso configurável

### ⚡ Performance
- ✅ Baixa latência (< 100ms)
- ✅ Taxa de atualização: 15-30 FPS
- ✅ Consumo otimizado de recursos

---

## 🏗 Arquitetura do Sistema
```
┌─────────────────────────────────────────────────────────┐
│                     PTZ Tracker System                   │
└─────────────────────────────────────────────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        ▼                   ▼                   ▼
┌───────────────┐   ┌──────────────┐   ┌──────────────┐
│   Detector    │   │   Tracker    │   │ PTZ Control  │
│   (YOLO)      │──▶│   Logic      │──▶│   (UVC)      │
└───────────────┘   └──────────────┘   └──────────────┘
        │                   │                   │
        ▼                   ▼                   ▼
    Inferência        Centroide/PID        Pan/Tilt
    Bounding Box      Suavização           Comandos USB
    Confiança         Velocidade           Feedback
```

### 📂 Estrutura de Arquivos
```
PTZ-Tracker-Person-YOLO/
│
├── 📁 src/
│   ├── 🔍 detector.cpp/.h         # Interface YOLO
│   │   ├── Carregamento do modelo
│   │   ├── Pré-processamento
│   │   └── Inferência e pós-processamento
│   │
│   ├── 🎯 tracker.cpp/.h          # Lógica de rastreamento
│   │   ├── Cálculo de centroide
│   │   ├── Controle PID
│   │   ├── Filtro de suavização
│   │   └── Deadband e edge detection
│   │
│   ├── 🎮 ptz_controller.cpp/.h   # Controle PTZ
│   │   ├── Comandos UVC
│   │   ├── Gerenciamento de velocidade
│   │   ├── Limites físicos
│   │   └── Calibração automática
│   │
│   └── 🚀 main.cpp                # Loop principal
│       ├── Inicialização
│       ├── Captura de frames
│       └── Integração dos módulos
│
├── 📁 models/                     # Modelos YOLO
│   ├── yolov5s.onnx
│   └── coco.names
│
├── 📁 config/                     # Arquivos de configuração
│   ├── default_params.yaml
│   └── camera_profiles.json
│
├── 📄 CMakeLists.txt              # Configuração CMake
├── 📄 README.md                   # Este arquivo
└── 📄 LICENSE                     # Licença MIT
```

---

## 🚀 Instalação

### Pré-requisitos
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    cmake \
    build-essential \
    libopencv-dev \
    libonnxruntime-dev \
    v4l-utils

# Fedora/RHEL
sudo dnf install -y \
    cmake \
    gcc-c++ \
    opencv-devel \
    onnxruntime-devel \
    v4l-utils
```

### Compilação
```bash
# Clone o repositório
git clone https://github.com/carlosfallen/PTZ-Tracker-Person-YOLO.git
cd PTZ-Tracker-Person-YOLO

# Crie o diretório de build
mkdir build && cd build

# Configure e compile
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Instale (opcional)
sudo make install
```

### Verificação da Instalação
```bash
# Teste o executável
./PTZTrackerPersonYOLO --version

# Liste câmeras disponíveis
./PTZTrackerPersonYOLO --list-cameras

# Teste com webcam padrão
./PTZTrackerPersonYOLO --test --camera 0
```

---

## 💻 Uso

### 🎯 Modo Básico
```bash
./PTZTrackerPersonYOLO \
    --model ../models/yolov5s.onnx \
    --classes ../models/coco.names \
    --camera 0
```

### ⚙️ Modo Avançado
```bash
./PTZTrackerPersonYOLO \
    --model ../models/yolov5s.onnx \
    --classes ../models/coco.names \
    --camera /dev/video0 \
    --pan-speed 1.2 \
    --tilt-speed 1.0 \
    --deadband 0.03 \
    --edge-threshold 0.15 \
    --scan-mode true \
    --confidence 0.6 \
    --resolution 1280x720 \
    --fps 30 \
    --log-level debug
```

### 📊 Parâmetros de Linha de Comando

| Parâmetro | Tipo | Padrão | Descrição |
|-----------|------|--------|-----------|
| `--model` | string | - | **[Obrigatório]** Caminho para o modelo YOLO (.onnx) |
| `--classes` | string | - | **[Obrigatório]** Arquivo de classes (coco.names) |
| `--camera` | int/string | 0 | ID da câmera ou caminho do dispositivo |
| `--pan-speed` | float | 1.0 | Velocidade base de pan (0.5-3.0) |
| `--tilt-speed` | float | 1.0 | Velocidade base de tilt (0.5-3.0) |
| `--deadband` | float | 0.03 | Zona morta (0.01-0.10) |
| `--edge-threshold` | float | 0.15 | Limiar para aceleração nas bordas |
| `--scan-mode` | bool | true | Ativar busca automática |
| `--confidence` | float | 0.5 | Confiança mínima de detecção |
| `--resolution` | string | 640x480 | Resolução da câmera |
| `--fps` | int | 30 | Taxa de frames desejada |
| `--log-level` | string | info | Nível de log (debug/info/warn/error) |

---

## ⚙️ Configuração Avançada

### 🎚️ Parâmetros Recomendados

#### Para Ambientes Internos
```yaml
pan_speed_base: 1.0
pan_speed_max: 2.0
tilt_speed_base: 1.0
tilt_speed_max: 2.0
deadband: 0.03          # 3% do frame
edge_threshold: 0.15    # 15% da borda
approach_limit: 0.05    # 5% zona central
lost_timeout: 0.5       # segundos
confidence: 0.6
```

#### Para Ambientes Externos
```yaml
pan_speed_base: 1.5
pan_speed_max: 2.5
tilt_speed_base: 1.2
tilt_speed_max: 2.2
deadband: 0.05          # Maior estabilidade
edge_threshold: 0.20
approach_limit: 0.08
lost_timeout: 1.0
confidence: 0.7
```

### 📐 Calibração de Câmera
```bash
# Execute o assistente de calibração
./PTZTrackerPersonYOLO --calibrate --camera 0

# Salve o perfil
./PTZTrackerPersonYOLO --save-profile my_camera_profile
```

### 🔧 Arquivo de Configuração (config.yaml)
```yaml
system:
  log_level: info
  performance_mode: balanced  # low/balanced/high
  
camera:
  device: /dev/video0
  resolution: [1280, 720]
  fps: 30
  auto_exposure: true
  
detection:
  model_path: ../models/yolov5s.onnx
  classes_path: ../models/coco.names
  confidence_threshold: 0.6
  nms_threshold: 0.45
  target_class: person
  
tracking:
  deadband: 0.03
  edge_threshold: 0.15
  approach_limit: 0.05
  lost_timeout: 0.5
  smoothing_factor: 0.3
  
ptz:
  pan_speed_base: 1.0
  pan_speed_max: 2.0
  tilt_speed_base: 1.0
  tilt_speed_max: 2.0
  pan_limits: [-170, 170]
  tilt_limits: [-30, 90]
  acceleration: 0.5
  
scan:
  enabled: true
  pattern: horizontal  # horizontal/vertical/spiral
  speed: 0.8
  range: 180
```

---

## 🐛 Resolução de Problemas

### ⚠️ Problemas Comuns

<details>
<summary><b>❌ Alvo fica "saltando" (jittering)</b></summary>

**Causa:** Movimentos muito sensíveis

**Solução:**
```bash
# Aumente a zona morta
--deadband 0.05

# Ou ajuste o fator de suavização no código
smoothing_factor = 0.4  # Aumentar de 0.3 para 0.4
```
</details>

<details>
<summary><b>❌ Câmera se move demais/rápido</b></summary>

**Causa:** Velocidade excessiva

**Solução:**
```bash
# Reduza velocidades máximas
--pan-speed 0.8 \
--tilt-speed 0.8 \
--edge-threshold 0.20
```
</details>

<details>
<summary><b>❌ Sem resposta da câmera PTZ</b></summary>

**Causa:** Driver UVC ou permissões

**Solução:**
```bash
# Verifique dispositivo
v4l2-ctl --list-devices

# Teste controles PTZ
v4l2-ctl -d /dev/video0 --list-ctrls

# Ajuste permissões
sudo usermod -a -G video $USER
sudo chmod 666 /dev/video0

# Reinicie e teste
./PTZTrackerPersonYOLO --test
```
</details>

<details>
<summary><b>❌ Perda constante do alvo</b></summary>

**Causa:** Iluminação, modelo ou ângulo

**Solução:**
- ✓ Melhore iluminação do ambiente
- ✓ Ajuste ângulo da câmera
- ✓ Reduza threshold de confiança: `--confidence 0.4`
- ✓ Use modelo mais robusto (YOLOv8)
- ✓ Aumente taxa de frames: `--fps 60`
</details>

<details>
<summary><b>❌ Erro de compilação</b></summary>

**Solução:**
```bash
# Limpe build anterior
rm -rf build/
mkdir build && cd build

# Compile com verbose
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
make VERBOSE=1

# Verifique dependências
ldd ./PTZTrackerPersonYOLO
```
</details>

### 📊 Diagnóstico Avançado
```bash
# Ative logs detalhados
./PTZTrackerPersonYOLO \
    --log-level debug \
    --log-file debug.log

# Monitore performance
./PTZTrackerPersonYOLO --performance-monitor

# Teste individual de módulos
./PTZTrackerPersonYOLO --test-detector
./PTZTrackerPersonYOLO --test-tracker
./PTZTrackerPersonYOLO --test-ptz
```

---

## 🗺 Roadmap

### 📅 Próximas Versões

#### v2.0 - Q2 2025
- [ ] 🔍 **Zoom automático** baseado em tamanho da bounding-box
- [ ] 👥 **Multi-alvo** com priorização inteligente
- [ ] 🎯 **Predição de movimento** com Kalman Filter
- [ ] 📱 **API REST** para controle remoto

#### v2.5 - Q3 2025
- [ ] 🌐 **Suporte ONVIF** para câmeras IP
- [ ] 🖥️ **Interface gráfica** (Qt/ImGui)
- [ ] 📊 **Dashboard** de métricas em tempo real
- [ ] ☁️ **Integração cloud** para analytics

#### v3.0 - Q4 2025
- [ ] 🤖 **IA adaptativa** - aprendizado de padrões
- [ ] 🎭 **Reconhecimento facial** (opcional)
- [ ] 🔊 **Rastreamento por áudio** (complementar)
- [ ] 🌍 **Localização 3D** com múltiplas câmeras

### 💡 Ideias em Avaliação
- Suporte a modelos customizados (treinar para objetos específicos)
- Modo "privacy" com detecção sem gravar imagens
- Integração com sistemas de automação (Home Assistant)
- Suporte a GPUs AMD/Intel
- Versão mobile (Android/iOS)

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Veja como você pode ajudar:

### 🔧 Como Contribuir

1. **Fork** o projeto
2. **Crie** uma branch para sua feature (`git checkout -b feature/NovaFuncionalidade`)
3. **Commit** suas mudanças (`git commit -m 'Adiciona nova funcionalidade'`)
4. **Push** para a branch (`git push origin feature/NovaFuncionalidade`)
5. **Abra** um Pull Request

### 📝 Diretrizes

- Siga o estilo de código C++17
- Adicione testes para novas funcionalidades
- Atualize a documentação
- Use commits semânticos (feat, fix, docs, refactor, etc.)

### 🐛 Reportando Bugs

Use o template de issue e inclua:
- Sistema operacional e versão
- Modelo da câmera PTZ
- Logs de erro completos
- Passos para reproduzir

---

## 📄 Licença

Este projeto está licenciado sob a **MIT License** - veja o arquivo [LICENSE](LICENSE) para detalhes.

---

## 🙏 Agradecimentos

- [Ultralytics YOLO](https://github.com/ultralytics/yolov5) - Framework de detecção
- [ONNX Runtime](https://onnxruntime.ai/) - Engine de inferência
- [V4L2](https://www.kernel.org/doc/html/latest/userspace-api/media/v4l/v4l2.html) - Driver UVC Linux

---

## 📞 Contato

**Carlos Fallen** - [@carlosfallen](https://github.com/carlosfallen)

🔗 **Projeto:** [https://github.com/carlosfallen/PTZ-Tracker-Person-YOLO](https://github.com/carlosfallen/PTZ-Tracker-Person-YOLO)

---

<div align="center">

### ⭐ Se este projeto foi útil, considere dar uma estrela!

[![GitHub stars](https://img.shields.io/github/stars/carlosfallen/PTZ-Tracker-Person-YOLO?style=social)](https://github.com/carlosfallen/PTZ-Tracker-Person-YOLO/stargazers)

**Feito com ❤️ e C++**

</div>