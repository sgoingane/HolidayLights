# HolidayLights

Small ESP8266 project for controlling holiday LED lights over the local network.

## What it does
- Connects to Wi-Fi and hosts a simple web UI.
- Turns lights on/off from a browser.
- Runs themed holiday patterns (Halloween, Christmas, etc.) on an Adafruit NeoPixel strip.
- Supports OTA firmware updates over the network.
- Logs runtime status to a local web page.

## Environment
- Arduino IDE 2.3.6
- ESP8266 board support
- Adafruit NeoPixel library
- ElegantOTA library
- NTP client for time sync

## Setup
1. Open this project in Arduino IDE 2.3.6.
2. Install required libraries: `Adafruit NeoPixel`, `ElegantOTA`, `NTPClient`.
3. Add your Wi-Fi credentials in `Secrets.h` (or update the constants used by the sketch).
4. Select the correct ESP8266 board (Generic ESP8266 Module) and COM port.
5. Compile and upload sketch binary at the device IP in a browser ('http://<your-ip>/update').

## Usage
- Visit the ESP8266 IP address.
- Use the toggle to turn lights on/off.
- Open `/log` for runtime messages.
- OTA updates are available via the built-in web interface.

## Notes
- Intended for a local network, not public internet access.
- Designed for a holiday lighting display controlled by a single ESP8266 device.
