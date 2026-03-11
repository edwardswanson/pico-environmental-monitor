# Pico Environmental Monitor (CS Capstone)

## Overview

This project is an embedded systems capstone built on the **Raspberry Pi Pico (RP2040)** using **bare-metal C** and the official **Pico SDK**. The system reads temperature and humidity data from a DHT20 sensor and displays it in real time on a 16×2 LCD screen over **I²C**. A WS2812 LED strip provides a color-coded visual temperature indicator, and an array of individual LEDs displays humidity level at a glance.

The project emphasizes low-level hardware interaction, protocol understanding, and embedded debugging rather than reliance on high-level frameworks.

---

## Project Goals

This capstone was designed to demonstrate:

- Comfort working close to hardware
- Understanding of embedded communication protocols (I²C, PIO)
- Ability to debug real hardware/software interactions
- Clean, modular C code in a constrained environment
- Collaborative embedded development using Git branching and pull requests

---

## Hardware

- Raspberry Pi Pico (RP2040)
- DHT20 (AHT20) temperature & humidity sensor (I²C)
- 16×2 LCD with I²C backpack (PCF8574)
- WS2812 LED strip (8 LEDs)
- 6× individual LEDs
- Breadboard and jumper wires

### Wiring

| Component | Pin | Pico Pin | Notes |
|-----------|-----|----------|-------|
| LCD VCC | VCC | Pin 40 (VBUS) | 5V |
| LCD GND | GND | Any GND | |
| LCD SDA | SDA | Pin 6 (GPIO 4) | Shared I2C bus |
| LCD SCL | SCL | Pin 7 (GPIO 5) | Shared I2C bus |
| DHT20 VCC | VCC | Pin 36 (3V3 OUT) | 3.3V |
| DHT20 GND | GND | Any GND | |
| DHT20 SDA | SDA | Pin 6 (GPIO 4) | Shared with LCD |
| DHT20 SCL | SCL | Pin 7 (GPIO 5) | Shared with LCD |
| LED Strip VCC | VCC | Pin 40 (VBUS) | 5V |
| LED Strip GND | GND | Any GND | |
| LED Strip DIN | DIN | Pin 4 (GPIO 2) | Data signal |
| Individual LEDs (x6) | + | GPIO 10–15 | One per LED |
| Individual LEDs (x6) | - | GND rail | Via breadboard |

---

## Features

- Real-time temperature and humidity sensing via DHT20 over I²C
- LCD display showing temperature (Celsius or Fahrenheit) and humidity
- WS2812 LED strip with two display patterns:
  - **Pattern 1:** All LEDs lit in a single color based on temperature
  - **Pattern 2:** Progressive fill based on temperature range
- Individual LED array displaying humidity level
- USB serial command interface (`picocmd.py`) for runtime control and mock sensor testing

---

## Software & Tools

- C (C11)
- Raspberry Pi Pico SDK (v2.2.0)
- ARM GCC toolchain
- CMake
- PIO (Programmable I/O) for WS2812 LED strip
- Python 3 (for `picocmd.py` serial shell)
- Git / GitHub for version control and collaboration

---

## Project Structure

```text
.
├── src/
│   ├── main.c                        # Application entry point
│   ├── i2c_scan.c                    # I2C bus scanning utility
│   ├── drivers/
│   │   ├── dht20.c / .h              # DHT20 temperature & humidity sensor driver
│   │   ├── lcd_pcf8574.c / .h        # LCD driver (PCF8574 I2C backpack)
│   │   ├── led.c / .h                # Individual LED driver
│   │   ├── led_strip.c / .h          # WS2812 LED strip driver
│   │   └── ws2812.pio                # PIO program for WS2812 protocol
│   ├── app/
│   │   ├── ui.c / .h                 # UI layer (LCD, LED array, LED strip)
│   │   └── sensor_task.c / .h        # Sensor reading and mock sensor logic
│   └── interfaces/
│       ├── command_interface.c / .h  # Serial command dispatcher
│       ├── commands.c                # Command handler definitions
│       └── parse.c / .h             # Command line parser
├── scripts/
│   ├── picocmd.py                    # Interactive serial command shell
│   ├── quotes.py                     # Quit quotes for picocmd
│   └── deploy.sh                    # Build and flash script (requires picotool)
├── CMakeLists.txt
└── README.md
```

---

## Getting Started

### Prerequisites

- [Visual Studio Code](https://code.visualstudio.com/)
- [Raspberry Pi Pico VS Code Extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico) — automatically installs the Pico SDK and toolchain
- [Python 3](https://www.python.org/downloads/)
- Git

**Windows users:** WSL (Windows Subsystem for Linux) is strongly recommended. Install via PowerShell as Administrator:
```sh
wsl --install
```
WSL users will also need [usbipd-win](https://github.com/dorssel/usbipd-win/releases) to share USB devices with WSL.

### Clone the Repository

```sh
git clone https://github.com/edwardswanson/pico-environmental-monitor
cd pico-environmental-monitor
```

### Build the Firmware

Open the project folder in VS Code. The Raspberry Pi Pico extension will detect the project automatically. Click **Compile** in the bottom status bar, or run:

```
Ctrl+Shift+P → "Raspberry Pi Pico: Compile Project"
```

The output file will be at `build/lcd_demo.uf2`.

---

## Flashing the Firmware

### Method 1: Drag and Drop (All Platforms)

1. Hold the **BOOTSEL** button on the Pico
2. While holding, connect via USB
3. Release BOOTSEL — the Pico appears as a drive named **RPI-RP2**
4. Copy `build/lcd_demo.uf2` to the drive
5. The Pico reboots automatically and starts running

### Method 2: deploy.sh (macOS / Linux / WSL)

Requires [picotool](https://github.com/raspberrypi/picotool) to be installed.

```sh
./scripts/deploy.sh
```

This script builds the project and flashes the firmware automatically using `picotool`.

---

## Serial Command Interface

The device exposes a serial command interface over USB. Use the included `picocmd.py` script to connect:

```sh
# List available ports
python3 scripts/picocmd.py -l

# Connect (replace port as needed)
python3 scripts/picocmd.py /dev/ttyACM0        # Linux / WSL
python3 scripts/picocmd.py /dev/cu.usbmodem101 # macOS
python3 scripts/picocmd.py COM3                # Windows
```

### Available Commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `help` | none | List all registered commands |
| `temp` | `<whole> <decimal>` | Set mock temperature (e.g. `temp 22 5` = 22.5°C) |
| `humid` | `<whole> <decimal>` | Set mock humidity (e.g. `humid 60 0` = 60%) |
| `mock` | `<0 or 1>` | Enable (1) or disable (0) mock sensor mode |
| `unit` | `<0 or 1>` | Set temperature unit: 0 = Celsius, 1 = Fahrenheit |
| `pattern` | `<1 or 2>` | Set LED strip pattern: 1 = solid color, 2 = progressive fill |

> **Note:** Mock mode allows testing the display without a live sensor. When disabled, the device reads from the real DHT20 sensor.

---

## Team

- **Alexander Swanson**
- **Edward Swanson**
- **Grant Wu**