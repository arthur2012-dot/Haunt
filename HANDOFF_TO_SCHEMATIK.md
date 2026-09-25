# Haunt Firmware — Project Handoff for Schematik

**To:** Schematik AI  
**From:** Previous development session  
**Owner:** Batista  
**Date:** 2026-09-25  
**Status:** Beta — Visual & asset stage

---

## 1. What is Haunt?

**Haunt is an independent system** for the ESP32 Cheap Yellow Display (CYD).

It is **not** an extension or theme pack for Bruce Firmware.  
It is only **compatible** with Bruce (can reuse some of its structure, boot method, and asset loading ideas), but the goal is a standalone firmware/UI experience owned by Batista.

Goal: Create a clean, personal, independent system for the CYD with its own identity (Gengar-inspired, dark, readable, useful).

---

## 2. Hardware Target

- Board: ESP32-2432S028 (Cheap Yellow Display)
- Display: 320 × 240, ILI9341
- Independent system (Bruce-compatible where useful)
- Storage: SD card or LittleFS

---

## 3. What Already Exists

### Boot
- `boot.gif` — Gengar animation + progress bar + “Initializing...” + “Haunt firmware v0.1”
- Optimized for low resource usage

### Visual Assets
- Icon set (wifi, ble, rf, rfid, files, clock, config, etc.)
- Status icons (success, error, warning, info)
- Background
- Multiple UI screen mockups (320×240)

### UI Screens (design references)
- Home / Launcher
- Clock
- WiFi
- Bluetooth
- RF Scan
- RFID
- Power / Battery
- Storage
- Logs
- Network
- Settings
- About (“Built for Batista”)

---

## 4. Design Rules (Important)

- Clean and readable. Avoid heavy monochrome purple.
- Dark background.
- Purple only as accent.
- Clear visual hierarchy.
- No bottom navigation bar (rejected by owner).
- No lockscreen (rejected by owner).
- Top status bar is acceptable.
- Everything must stay lightweight for ESP32.

---

## 5. Important Clarification for Future Development

Haunt should be developed as an **independent system**.

Bruce compatibility is a convenience (especially for boot.gif and basic asset loading), not the final architecture.  
Do not treat Haunt as “just a Bruce theme”. Build it so it can evolve into its own firmware experience.

---

## 6. What Needs to Be Done Next

1. Turn the visual mockups into real working screens in the independent system.
2. Implement proper boot sequence using the existing `boot.gif`.
3. Build core apps: Home, Clock, About, Settings, Power.
4. Then implement tool screens (WiFi, Bluetooth, RF, RFID, Storage, Logs).
5. Keep performance high and UI clean.

---

## 7. How to Continue the Project Well

- Treat current PNG screens as **design references**.
- Prefer building a clean, independent UI layer.
- Reuse Bruce ideas only when they help (boot loading, file system, etc.).
- Prioritize:
  1. Reliable boot animation
  2. Solid Home + navigation
  3. Clock / Power / About
  4. Tool screens

### Style guide
- Dark background
- White / light gray text
- Soft purple as accent
- Green = success / connected
- Orange = active / warning
- Clean cards, rounded corners, minimal decoration

---

## 8. Owner Preferences (Batista)

- Wants an **independent system**, not a Bruce extension.
- Rejects exaggerated or unnecessary features.
- Prefers clean and useful screens.
- Dislikes monotonous single-color UIs.
- Personal branding: “Built for Batista”.
- Always confirm before big directional changes.

---

## 9. Success Criteria

- Device boots with Haunt animation
- Feels like its own system
- Clean dark UI
- Core screens working
- Good performance on CYD

---

**End of handoff.**

Schematik: Haunt is an independent project. Continue it as such.
