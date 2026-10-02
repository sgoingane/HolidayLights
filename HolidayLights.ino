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
#include "Tasks.h"
#include "LEDs.h"
#include "OTA.h"
#include "Secrets.h"

#define SERIAL_PRINT

const char* ssid = "SSID";
const char* password = "PASSWD";

void setup(void) {
  // Configure and initialize LED strip object
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

  // Start OTA reprogramming server
  ElegantOTA.begin(&server);
  setupOTACallbacks();

  server.begin();
  logMsg("HTTP server started\n");

  // Blink LED four times to show setup complete
  pinMode(ESP_BUILTIN_LED, OUTPUT);
  digitalWrite(ESP_BUILTIN_LED, LOW);

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
