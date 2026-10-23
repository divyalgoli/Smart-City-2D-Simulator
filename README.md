# 🏙️ SMART CITY — Interactive 2D City Simulator

A **Computer Graphics (CGR) mini project** built in **C using Turbo C/C++ BGI graphics**. SMART CITY presents a small animated city with moving vehicles, a working traffic signal, a pedestrian crossing, and switchable day/night scenes.

The project demonstrates core 2D computer-graphics concepts through a keyboard-controlled simulation.

## 📸 Output Screenshots

### ☀️ Day Mode
<img width="1360" height="765" alt="Screenshot 2026-10-23 201049" src="https://github.com/user-attachments/assets/af2d3e58-c8c8-4c3f-89de-db11e0aaf045" />

### 🌙 Night Mode
<img width="1360" height="768" alt="Screenshot 2026-10-23 201113" src="https://github.com/user-attachments/assets/47ab129f-e18c-4caf-bcc0-7e82306edbbd" />


## ✨ Features

- **Day and night modes** with different sky, building, road, grass, and window colours.
- **Animated city environment** with buildings, trees, clouds, a sun, a moon, stars, and streetlights.
- **Moving vehicles** in two directions, including sedan and van designs.
- **Animated wheels** with rotating spokes.
- **Automatic traffic signal** cycling through green, yellow, and red.
- **Traffic rules** that make vehicles stop at the stop line during red/yellow signals.
- **Pedestrian crossing** that activates during the red-light phase when the crossing is clear.
- **Safe vehicle spacing** to help prevent cars in the same lane from overlapping.
- **Adjustable simulation speed** and pause/resume controls.
- **Welcome and help screens** with keyboard instructions.
- **2D transformations** such as translation, rotation, and scaling.
- **Flicker reduction** using vertical-retrace synchronization where supported.

## 🎮 Keyboard Controls

| Key | Action |
|---|---|
| `D` | Switch to Day Mode |
| `N` | Switch to Night Mode |
| `+` or `=` | Increase vehicle speed |
| `-` or `_` | Decrease vehicle speed |
| `P` | Pause / resume simulation |
| `R` | Reset the simulation |
| `H` | Open the help screen |
| `ESC` | Exit the program |

## 🧰 Tech Stack

- **Language:** C
- **Graphics library:** BGI (`graphics.h`)
- **Target environment:** Turbo C / Turbo C++ 3.0
- **Graphics mode:** VGA high-resolution mode (`VGAHI`), approximately 640 × 480 pixels, 16 colours
- **Headers used:** `graphics.h`, `conio.h`, `dos.h`, `math.h`, `stdio.h`, `stdlib.h`, `string.h`

## 🚀 How to Run

This project uses the legacy BGI graphics library, so it is intended for a Turbo C/Turbo C++ environment that supports `graphics.h`.

1. Download or clone this repository.
2. Open `SMARTCTY.C` in Turbo C / Turbo C++.
3. Confirm that the BGI driver file `EGAVGA.BGI` exists in your BGI folder.
4. Update the `BGI_PATH` constant near the top of the source file if your BGI folder is in a different location.
5. Compile and run the program.

The source currently uses this path:

```c
#define BGI_PATH "C:\\TURBOC3\\BGI"
```

Change it to match your local setup. For example, if your BGI files are stored in another folder, use that folder's path instead.

> **Compatibility note:** This is a DOS-era BGI graphics project. Modern compilers such as standard GCC do not normally provide `graphics.h` or the Turbo C BGI runtime by default. Use a compatible Turbo C/Turbo C++ setup or a suitable BGI-compatible environment.

## 🧠 Computer Graphics Concepts Demonstrated

| Concept | Where it appears |
|---|---|
| **Translation** | Vehicles move across the road; clouds move across the sky. |
| **Rotation** | Wheel spokes and sun rays rotate during animation. |
| **Scaling** | Clouds are drawn at different sizes. |
| **Basic drawing primitives** | Lines, rectangles, pixels, circles, ellipses, and filled polygons create the scene. |
| **Colour and fill styles** | Day/night themes, building windows, road, grass, and signal lamps. |
| **Animation loop** | The simulation updates object positions and redraws dynamic elements frame by frame. |
| **Keyboard interaction** | Users can change modes, speed, pause state, reset, view help, or exit. |
| **Simple simulation logic** | Signal timing, vehicle stop lines, lane spacing, and pedestrian-crossing conditions. |

## 🚦 Traffic Simulation Logic

- The traffic signal cycles through **green → yellow → red → green**.
- During the yellow and red phases, vehicles are prevented from moving beyond their configured stop line.
- The pedestrian begins crossing during the red phase when the crossing is clear.
- Vehicles maintain a minimum gap from other vehicles in the same lane.
- The simulation can be paused, resumed, reset, or run at a different speed using the keyboard.

## 📁 Repository Structure

```text
SMART-CITY/
├── SMARTCTY.C
├── README.md
└── assets/
    ├── day-mode.png
    └── night-mode.png
```

## 🎓 Project Information

- **Project:** SMART CITY — Interactive 2D City Simulator
- **Subject:** Computer Graphics (CGR)
- **Course:** Diploma in Computer Technology
- **Developed by:** Divyal
- **Institute:** S.E.S. Polytechnic, Solapur

## 🔮 Possible Future Improvements

- Add a score or event log for traffic-rule violations.
- Add more vehicle types and configurable traffic density.
- Add a pedestrian signal and clearer crossing-state indicators.
- Add a settings screen for simulation parameters.
- Port the simulator to a modern graphics framework for easier cross-platform use.

## 📄 License

No license has been specified yet. If you plan to let others reuse, modify, or distribute this project, consider adding an appropriate open-source license.

---

**SMART CITY** is a small educational project exploring 2D graphics, animation, keyboard input, and basic traffic-simulation logic in C.
