# HolidayLights

ESP8266 firmware for controlling a holiday LED strip from a browser. It provides themed LED effects, scheduled operation, sunrise/sunset-based clock colors, runtime logs, and OTA firmware updates.

## Requirements

- Arduino IDE 2.3.6
- ESP8266 board support from the Boards Manager
- Libraries: `Adafruit NeoPixel`, `ArduinoJson`, `ElegantOTA`, and `NTPClient`
- An ESP8266 board and a compatible NeoPixel strip

`DNSServer` and BearSSL are provided by the ESP8266 core; they are not separate library dependencies.

## Initial Setup

1. Open the project in Arduino IDE.
2. Install the ESP8266 board package and libraries listed above.
3. Configure `Secrets.h` for your Wi-Fi network. The sketch checks for these exact macro names:

	```cpp
	#define WIFI_SSD "your-wifi-ssid"
	#define WIFI_PASSWD "your-wifi-password"
	```

	Keep real credentials private; do not publish them in a repository or screenshot.
4. Connect the ESP8266 over USB. Select **Generic ESP8266 Module** (or the matching board), choose the correct port, and upload the sketch.
5. Open the Serial Monitor at 115200 baud. In station mode, use the printed IP address to open the control page from a device on the same Wi-Fi network.

## Network Modes

- **Configured Wi-Fi:** When both `WIFI_SSD` and `WIFI_PASSWD` are defined, the device joins that network. Open `http://<device-ip>/` on the same network.
- **Setup hotspot:** If either home Wi-Fi credential macro is absent, the device starts the open `DropItLikeItsHotspot` network. Wildcard DNS directs HTTP requests to the OTA page. If the captive prompt does not open, visit `http://192.168.4.1/update` manually.
- AP-mode firmware exposes ElegantOTA only; it does not serve the lighting controls.

## Controls and Updates

- Use the slider on the main page to turn the LEDs on or off.
- Use the numbered clock buttons at 04:00 and 05:00 to toggle Relay 1 and Relay 2. The grid button toggles the LED matrix display.
- Open `/log` to view runtime messages. The log scrolls within the page.
- To move from setup mode to home Wi-Fi, compile a new build with both `WIFI_SSD` and `WIFI_PASSWD` defined. While the device is still running setup-mode firmware, connect to its hotspot, open `http://192.168.4.1/update`, and upload the new build. After reboot, it joins the configured home network and serves the lighting controls.
- OTA is available only in AP setup builds. A build with home Wi-Fi credentials does not expose OTA; future updates require USB upload or another AP-mode provisioning build.
- ElegantOTA identifies this firmware as `HolidayLights`, version `1.0.0`. Update the version passed to `ElegantOTA.setFWVersion()` in `OTA.h` for each firmware release.
- OTA uses HTTP and is unauthenticated on the open setup hotspot. It is not available over the home Wi-Fi interface.

## Sunrise and Sunset

The device fetches sunrise and sunset daily from Sunrise-Sunset.org. Set `SUNRISE_LATITUDE`, `SUNRISE_LONGITUDE`, and `TIME_ZONE_OFFSET_SECONDS` in `Constants.h` for the installation location. The current offset is a fixed UTC−7; locations with daylight saving time need their offset adjusted seasonally. If the API request fails, the firmware retains its fallback times.

## HTTPS and Captive Portal Notes

The captive portal uses HTTP because devices must be able to reach it before accepting a certificate. Wildcard DNS cannot transparently intercept HTTPS: browsers reject a certificate that does not match the requested host. The ESP8266 core includes BearSSL, but a usable HTTPS server also needs a private key and certificate trusted by each client. The current ElegantOTA integration requires the non-secure `ESP8266WebServer`, so adding HTTPS for the control page would leave OTA on a separate HTTP server. HTTPS is not enabled in this firmware; do not assume the current HTTP pages or OTA uploads are encrypted.

## Notes

- Intended for a trusted local network, not public internet access.
- Designed for one holiday lighting controller and LED strip.
