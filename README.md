# ESP32 Bluetooth Music Controller

## Overview
This is a gesture-based audio control system that allows users to manage music playback on Android devices without WiFi. Able to detect hand gestures for controlling volume, navigating tracks, and toggling play/pause. The project uses Bluetooth HID (Human Interface Device) to send media commands to apps like Spotify and YouTube, offering a touchless, intuitive interface for offline music control.

### Features
- **Volume Control**: Adjust volume with forward (increase) and backward (decrease) gestures.
- **Track Navigation**: Swipe left for previous track, right for next track.
- **Play/Pause**: Toggle playback with a single touch.
- **Offline Operation**: Bluetooth-only, no internet required.
- **Compact Design**: Built on a 170-point breadboard for portability.

## Hardware Requirements
- **ESP32 Dev Board**: Core microcontroller with Bluetooth Low Energy (BLE) support.
- **Adafruit VL53L0X**: Time-of-Flight proximity sensor for volume gestures.
- **HC-SR04 Ultrasonic Sensor**: Detects left/right swipes for track navigation.
- **TTP223 Capacitive Touch Sensor**: For play/pause control.
- **170-Point Breadboard**
- **Male-to-Male Jumper Wires**
- **Power Source**: USB cable or 5V supply.

### Pin Connections
| **Component**     | **Pin** | **ESP32 Pin** |
|-------------------|---------|---------------|
| **VL53L0X**       | VIN     | 3.3V          |
|                   | GND     | GND           |
|                   | SDA     | GPIO 21       |
|                   | SCL     | GPIO 22       |
| **HC-SR04**       | VCC     | 5V            |
|                   | GND     | GND           |
|                   | Trig    | GPIO 12       |
|                   | Echo    | GPIO 13       |
| **TTP223**        | VCC     | 3.3V          |
|                   | GND     | GND           |
|                   | OUT     | GPIO 27       |

**Notes**:
- The HC-SR04 requires 5V; ensure your ESP32 board provides a 5V pin or use an external 5V supply with a common GND.
- Position the VL53L0X facing forward for forward/backward gestures and the HC-SR04 facing forward for swipes to avoid interference.

## Software Requirements
- **Arduino IDE**: For uploading code to the ESP32.
- **Libraries**:
  - `Adafruit_VL53L0X`: For the VL53L0X proximity sensor.
  - `ESP32-BLE-Keyboard` (by T-vK): For Bluetooth HID media control.
- **ESP32 Board Package**: Add ESP32 support to Arduino IDE via Boards Manager.

## Installation
1. **Set Up Arduino IDE**:
   - Install the Arduino IDE.
   - Add ESP32 board support. Search for “ESP32” and install `esp32` by Espressif Systems.
   - Install libraries `Adafruit_VL53L0X` and `ESP32-BLE-Keyboard`.

2. **Connect Hardware**:
   - Assemble components on the 170-point breadboard per the Pin Connections table.
   - Ensure secure jumper wire connections and a common GND rail.
   - Power the ESP32 via USB or a 5V supply.

3. **Upload Code**:
   - Download the project code (`esp32_music_controller.ino`) from this repository.
   - Open in Arduino IDE, select your ESP32 board.
   - Connect the ESP32 via USB and upload the code.

## Gesture Controls
- **Volume Control (VL53L0X)**:
  - **Forward Gesture**: Move hand closer (5-30cm) to increase volume by one step.
  - **Backward Gesture**: Move hand farther to decrease volume by one step.
- **Track Navigation (HC-SR04)**:
  - **Right Swipe**: Quickly pass hand within 10cm (<500ms) to skip to the next track.
  - **Left Swipe**: Pass hand more slowly (500-1000ms) to return to the previous track.
- **Play/Pause (TTP223)**:
  - Tap the touch sensor to toggle play/pause.

## Usage
1. **Pair with Android**:
   - On an Android phone scan, and pair with “ESP32 Music Controller.”
   - Confirm any pairing code if prompted.
2. **Test with Music App**:
   - Open any music app
   - Play a playlist and test gestures:
     - Swipe right/left for track changes.
     - Move hand closer/farther for volume.
     - Tap TTP223 for play/pause.
3. **Debugging**:
   - Open Serial Monitor at 115200 baud:
     - Sensor initialization status.
     - Distance readings (VL53L0X: 50-300mm, HC-SR04: <100mm during swipes).
     - Gesture detection (e.g., “Right swipe: Next track”, “Forward gesture: Volume UP”).
     - Bluetooth connection status.



