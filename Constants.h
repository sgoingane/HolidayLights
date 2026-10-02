#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Tasks.h"

#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define DEBUG_ON

#define LED_DPIN 5
#define NUM_PIXELS 94
#define MAX_BRIGHTNESS 100

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

task tasks[3];

unsigned long ota_progress_millis = 0;

const int ESP_BUILTIN_LED = 2;
const int MAX_LOG_BUFF_SIZE = 256;
const unsigned int maxLogSize = 4096; // bytes

static bool showLeds_ = false;

#endif