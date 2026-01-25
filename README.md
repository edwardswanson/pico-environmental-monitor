# Pico Environmental Monitor (CS Capstone)

## Overview

This project is an embedded systems capstone built on the **Raspberry Pi Pico (RP2040)** using **bare-metal C** and the official **Pico SDK**. The goal is to design and implement a small environmental monitoring system that reads temperature and humidity data and displays it on an LCD over **I²C**.

The project emphasizes low-level hardware interaction, protocol understanding, and embedded debugging rather than reliance on high-level frameworks.

---

## Current Status (Week 1)

- Toolchain and firmware bring-up
- USB-serial debugging
- I²C bus scanning and device discovery
- LCD driver implementation and testing
- Sensor integration (AHT20/DHT20) — *in progress*

---

## Hardware

- Raspberry Pi Pico (RP2040)
- 16×2 LCD with I²C backpack (PCF8574)
- Breadboard and jumper wires
- *(Planned)* AHT20 / DHT20 temperature & humidity sensor

---

## Software & Tools

- C (C11)
- Raspberry Pi Pico SDK
- ARM GCC toolchain
- CMake
- USB serial for runtime debugging

---

## Features Implemented

- Bare-metal firmware build and flashing via UF2
- USB-serial logging for runtime visibility
- Custom I²C scanner to detect devices on the bus
- LCD driver supporting:
  - 4-bit HD44780 initialization
  - Cursor control
  - Multi-line text output
  - Periodic screen updates

---

## Project Structure

```text
.
├── src/
│   ├── main.c              # Application logic
│   ├── lcd_pcf8574.c       # LCD driver implementation
│   └── lcd_pcf8574.h       # LCD driver interface
├── CMakeLists.txt
└── README.md
```

## Planned Next Steps

- Integrate AHT20 temperature & humidity sensor (I²C)
- Add formatted environmental data display
- Implement basic error handling and retries
- Optional power optimization and sleep modes
- Final documentation and demo for capstone submission

⸻

## Why This Project

This capstone is designed to demonstrate:

- Comfort working close to hardware
- Understanding of embedded communication protocols (I²C)
- Ability to debug real hardware/software interactions
- Clean, modular C code in a constrained environment

## Deployment Guide

### Setup (One-time)

1. Install picotool: `brew install picotool`
2. Set SDK path in `~/.zshrc`:

   ```sh
   export PICO_SDK_PATH=/path/to/pico-sdk
   ```

3. Reload shell: `source ~/.zshrc`

### Deploy Firmware

1. Plug in your Pico via USB
2. Run the deploy script:

   ```sh
   ./deploy.sh
   ```

The script automatically builds and flashes the firmware!

### Optional: Create Alias for Convenience

Add to `~/.zshrc`:

```sh
alias pico-deploy='cd ~/path/to/pico-environmental-monitor && ./deploy.sh'
```

Then you can run `pico-deploy` from anywhere.
