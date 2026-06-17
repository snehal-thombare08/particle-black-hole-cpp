# 🌌 Particle Black Hole

A real-time **Particle Black Hole simulation** built with **C++ and SFML 3.0**, featuring 8000 particles forming a stunning accretion disk under gravitational attraction.

![Particle Black Hole Preview](https://raw.githubusercontent.com/snehal-thombare08/particle-black-hole-cpp/main/Screenshot%202026-06-17%20134045.png)

## ✨ Features

- **8000 Particles** — massive particle system with real-time gravitational physics
- **Accretion Disk** — particles naturally form orbital rings around the black hole
- **Drag Black Hole** — move the singularity with your mouse, particles follow in real-time
- **Nova Burst** — explosive scatter sends all particles flying outward
- **Repulse Mode** — right-click to push particles away from the black hole
- **Adjustable Mass** — increase or decrease gravitational pull on the fly
- **Speed-based Color** — cool blue far away, hot white/orange near the event horizon
- **Smooth Trail Effect** — particles leave fading trails as they spiral inward

## screenshot
https://raw.githubusercontent.com/snehal-thombare08/particle-black-hole-cpp/main/Screenshot%202026-06-17%20134045.png

## 🛠️ Built With

- **C++17**
- **SFML 3.0.2**
- **CMake** + **MinGW**

## ▶️ How to Run

1. Download `BlackHole-v1.0-Windows.zip` from [Releases](../../releases)
2. Extract all files to the same folder
3. Run `BlackHole.exe`

> Requires Windows. No install needed — just extract and run.

## 🎮 Controls

| Key / Mouse | Action |
|---|---|
| **LMB Drag** | Move the black hole |
| **RMB Hold** | Repulse particles outward |
| **Space** | Nova burst — scatter all particles |
| **+** | Increase black hole mass |
| **-** | Decrease black hole mass |
| **R** | Reset simulation |
| **Esc** | Quit |

## ⚙️ Build from Source

```bash
git clone https://github.com/snehal-thombare08/particle-black-hole-cpp.git
cd particle-black-hole-cpp
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -G "MinGW Makefiles"
mingw32-make
```

## 📁 Project Structure

```
particle-black-hole-cpp/
├── src/
│   └── main.cpp
├── CMakeLists.txt
└── README.md
```

---

Part of a C++ graphics & simulation portfolio built with SFML 3.0.
