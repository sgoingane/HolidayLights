#undef VALENTINES
#undef INDEPENDENCE
#undef HALLOWEEN
#define THANKSGIVING
#undef CHRISTMAS


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#include <Adafruit_NeoPixel.h>
#include <LedControl.h>
#include <ElegantOTA.h>

#include "Constants.h"
#include "Pages.h"
#include "Helpers.h"
#include "Tasks.h"
#include "LEDs.h"
#include "OTA.h"
#include "Secrets.h"

#define SERIAL_PRINT

DNSServer dnsServer;
bool captivePortalActive_ = false;

void setup(void) {
  // Pin Configuration
  pinInit();

#if defined(WIFI_SSD) && defined(WIFI_PASSWD)
  // Configure and initialize LED strip object
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(0);
  pixels.show();

  matrix.shutdown(0, false);
  matrix.setIntensity(0,5);
  matrix.clearDisplay(0);

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
  timeClient.setTimeOffset(TIME_ZONE_OFFSET_SECONDS);
  timeClient.update();
  updateSunTimes();

  server.on("/", handleMainPage);
  server.on("/log", handleLogPage);
  server.on("/on", []() {
    showLeds_ = true;
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/off", []() {
    showLeds_ = false;
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/test/on", []() {
    testActive_ = true;
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/test/off", []() {
    testActive_ = false;
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/relay1/on", []() {
    relayOneActive_ = true;
    digitalWrite(RELAY1_PIN, LOW);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/relay1/off", []() {
    relayOneActive_ = false;
    digitalWrite(RELAY1_PIN, HIGH);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/relay2/on", []() {
    relayTwoActive_ = true;
    digitalWrite(RELAY2_PIN, LOW);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/relay2/off", []() {
    relayTwoActive_ = false;
    digitalWrite(RELAY2_PIN, HIGH);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/matrix/on", []() {
    showMatrix_ = true;
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/matrix/off", []() {
    showMatrix_ = false;
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not Found");
  });

  logMsg("HTTP server started\n");

  // Setup tasks
  tasks[0].name = "heartbeat";
  tasks[0].period = 10000;
  tasks[0].handler = &onboardLed;

  tasks[1].name = "timeprint";
  tasks[1].period = 600000;
  tasks[1].handler = &printTime;

  tasks[2].name = "ledeffect";
  tasks[2].period = LED_PERIOD;
  tasks[2].handler = &ledStrip;

  tasks[3].name = "sun-times";
  tasks[3].period = 86400000UL;
  tasks[3].handler = &updateSunTimes;

  tasks[4].name = "matrix";
  tasks[4].period = 5000;
  tasks[4].handler = &dotMatrix;

#else // WIFI_SSD && WIFI_PASSWD

  WiFi.mode(WIFI_AP);
  WiFi.softAP("DropItLikeItsHotspot");
  captivePortalActive_ = true;
  dnsServer.start(53, "*", WiFi.softAPIP());

  server.onNotFound([]() {
    server.sendHeader("Location", "/update");
    server.send(302, "text/plain", "Open /update for setup firmware updates");
  });

#endif // WIFI_SSD && WIFI_PASSWD

  server.begin();

  ElegantOTA.begin(&server);
  setupOTACallbacks();

  // Blink LED four times to show setup complete
  digitalWrite(ESP_BUILTIN_LED, LOW);

  for (unsigned int i = 0; i < 4; i++) {
    blinkOnboardLed(100);
    delay(80);
  }
  digitalWrite(ESP_BUILTIN_LED, LOW);
}

static unsigned int prevTime_ = 0;
void loop(void) {
  if (captivePortalActive_) {
    dnsServer.processNextRequest();
  } else {
    timeClient.update();
  }
  server.handleClient();
  ElegantOTA.loop();

  int currMin = timeClient.getHours()*60 + timeClient.getMinutes();
  if (TIME_ON < TIME_OFF) {
    schedActive_ = timeClient.isTimeSet() && (currMin >= TIME_ON && currMin < TIME_OFF);
  } else {
    schedActive_ = timeClient.isTimeSet() && (currMin >= TIME_ON || currMin < TIME_OFF);
  }

  digitalWrite(STATUS_LED_PIN, digitalRead(ACT_BUTTON_PIN) ? LOW : HIGH);

  unsigned long tickTime_ = millis() - prevTime_;
  for (unsigned int i = 0; i < sizeof(tasks) / sizeof(tasks[0]); i++) {
    tasks[i].tick(tickTime_);
  }
  prevTime_ = millis();
}
