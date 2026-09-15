# 🎮 Tetris on Arduino with OLED Display

A compact, fully playable Tetris game built with **Arduino** and a **0.96" SSD1306 OLED display**, developed entirely with **PlatformIO**. This project is a great example of squeezing a classic game into a microcontroller with limited memory and processing power, while keeping the code clean and readable.

## 📸 Overview

This project implements the classic **Tetris** game on an Arduino board (Uno/Nano/etc.) using:
- A **128×64 SSD1306 OLED** display (I2C)
- **4 push buttons** for control
- Efficient fixed-size data structures (no dynamic memory)
- Simple gravity + collision + line-clear logic

The board is 10 × 20 cells (standard Tetris grid), with a small cell size (3 px) so it fits nicely beside the display margins.

## ✨ Features

- ✅ All 7 classic tetrominoes: **I, O, T, L, J, S, Z**
- ✅ Full rotation support per piece (4 rotations for I/T/L/J, 2 for S/Z, 1 for O)
- ✅ **Wall kick** — tries shifting the piece left/right when rotating near a wall
- ✅ Line clearing with board shifting
- ✅ Automatic gravity-based falling
- ✅ Soft drop (fast fall) using the Down button
- ✅ Game Over detection
- ✅ Start / Game Over screens
- ✅ Lightweight — no dynamic allocation, no external game libraries
- ✅ Clean PlatformIO project structure with all dependencies declared in `platformio.ini`

## 🧰 Hardware Requirements

| Component | Quantity |
|---|---|
| Arduino Uno / Nano / similar | 1 |
| 0.96" OLED Display (SSD1306, I2C, 128×64) | 1 |
| Push buttons (momentary) | 4 |
| Jumper wires | — |
| Breadboard (optional) | 1 |

## 🔌 Wiring

### OLED (I2C)

| OLED Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| SCL | A5 (Uno) |
| SDA | A4 (Uno) |

> On other boards (Leonardo, Mega, ESP), use the corresponding SDA/SCL pins.

### Buttons

All buttons use the internal pull-up (`INPUT_PULLUP`), so they are wired **between the pin and GND**.

| Function | Arduino Pin |
|---|---|
| Move Left | D2 |
| Move Right | D3 |
| Soft Drop (Down) | D4 |
| Rotate / Start | D5 |

## 🎮 Controls

| Button | Action |
|---|---|
| **Left** | Move piece left |
| **Right** | Move piece right |
| **Down** | Fast drop (soft drop) |
| **Rotate** | Rotate the piece / Start / Restart the game |

## 📁 Project Structure

```
Arduino-OLED-Tetris/
├── src/
│ └── main.cpp # Main game source code
├── platformio.ini # PlatformIO configuration & dependencies
├── README.md
└── LICENSE
```

## 📚 Dependencies

All libraries are declared in `platformio.ini` and are installed **automatically** by PlatformIO when you first build the project. No manual installation is required.

```ini
lib_deps =
adafruit/Adafruit GFX Library
adafruit/Adafruit SSD1306
```

- **Adafruit GFX Library** — low-level graphics primitives
- **Adafruit SSD1306** — driver for the 0.96" OLED display
- **Wire** — built into the Arduino framework, no extra install needed

## 🚀 Getting Started

### 1. Prerequisites

Install one of the following:

- **PlatformIO IDE** (VS Code extension) — recommended
→ https://platformio.org/install/ide?install=vscode

- **PlatformIO Core (CLI)** — for terminal users
```bash
pip install -U platformio
```

### 2. Clone the repository

```bash
git clone https://github.com/ifreake/Arduino-OLED-Tetris.git
cd Arduino-OLED-Tetris
```

### 3. Open the project in PlatformIO

- **VS Code:** `File → Open Folder…` and select the cloned folder.
PlatformIO will automatically detect `platformio.ini` and set up the project.

- **CLI:** simply `cd` into the project folder (already done in step 2).

### 4. Configure your board (if needed)

Open `platformio.ini`. The default target is an **Arduino Uno**:

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 9600

