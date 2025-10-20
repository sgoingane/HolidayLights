/*
  -----------------------
  ElegantOTA - Demo Example
  -----------------------

  Skill Level: Beginner

  This example provides with a bare minimal app with ElegantOTA functionality.

  Github: https://github.com/ayushsharma82/ElegantOTA
  WiKi: https://docs.elegantota.pro

  Works with following hardware:
  - ESP8266
  - ESP32
  - RP2040 (with WiFi) (Example: Raspberry Pi Pico W)


  Important note for RP2040 users:
  - RP2040 requires LittleFS partition for the OTA updates to work. Without LittleFS partition, OTA updates will fail.
    Make sure to select Tools > Flash Size > "2MB (Sketch 1MB, FS 1MB)" option.
  - If using bare RP2040, it requires WiFi module like Pico W for ElegantOTA to work.

  -------------------------------

  Upgrade to ElegantOTA Pro: https://elegantota.pro

*/


#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WebServer.h>
#elif defined(TARGET_RP2040) || defined(TARGET_RP2350) || defined(PICO_RP2040) || defined(PICO_RP2350)
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WiFiServer.h>
  #include <WebServer.h>
#endif

#include <Adafruit_NeoPixel.h>
#include <ElegantOTA.h>

#include "LedEffects.h"

#define LEDS_OFF // MAIN LED STRIP CONTROL

#define LED_DPIN 5
#define NUM_PIXELS 94
#define MAX_BRIGHTNESS 100

const char* ssid = "SSID";
const char* password = "PASSWD";

const int ESP_BUILTIN_LED = 2;

#if defined(ESP8266)
  ESP8266WebServer server(80);
#elif defined(ESP32)
  WebServer server(80);
#elif defined(TARGET_RP2040) || defined(TARGET_RP2350) || defined(PICO_RP2040) || defined(PICO_RP2350)
  WebServer server(80);
#endif

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, LED_DPIN, NEO_BRG + NEO_KHZ800);

unsigned long ota_progress_millis = 0;

//uint32_t ORANGE = pixels.Color(255, 0, 64);
//uint32_t PURPLE = pixels.Color(89, 10, 153);

uint32_t ORANGE = pixels.Color(128, 40, 0);
uint32_t PURPLE = pixels.Color(64, 0, 128);

void ledStrip() {
#ifdef LEDS_ON
  pixels.setBrightness(MAX_BRIGHTNESS);

  for (int i = 0; i < NUM_PIXELS; i++) {
    if (i % 8 < 4) {
      pixels.setPixelColor(i, ORANGE);
    } else {
      pixels.setPixelColor(i, PURPLE);
    }
  }
  pixels.show();
  delay(5000);
#endif // LEDS_ON
}

void blinkLed(int timeOn, int timeOff) {
  digitalWrite(ESP_BUILTIN_LED, HIGH);
  delay(timeOn);
  digitalWrite(ESP_BUILTIN_LED, LOW);
  delay(timeOff);
}

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>

}

void setup(void) {
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(0);
  pixels.show();

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is ElegantOTA Demo.");
  });

  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
  Serial.println("HTTP server started");

  pinMode(ESP_BUILTIN_LED, OUTPUT);
  digitalWrite(ESP_BUILTIN_LED, LOW);

  for (unsigned int i=0; i<4; i++) {
    blinkLed(200, 100);
  }
  digitalWrite(ESP_BUILTIN_LED, LOW);
}

void loop(void) {
  server.handleClient();
  ElegantOTA.loop();

  // Onboard LED heartbeat
  blinkLed(700, 300);
  blinkLed(700, 1500);

  ledStrip();
}
