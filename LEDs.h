#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include "Constants.h"
#include "Helpers.h"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, LED_DPIN, NEO_BRG + NEO_KHZ800);

const uint32_t WHITE  = pixels.Color(128, 128, 128);
const uint32_t RED    = pixels.Color(128,   0,   0);
const uint32_t PINK   = pixels.Color(128,   0, 128);
const uint32_t GREEN  = pixels.Color(  0, 128,   0);
const uint32_t ORANGE = pixels.Color(128,  40,   0);
const uint32_t BLUE   = pixels.Color(  0,   0, 128);
const uint32_t PURPLE = pixels.Color( 64,   0, 128);

// ONBOARD LED FUNCTIONS
void blinkOnboardLed(int timeOn) {
  digitalWrite(ESP_BUILTIN_LED, LOW);
  delay(timeOn);
  digitalWrite(ESP_BUILTIN_LED, HIGH);
}

void onboardLed() {
#ifdef DEBUG_ON
  logMsg("Heartbeat at time: %s\n", timeClient.getFormattedTime());
  Serial.print("I'm alive at ");
  Serial.println(WiFi.localIP());
#endif

  blinkOnboardLed(350);
  delay(100);
  blinkOnboardLed(350);
}

// LED STRIP EFFECTS
void fade_wipe(Adafruit_NeoPixel* px, uint32_t color) {
  for (unsigned int i = 0; i < 18; i++) {
    int var = 0;
  }
}

void fadeColor(byte r1, byte g1, byte b1, byte r2, byte g2, byte b2, int wait) {
  for (int i = 0; i <= 255; i++) {
    // Calculate the intermediate color
    byte r = map(i, 0, 255, r1, r2);
    byte g = map(i, 0, 255, g1, g2);
    byte b = map(i, 0, 255, b1, b2);
    
    // Set and show the color on the strip
    for (int j = 0; j < pixels.numPixels(); j++) {
      pixels.setPixelColor(j, pixels.Color(r, g, b));
    }
    pixels.show();
    delay(wait);
  }
}

void setGradient() {
  const int maxGreen = 220;
  pixels.setBrightness(MAX_BRIGHTNESS);

  for (int i = 0; i <= NUM_PIXELS; i++) {
    float pos = (float) i / NUM_PIXELS;
    byte r, g, b;
    if (i <= NUM_PIXELS / 2) {
      r = 255;
      g = (byte)(maxGreen*(1-2*pos));
      b = 0;
    } else {
      r = 255;
      g = (byte)(maxGreen*(2*(pos-0.5)));
      b = 0;
    }

    // Set the pixel color
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

// SEQUENCES
void ledStrip() {
#ifdef DEBUG_ON
  logMsg("LED write at time: %s\n", timeClient.getFormattedTime());
#endif

  if (showLeds_) {
#ifdef INDEPENDENCE_DAY
    pixels.setBrightness(MAX_BRIGHTNESS);

    for (int i = NUM_PIXELS; i >= 0; i--) {
      if (i > floor(2*NUM_PIXELS / 3)) {
        if (i%4) {
          pixels.setPixelColor(i, BLUE);
        } else {
          pixels.setPixelColor(i, WHITE);
        }
      } else {
        if (int(i/2 + 1) % 2) {
          pixels.setPixelColor(i, RED);
        } else {
          pixels.setPixelColor(i, WHITE);
        }
      }
    }
    pixels.show();
#endif

#ifdef VALENTINES
    pixels.setBrightness(MAX_BRIGHTNESS);

    for (int i = 0; i < NUM_PIXELS; i++) {
      if (i % 9 < 3) {
        pixels.setPixelColor(i, RED);
      } else if (i % 9 < 6) {
        pixels.setPixelColor(i, PURPLE);
      } else {
        pixels.setPixelColor(i, PINK);
      }
    }
    pixels.show();
#endif

#ifdef CHRISTMAS
    if (showLeds_) {
      pixels.setBrightness(MAX_BRIGHTNESS);

      for (int i = 0; i < NUM_PIXELS; i++) {
        if (i % 8 < 4) {
          pixels.setPixelColor(i, RED);
        } else {
          pixels.setPixelColor(i, GREEN);
        }
      }
      pixels.show();
#endif

#ifdef THANKSGIVING
    if (showLeds_) {
      setGradient();
    }
#endif

#ifdef HALLOWEEN 
    if (showLeds_) {
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
    }
#endif
  } else {
    pixels.setBrightness(0);
    pixels.clear();
    pixels.show();
    delay(5000);
  }


}

#endif