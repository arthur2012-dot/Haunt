# Haunt Firmware — Project Handoff for Schematik

**To:** Schematik AI  
**From:** Previous development session  
**Owner:** Batista  
**Date:** 2026-09-25  
**Status:** Beta — Visual & asset stage

---

## 1. What is Haunt?

Haunt is a custom visual theme + UI asset pack for the **ESP32 Cheap Yellow Display (CYD)** running **Bruce Firmware**.

It is **not** a full custom firmware from scratch.  
It is a complete visual layer (boot animation, icons, screens, theme) designed to run on top of Bruce.

Goal: Make the CYD feel like a clean, modern, personal device with a consistent identity (Gengar-inspired, dark, readable).

---

## 2. Hardware Target

- Board: ESP32-2432S028 (Cheap Yellow Display)
- Display: 320 × 240, ILI9341
- Base system: Bruce Firmware
- Storage: SD card or LittleFS for theme assets

---

## 3. What Already Exists

### Boot
- `boot.gif` — Gengar animation + progress bar + “Initializing...” + “Haunt firmware v0.1”
- Optimized (low frame count, limited colors, small file size)

### Theme
- `Haunt.json` — Bruce-compatible theme file
- Full set of menu icons (wifi, ble, rf, rfid, files, clock, config, etc.)
- Status icons (success, error, warning, info)
- Background

### UI Screens (mockups / visual references)
All screens are 320×240 PNG mockups that show the intended look:

- Home / Launcher
- Clock
- WiFi (network list)
- Bluetooth
- RF Scan (signal bars)
- RFID
- Power / Battery
- Storage
- Logs
- Network details
- Settings
- About (with Gengar + “Built for Batista”)

---

## 4. Design Rules (Important)

- Keep it **clean and readable**. Avoid heavy purple monochrome.
- Use dark background (`#0C0C10` range).
- Purple only as accent, not the dominant color.
- Prefer clear hierarchy: title → content → status.
- No bottom navigation bar (user rejected it).
- No lockscreen (user rejected it).
- Status bar at the top is acceptable (time + basic icons).
- Everything must stay lightweight for ESP32.

---

## 5. What Needs to Be Done Next

This project is currently at the **visual/asset stage**. The next steps are:

1. **Turn the mockups into real Bruce-compatible screens or custom app pages**
   - Either as theme assets or as custom LVGL / Bruce UI pages.

2. **Integrate the boot animation properly**
   - Ensure `/boot.gif` is loaded correctly on Bruce.

3. **Make the icons and theme fully functional**
   - Verify all icons in `Haunt.json` load on device.

4. **Optional but valuable**
   - Real WiFi scan list
   - Real Bluetooth list
   - Real RFID read/write flow
   - Real battery/power info
   - Simple file browser

5. **Keep performance in mind**
   - Prefer small PNGs / GIFs
   - Avoid heavy animations
   - Target smooth experience on CYD

---

## 6. How to Continue the Project Well

### Recommended approach for Schematik:

- Treat the existing PNG screens as **design references**, not final code.
- Rebuild the UI using whatever system Bruce currently supports (theme engine, custom screens, or LVGL if available).
- Keep the same information architecture and visual language.
- Prioritize:
  1. Boot animation working reliably
  2. Theme icons loading
  3. Home + Clock + About
  4. Then the tool screens (WiFi, RFID, RF, etc.)

### Style guide summary:
- Dark background
- White / light gray text
- One accent color (soft purple)
- Green for success / connected
- Orange for warnings / active scan
- Clean cards with rounded corners
- Minimal decoration

---

## 7. File Structure (Current)

```
Haunt/
├── README.md
├── HANDOFF_TO_SCHEMATIK.md      ← this file
├── SCREENS.md
├── theme/
│   ├── Haunt.json
│   ├── boot.gif
│   └── *.png (icons)
└── (UI mockups distributed as zips in conversation)
```

---

## 8. Owner Preferences (Batista)

- Does **not** want exaggerated or “invented” features (no lockscreen, no unnecessary nav bars).
- Prefers clean, useful screens.
- Dislikes monotonous single-color UIs.
- Wants the project to feel personal (“Built for Batista”).
- Always ask before making big directional changes.

---

## 9. Success Criteria

A good continuation of Haunt should result in:

- Device boots with the Gengar animation
- Theme icons appear correctly in Bruce menu
- At least Home, Clock and About feel native
- Overall look is clean, dark, and consistent
- Performance stays acceptable on CYD

---

**End of handoff.**

Schematik: you now have full context to continue the project properly.
