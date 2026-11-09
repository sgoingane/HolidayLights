#ifndef HELPERS_H
#define HELPERS_H

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>
#include <string.h>

void pinInit(void) {
  pinMode(ESP_BUILTIN_LED, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(DOT_MATRIX_CLK, OUTPUT);
  pinMode(DOT_MATRIX_CS, OUTPUT);
  pinMode(DOT_MATRIX_DIN, OUTPUT);
  pinMode(ACT_BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(STATUS_LED_PIN, HIGH);
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(DOT_MATRIX_CLK, LOW);
  digitalWrite(DOT_MATRIX_CS, LOW);
  digitalWrite(DOT_MATRIX_DIN, LOW);

  delay(1000);

  digitalWrite(RELAY1_PIN, LOW);
  delay(750);
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, LOW);
  delay(750);
  digitalWrite(RELAY2_PIN, HIGH);
  delay(750);

  digitalWrite(STATUS_LED_PIN, LOW);
}

void logMsg(const char* format, ...) {
  char msgBuff[MAX_LOG_BUFF_SIZE];
  va_list args;
  va_start(args, format);
  vsnprintf(msgBuff, sizeof(msgBuff), format, args);
  va_end(args);

  messageLog += String(msgBuff) + "\n";

  while (messageLog.length() > maxLogSize) {
    int truncateIdx = messageLog.indexOf('\n') + 1;
    if (truncateIdx > 0) {
      messageLog.remove(0, truncateIdx);
    } else {
      messageLog = "";
    }
  }

#ifdef SERIAL_PRINT
  Serial.print(msgBuff);
#endif
}

String formatTime(int minSinceMidnight) {
  int hour24 = (minSinceMidnight / 60) % 24;
  int minute = minSinceMidnight % 60;

  const char* period = (hour24 < 12) ? "am" : "pm";
  int hour12 = hour24 % 12;
  if (hour12 == 0) hour12 = 12;

  char timeText[10];
  snprintf(timeText, sizeof(timeText), "%d:%02d%s", hour12, minute, period);
  return String(timeText);
}

int parseUtcTimeToLocalMinutes(const char* timestamp) {
  if (timestamp == nullptr || strlen(timestamp) < 16 || timestamp[10] != 'T') {
    return -1;
  }
  if (timestamp[11] < '0' || timestamp[11] > '9' ||
      timestamp[12] < '0' || timestamp[12] > '9' ||
      timestamp[14] < '0' || timestamp[14] > '9' ||
      timestamp[15] < '0' || timestamp[15] > '9') {
    return -1;
  }

  int hour = (timestamp[11] - '0') * 10 + timestamp[12] - '0';
  int minute = (timestamp[14] - '0') * 10 + timestamp[15] - '0';
  if (hour > 23 || minute > 59) {
    return -1;
  }

  int localMinutes = hour * 60 + minute + TIME_ZONE_OFFSET_SECONDS / 60;
  localMinutes %= 1440;
  if (localMinutes < 0) {
    localMinutes += 1440;
  }
  return localMinutes;
}

void updateSunTimes() {
  if (!timeClient.isTimeSet()) {
    logMsg("Skipping sunrise update: clock is not synchronized\n");
    return;
  }

  time_t localEpoch = static_cast<time_t>(timeClient.getEpochTime());
  struct tm* localDate = gmtime(&localEpoch);
  if (localDate == nullptr) {
    logMsg("Skipping sunrise update: unable to read local date\n");
    return;
  }

  char date[11];
  snprintf(date, sizeof(date), "%04d-%02d-%02d", localDate->tm_year + 1900,
           localDate->tm_mon + 1, localDate->tm_mday);
  String url = "https://api.sunrise-sunset.org/json?lat=" + String(SUNRISE_LATITUDE, 4) +
               "&lng=" + String(SUNRISE_LONGITUDE, 4) + "&date=" + date + "&formatted=0";

  BearSSL::WiFiClientSecure secureClient;
  secureClient.setInsecure();
  secureClient.setTimeout(8000);

  HTTPClient http;
  http.setTimeout(8000);
  if (!http.begin(secureClient, url)) {
    logMsg("Unable to start sunrise API request\n");
    return;
  }

  int responseCode = http.GET();
  if (responseCode != HTTP_CODE_OK) {
    logMsg("Sunrise API request failed: HTTP %d\n", responseCode);
    http.end();
    return;
  }

  String responseBody = http.getString();
  http.end();
  DynamicJsonDocument response(1536);
  DeserializationError error = deserializeJson(response, responseBody);
  if (error) {
    String bodyPrefix = responseBody.substring(0, 80);
    bodyPrefix.replace("\r", " ");
    bodyPrefix.replace("\n", " ");
    logMsg("Unable to parse sunrise API response: %s len=%u body=%s\n",
           error.c_str(), responseBody.length(), bodyPrefix.c_str());
    return;
  }

  const char* status = response["status"] | "";
  const char* sunrise = response["results"]["sunrise"] | "";
  const char* sunset = response["results"]["sunset"] | "";
  int sunriseMinutes = parseUtcTimeToLocalMinutes(sunrise);
  int sunsetMinutes = parseUtcTimeToLocalMinutes(sunset);
  if (strcmp(status, "OK") != 0 || sunriseMinutes < 0 || sunsetMinutes < 0) {
    logMsg("Invalid sun times: status=%s sunrise=%s sunset=%s parsed=%d,%d\n",
           status, sunrise, sunset, sunriseMinutes, sunsetMinutes);
    return;
  }

  TIME_SUNRISE = sunriseMinutes;
  TIME_SUNSET = sunsetMinutes;
  logMsg("Daytime updated: %s (sunrise) to %s (sunset)\n", formatTime(TIME_SUNRISE).c_str(),
         formatTime(TIME_SUNSET).c_str());
}

void printTime() {
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm* ptm = gmtime((time_t*)&epochTime);

#ifdef DEBUG_ON
  logMsg("Current time: %s\n", timeClient.getFormattedTime());
#endif
}

#endif