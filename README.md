# Haunt Firmware

**Haunt** is a custom firmware theme and asset pack for **ESP32 Cheap Yellow Display (CYD)** devices, built on top of the Bruce Firmware ecosystem.

> Status: **Beta**  
> Built for **Batista**

---

## Features

- Custom **boot animation** with Gengar + progress bar + "Initializing..."
- Complete **Bruce-compatible theme** (`Haunt.json`)
- Polished UI screens (Home, Clock, Files, Config, RF Tools, System, About)
- Consistent purple / ghost-themed icon set
- Status icons (success, error, warning, info)
- Lightweight assets optimized for ESP32

---

## Hardware

- ESP32-2432S028 (Cheap Yellow Display - CYD)
- 320×240 ILI9341 display

---

## Installation

### 1. Flash Bruce Firmware
Use the official Bruce flasher or your preferred method for CYD.

### 2. Install Haunt Theme
1. Copy the contents of the `theme/` folder to the root of your **SD card** or **LittleFS**.
2. On the device go to:  
   `Config → UI Theme → select Haunt.json`

### 3. Boot Animation
The file `boot.gif` is already inside the theme folder.  
You can also place it at the root of the SD card as `/boot.gif`.

---

## UI Screens included

- Home / Launcher
- Clock + System stats
- Files browser
- RF Tools
- Config
- System Status
- About (with Gengar)

---

## Credits

- Built for **Batista**
- Inspired by Bruce Firmware and Schematik workshop projects
- Gengar theme

---

## License

MIT