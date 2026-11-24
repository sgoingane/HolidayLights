#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include "Constants.h"
#include "Helpers.h"

LedControl matrix(14,16,12,1);

#undef LED_STRIP
#define LED_STRAND

#if defined(LED_STRIP)
  // House Lights: WS2811 Addressable LED Light Strip
  #define NUM_PIXELS 94
  Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, LED_DPIN, NEO_BRG + NEO_KHZ800);
#elif defined(LED_STRAND)
  // Christmas Tree: WS2811 Addressable LED Light Strand
  #define NUM_PIXELS 200
  Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, LED_DPIN, NEO_RGB + NEO_KHZ800);
#endif

#define MAX_BRIGHTNESS 100

static uint16_t effectCnt = 0;
static uint8_t currEffect = 0;
static uint16_t hueOffset = 0;
static uint8_t level[NUM_PIXELS] = {0};
static int8_t direction[NUM_PIXELS] = {0};

const uint32_t BLACK  = pixels.Color(  0,   0,   0);
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
  blinkOnboardLed(350);
  delay(100);
  blinkOnboardLed(350);
}

void dotMatrix() {
  for (int row = 0; row < 8; row++) {
    if (showMatrix_) {
      matrix.setRow(0, row, pattern[row]);
    } else {
      matrix.setRow(0, row, B00000000);
    }
  }
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
  if (testActive_ || (showLeds_ && schedActive_)) {
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
    pixels.setBrightness(MAX_BRIGHTNESS);
    uint16_t step = 0;

    switch (currEffect) {
      case 0:
        for (uint16_t i = 0; i < NUM_PIXELS; i++) {
          uint16_t hue = hueOffset + (uint32_t)i * 65536 / NUM_PIXELS;
          pixels.setPixelColor(i, pixels.ColorHSV(hue));
        }

        hueOffset -= 1024;  // Increase this for faster movement
        if (effectCnt++ > 16 * 1000 / LED_PERIOD) {
          effectCnt = 0;
          currEffect = 1;
        }
        break;

      case 1:
        for (int i = 0; i < NUM_PIXELS; i++) {
          if (i % 8 < 4) {
            pixels.setPixelColor(i, RED);
          } else {
            pixels.setPixelColor(i, GREEN);
          }
        }
        if (effectCnt++ > 6 * 1000 / LED_PERIOD) {
          effectCnt = 0;
          currEffect = 2;
        }
        break;

      case 2:
        // Occasionally start a twinkle on a random inactive LED.
        step = random(NUM_PIXELS);
        if (direction[step] == 0 && random(100) < 85) {
          direction[step] = 1;  // Fade in
        }

        for (step = 0; step < NUM_PIXELS; step++) {
          if (direction[step] == 1) {
            if (level[step] < 255) {
              level[step] = min(255, level[step] + 8);
            } else {
              direction[step] = -1;  // Fade out
            }
          } else if (direction[step] == -1) {
            if (level[step] > 8) {
              level[step] -= 8;
            } else {
              level[step] = 0;
              direction[step] = 0;
            }
          }

          pixels.setPixelColor(
            step,
            pixels.Color(
              255 * level[step] / 255,
              110 * level[step] / 255,
              20  * level[step] / 255
            )
          );
        }
        if (effectCnt++ > 20 * 1000 / LED_PERIOD) {
          effectCnt = 0;
          currEffect = 0;
        }
        break;

      default:
        currEffect = 0;
        break;
    }

    pixels.show();
#endif

#ifdef THANKSGIVING
    pixels.setBrightness(MAX_BRIGHTNESS);

    for (uint16_t i = 0; i < NUM_PIXELS; i++) {
      if (level[i] == 0) {
        level[i] = random(100, 220);
        direction[i] = random(0, 2) ? 1 : -1;
      } else {
        // Usually keep moving in the same direction; sometimes reverse.
        if (random(100) < 15) {
          direction[i] = -direction[i];
        }

        int nextBrightness =
          level[i] + direction[i] * random(3, 13);

        // Occasional sharper flicker.
        if (random(100) < 4) {
          nextBrightness -= random(25, 61);
        }

        if (nextBrightness < 70) nextBrightness = 70;
        if (nextBrightness > 255) nextBrightness = 255;
        level[i] = nextBrightness;
      }

      pixels.setPixelColor(
        i,
        pixels.Color(
          (uint32_t)255 * level[i] / 255,
          (uint32_t)100 * level[i] / 255,
          (uint32_t)15  * level[i] / 255
        )
      );
    }

    pixels.show();
#endif

#ifdef HALLOWEEN 
    pixels.setBrightness(MAX_BRIGHTNESS);

    // EYES
    pixels.fill(PURPLE);
    for (int i = 0; i < 8; i++) {
      pixels.fill(ORANGE, NUM_PIXELS/3-3, 11);
      pixels.setPixelColor(NUM_PIXELS/3 - 2 + i, BLACK);
      pixels.setPixelColor(NUM_PIXELS/3 - 2 + i + 1, BLACK);

      pixels.fill(ORANGE, 2*NUM_PIXELS/3-3, 11);
      pixels.setPixelColor(2*NUM_PIXELS/3 - 2 + i, BLACK);
      pixels.setPixelColor(2*NUM_PIXELS/3 - 2 + i + 1, BLACK);

      pixels.show();
      delay(50);
    }

    delay(800);

    for (int i = 7; i >= 0; i--) {
      pixels.fill(ORANGE, NUM_PIXELS/3-3, 11);
      pixels.setPixelColor(NUM_PIXELS/3 - 2 + i, BLACK);
      pixels.setPixelColor(NUM_PIXELS/3 - 2 + i + 1, BLACK);

      pixels.fill(ORANGE, 2*NUM_PIXELS/3-3, 11);
      pixels.setPixelColor(2*NUM_PIXELS/3 - 2 + i, BLACK);
      pixels.setPixelColor(2*NUM_PIXELS/3 - 2 + i + 1, BLACK);
 
      pixels.show();
      delay(50);
    }
#endif
  } else {
    pixels.setBrightness(0);
    pixels.clear();
    pixels.show();
  }
}

#endif