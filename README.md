# Real-Time-Healthcare-DAQ-System
Embedded C/C++ real-time biomedical data acquisition (DAQ) firmware for pulse rate (BPM) and SpO2 monitoring with digital signal filtering and local 16x2 LCD telemetry.
# Real-Time Healthcare Data Acquisition (DAQ) System

An embedded biomedical Data Acquisition (DAQ) system designed to acquire, filter, and display real-time **Heart Rate (BPM)** and **Blood Oxygen Saturation (SpO2)** readings locally on a **16x2 I2C LCD** interface for low-cost home healthcare monitoring.

## Signal Processing & Hardware Flow
+--------------------+     I2C (400 kHz)     +-----------------------+     I2C     +------------------+
|  MAX30102 Pulse    | --------------------> |   Microcontroller     | ----------> |  16x2 I2C LCD    |
|  Oximeter Sensor   |   Red & IR Photodiode |  - DC Removal Filter  |             |  BPM & SpO2 UI   |
|  (PPG Raw Samples) |        Samples        |  - Moving Avg Filter  |             +------------------+
+--------------------+                       |  - Peak / Beat Detect |
+-----------------------+
## Key Features
- **High-Speed PPG Sampling:** Interfaces with optical pulse oximetry hardware over I2C to sample Red and Infrared (IR) photoplethysmogram (PPG) signals.
- **Digital Signal Conditioning:** Implements a DC-tracking filter and 4-point moving average filter to suppress ambient light drift and motion noise.
- **Low-Latency Local Display:** Updates BPM and SpO2 values cleanly on a 16x2 LCD without blocking sample acquisition, plus alerts when no finger is detected.

## Hardware Connections
| Component | Interface | Microcontroller Pin |
| :--- | :--- | :--- |
| MAX30102 Optical Sensor | I2C SDA / SCL | A4 (SDA) / A5 (SCL) |
| 16x2 Character LCD (PCF8574) | I2C SDA / SCL | A4 (SDA) / A5 (SCL) |
| Buzzer / Alert Indicator | Digital Output | D8 |

## Dependencies
- `Wire.h` (I2C Bus Driver)
- `LiquidCrystal_I2C.h`
- `MAX30105.h` & `heartRate.h` (SparkFun MAX3010x Sensor Library)
