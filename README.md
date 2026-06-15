# ESP32 Bad Apple TFT

Play Bad Apple video on ESP32 using TFT_eSPI library. Video stored in internal flash. No SD card needed.

> ⚠️ **CRITICAL WARNING:** Video only. No audio support.

---

## Video Specifications
* **Resolution:** 160x120 (Hardware scaled x2 to 320x240).
* **Framerate:** 15 FPS.
* **Color:** 1-bit Monochromatic.
* **Data Size:** ~1.6 MB (Compressed via RLE).

---

## Hardware used in this repo
* **MCU:** ESP32.
* **Display:** 2.4" TFT ST7789 (240x320). 
* **Note:** ILI display series (ILI9341, etc.) **untested**.
* **Storage:** Internal flash only (`PROGMEM`). No SD card reader required.

---

## Critical Configuration

### 1. Partition Scheme (Fix Sketch Too Big Error)
Sketch data (~1.6MB) exceeds default 1.2MB partition. Change partition layout:
* **Arduino IDE:** `Tools` -> `Partition Scheme` -> Select **Huge APP (3MB No OTA)**.
* **PlatformIO:** Add line to `platformio.ini`:
  ```ini
  board_build.partitions = huge_app.csv
  ```

### 2. TFT_eSPI Library Config

Modify `User_Setup.h` in TFT_eSPI library directory. Set correct drivers and pins for ST7789. Maximize SPI frequency for smooth frame rates:

```cpp
#define SPI_FREQUENCY  40000000 // Try 80000000 if display supports it
```

---

## Code Variants

### Variant 1: `BadApple.ino`

* Standard video playback.
* Uses RAM line-buffer rendering to eliminate screen scan lag.

### Variant 2: `BadApple_with-relay.ino`

* Video playback with external light automation.
* **Relay Pin:** GPIO 14 (Configured for Active LOW / `INVERT_RELAY = true`).
* **Trigger Logic:** Checks 4 corners of current frame. If $\ge 3$ corners turn white, triggers relay to turn on light.

## TODO
audio support 🫪
