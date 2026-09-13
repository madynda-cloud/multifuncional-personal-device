# Multifunctional Personal Device (Arduino)

A multi-sensor Arduino device combining distance, temperature, and heart-rate (BPM) measurement, with live readout on an OLED display.

## How it works

The Arduino continuously reads three sensors and refreshes the OLED display every 500 ms:

- **Ultrasonic distance sensor (HC-SR04):** triggers a pulse and measures echo return time to compute distance in cm.
- **Temperature sensor (DS18B20):** read over the OneWire bus via the DallasTemperature library.
- **Pulse sensor:** samples an analog heart-rate signal and detects beats by threshold crossing; BPM is calculated every 10-second window from the beat count, with an LED flashing on each detected beat.

All three readings are displayed simultaneously on a 128x64 SSD1306 OLED (SPI, via U8g2lib).

## Hardware

- Arduino (Uno/Nano-compatible)
- SSD1306 128x64 OLED display (4-wire SPI)
- HC-SR04 ultrasonic distance sensor
- DS18B20 temperature sensor (OneWire)
- Analog pulse/heart-rate sensor
- LED (beat indicator)

## Libraries used

- [U8g2lib](https://github.com/olikraus/u8g2) — OLED display driver
- [OneWire](https://github.com/PaulStoffregen/OneWire)
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)


## Possible improvements

- Adaptive threshold calibration for the pulse sensor (EMA-based calibration function is scaffolded in the code but currently disabled).
- Data logging over serial or to an SD card.

## Author

Matej Dynda and Stepan Gorichev