lib_deps =
adafruit/Adafruit GFX Library
adafruit/Adafruit SSD1306
```

If you're using a different board, change the `env` / `board` line to match your hardware. Examples:

| Board | `board =` value |
|---|---|
| Arduino Uno | `uno` |
| Arduino Nano (ATmega328P) | `nanoatmega328` |
| Arduino Mega 2560 | `megaatmega2560` |
| Arduino Leonardo | `leonardo` |

### 5. Build & Upload

**Via VS Code:**
- Click the **✓ (Build)** button in the PlatformIO toolbar
- Click the **→ (Upload)** button

**Via CLI:**

```bash
# Build the firmware
pio run

# Upload to the connected board
pio run --target upload

# Open the serial monitor (optional)
pio device monitor
```

### 6. Play!

After uploading, you'll see the **TETRIS** start screen on the OLED.
Press the **Rotate** button to start playing.

## ⚙️ Customization

You can easily tweak the game feel by changing these constants inside `src/main.cpp`:

```cpp
unsigned long fallDelay = 500; // gravity speed (ms)
const unsigned long fastFallDelay = 70; // soft-drop speed (ms)
const unsigned long moveDelay = 120; // horizontal move repeat rate (ms)

#define CELL_SIZE 3 // size of each block in pixels
#define BOARD_X 48 // board position on the OLED
#define BOARD_Y 2
```

Increasing `fallDelay` makes the game easier; lowering it makes it faster.

## 🧠 Code Structure

| Section | Responsibility |
|---|---|
| `PIECE_*` arrays | Stores shape coordinates for every tetromino and rotation |
| `checkCollision()` | Detects wall / floor / block collisions |
| `createNewPiece()` | Randomly spawns a new piece and checks for Game Over |
| `lockPiece()` | Commits the current piece to the board |
| `clearLines()` | Removes full rows and shifts the board down |
| `rotatePiece()` | Applies rotation with a simple wall-kick fallback |
| `drawGame()` | Renders the border, locked blocks, and the current piece |
| `readButtons()` | Handles debouncing, timing, and input actions |

All piece shapes are stored as **static `const Point` arrays**, which keeps the memory footprint low and predictable — ideal for AVR-based Arduinos.

## 🛠️ Built With

- [PlatformIO](https://platformio.org/) — cross-platform build system & IDE
- [Arduino Framework](https://www.arduino.cc/) — hardware abstraction layer
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) — graphics core
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) — OLED driver

## 📝 Notes & Limitations

- Score tracking is **not** implemented — focus is on gameplay mechanics.
- Only simple wall kicks are supported (no SRS-style rotation offsets).
- The OLED refresh rate is limited by the I2C bus; the render loop only draws when needed.
- Designed for **AVR-based Arduinos** (Uno, Nano, etc.) but should work on most 5V Arduino-compatible boards.
- Flash usage is low enough to also fit on smaller ATmega328P boards without modification.

## 🖼️ Screenshots

> *(Add photos or a short GIF of your build here — a video really sells the project!)*

```
## 🖼️ Screenshots

*(Screenshots coming soon — meanwhile, here's what you'll see on the OLED:)*

**Start Screen:**
┌────────────────────────┐
│        TETRIS          │
│       OLED 0.96        │
│    PRESS ROTATE        │
└────────────────────────┘

**In-Game:**
┌────────────────────────┐
│  ┌────┐                │
│  │ ▓▓ │                │
│  │ ▓▓ │                │
│  └────┘                │
└────────────────────────┘

**Game Over:**
┌────────────────────────┐
│         GAME           │
│         OVER           │
│    PRESS ROTATE        │
└────────────────────────┘
```

## 🤝 Contributing

Pull requests, suggestions, and improvements are welcome!
If you find a bug or want a new feature (scoring, levels, next-piece preview, sound, etc.), feel free to open an issue.

## 📄 License

This project is released under the **MIT License** — see the [LICENSE](LICENSE) file for details. Free to use, modify, and share.

## 🙌 Acknowledgments

- PlatformIO team for the excellent build system
- Adafruit for the **GFX** and **SSD1306** libraries
- The Arduino community for endless inspiration
- Alexey Pajitnov for creating Tetris in the first place 🎉

---

**Enjoy the game! 🕹️**

---
