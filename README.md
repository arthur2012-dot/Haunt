# Haunt Firmware

**Haunt** is a custom firmware theme and asset pack for **ESP32 Cheap Yellow Display (CYD)** devices, built on top of the Bruce Firmware ecosystem.

> Status: **Beta**  
> Built for **Batista**

---

## Features

- Custom **boot animation** with Gengar + progress bar + "Initializing..."
- Complete **Bruce-compatible theme** (`Haunt.json`)
- Clean and modern UI screens
- Consistent icon set
- Lightweight assets optimized for ESP32

---

## UI Screens

- Home / Launcher
- Clock
- WiFi (network list)
- Bluetooth (device list)
- RF Scan (signal visualizer)
- Storage / SD Card
- Logs
- Files
- Config
- System
- About (with Gengar)

---

## Hardware

- ESP32-2432S028 (Cheap Yellow Display - CYD)
- 320×240 ILI9341 display

---

## Installation

1. Flash **Bruce Firmware** on your CYD
2. Copy the theme files to the root of your SD card / LittleFS
3. On the device: `Config → UI Theme → Haunt.json`
4. Place `boot.gif` at the root (or inside the theme folder)

---

## Credits

- Built for **Batista**
- Inspired by Bruce Firmware
- Gengar theme

---

## License

MIT