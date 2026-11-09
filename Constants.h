#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Tasks.h"

#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define DEBUG_ON

// BOARD PINS
#define LED_DPIN 5          // D1
#define DOT_MATRIX_CLK 16   // D0
#define DOT_MATRIX_CS 12    // D6
#define DOT_MATRIX_DIN 14   // D5
#define ACT_BUTTON_PIN 13   // D7
#define STATUS_LED_PIN 15   // D8
#define RELAY1_PIN 0        // D3
#define RELAY2_PIN 4        // D2

#define NUM_PIXELS 94
#define MAX_BRIGHTNESS 100

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
String messageLog;

task tasks[5];

byte pattern[8] = {
  B00011000,
  B00100100,
  B10011001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

unsigned long ota_progress_millis = 0;

const int ESP_BUILTIN_LED = 2;
const int MAX_LOG_BUFF_SIZE = 256;
const int TIME_ON = (5.5 + 12)*60;    // 6:30pm
const int TIME_OFF = (11 + 12)*60;    // 11:00pm
const long TIME_ZONE_OFFSET_SECONDS = -25200;
const float SUNRISE_LATITUDE = 33.4484;
const float SUNRISE_LONGITUDE = -112.0740;
int TIME_SUNRISE = 6 * 60;
int TIME_SUNSET = 18 * 60;
const unsigned int maxLogSize = 4096; // bytes

static bool showLeds_ = false;
static bool showMatrix_ = false;
static bool schedActive_ = false;
static bool relayOneActive_ = false;
static bool relayTwoActive_ = false;

#endif