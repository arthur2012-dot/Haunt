# Haunt Firmware

**Haunt** is an **independent system** for the ESP32 Cheap Yellow Display (CYD).

Gengar-themed · Clean dark UI · Built for **Batista**

> Status: **Beta v0.2** — Core firmware functional

---

## Features (v0.2)

| App | Description |
|-----|-------------|
| **Home** | Launcher with status bar (time + Wi-Fi) |
| **Relógio** | Adjustable clock, persistent |
| **Calculadora** | Basic 4-operation calculator |
| **Notas** | Local notes with on-screen keyboard, NVS storage |
| **Luz** | Screen + RGB LED as soft lamp |
| **Sensor** | LDR light sensor with live bar |
| **Wi-Fi** | Scan, connect, save credentials, reconnect, forget |
| **Game Hub** | Three original mini-games |

### Mini-games
- **Caça Pontos** — Maze / collect dots
- **Pulo Rápido** — Endless runner jump
- **Teste de Reflexo** — Reaction time test

All games are original. No ROMs, no third-party characters.

---

## Hardware

- **Board:** ESP32-2432S028R (Cheap Yellow Display)
- **Display:** 2.8" ILI9341 240×320
- **Touch:** XPT2046 resistive
- **Extras:** RGB LED, LDR, piezo speaker

---

## How to build & flash

### PlatformIO (recommended)

```bash
git clone https://github.com/arthur2012-dot/Haunt.git
cd Haunt
pio run -t upload
```

### Arduino IDE

1. Install ESP32 board package (2.0.x recommended)
2. Install libraries: `TFT_eSPI`, `XPT2046_Touchscreen`
3. Configure `User_Setup.h` for CYD (or use the flags from `platformio.ini`)
4. Open `src/main.cpp` and upload

---

## Project structure

```
Haunt/
├── src/
│   └── main.cpp          ← Full firmware (v0.2)
├── theme/
│   └── Haunt.json        ← Theme metadata
├── platformio.ini
├── README.md
├── INSTALL.md
├── SCREENS.md
└── HANDOFF_TO_SCHEMATIK.md
```

---

## Design rules

- Dark background, white/light-gray text
- Soft purple as accent only
- Clean cards, rounded corners
- No bottom navigation bar
- No lockscreen
- Lightweight — stays fast on ESP32

---

## Roadmap

- [x] Core apps (Home, Clock, Calc, Notes, Light, Sensor, Wi-Fi)
- [x] Game Hub with 3 original games
- [x] Persistent storage (NVS)
- [ ] Gengar boot animation integration
- [ ] Bluetooth / RF / RFID tool screens
- [ ] Settings + About screens
- [ ] SD card support for themes & notes

---

## Credits

Built for **Batista**  
Gengar theme inspiration — original code and games

---

## License

MIT
