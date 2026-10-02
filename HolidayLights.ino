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

#undef VALENTINES
#undef INDEPENDENCE
#define HALLOWEEN
#undef THANKSGIVING
#undef CHRISTMAS


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Adafruit_NeoPixel.h>
#include <ElegantOTA.h>

#include "Constants.h"
#include "Pages.h"
#include "Helpers.h"
#include "TaskScheduler.h"
#include "LedEffects.h"
#include "Secrets.h"

#define SERIAL_PRINT

const char* ssid = "SSID";
const char* password = "PASSWD";

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
    logMsg("OTA update finished successfully!\n");
  } else {
    logMsg("There was an error during OTA update!\n");
  }
  // <Add your own code here>
}

void setup(void) {
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(0);
  pixels.show();

#ifdef SERIAL_PRINT
  Serial.begin(115200);
  Serial.println("");
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSD, WIFI_PASSWD);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

#ifdef SERIAL_PRINT
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSD);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif

  timeClient.begin();
  timeClient.setTimeOffset(-25200);  // UTC +7 Phoenix

  // Define routes
  server.on("/", handleMainPage);
  server.on("/log", handleLogPage);

  server.on("/on", []() {
    showLeds_ = true;
    server.sendHeader("Location", "/");  // root redirect
    server.send(303);
  });

  server.on("/off", []() {
    showLeds_ = false;
    server.sendHeader("Location", "/");  // root redirect
    server.send(303);
  });



  ElegantOTA.begin(&server);  // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  //ElegantOTA.setID("Holiday");
  //ElegantOTA.setFWVersion("0.1");

  server.begin();
  logMsg("HTTP server started\n");

  pinMode(ESP_BUILTIN_LED, OUTPUT);
  digitalWrite(ESP_BUILTIN_LED, LOW);

  // Initialization onboard flash
  for (unsigned int i = 0; i < 4; i++) {
    blinkOnboardLed(100);
    delay(80);
  }
  digitalWrite(ESP_BUILTIN_LED, LOW);

  // Setup tasks
  tasks[0].name = "heartbeat";
  tasks[0].period = 3000;
  tasks[0].handler = &onboardLed;

  tasks[1].name = "timeprint";
  tasks[1].period = 600000;
  tasks[1].handler = &printTime;

  tasks[2].name = "ledeffect";
  tasks[2].period = 8000;
  tasks[2].handler = &ledStrip;
}

static unsigned int prevTime_ = 0;
void loop(void) {
  timeClient.update();

  server.handleClient();
  ElegantOTA.loop();

  unsigned long tickTime_ = millis() - prevTime_;
  for (unsigned int i = 0; i < sizeof(tasks) / sizeof(tasks[0]); i++) {
    tasks[i].tick(tickTime_);
  }
  prevTime_ = millis();
}
