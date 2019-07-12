#include <M5Stack.h>
#include <Fastled.h>
#include "variables.h"
#include "messages.h"
#include "font.h"
#include "XYmap.h"
#include "utils.h"
#include "effects.h"


FASTLED_USING_NAMESPACE

CRGB leds[ NUM_LEDS ];
CRGB leds1[ NUM_LEDS ];
CRGBPalette16 currentPalette;

void setup()
{
  M5.begin();
  M5.Lcd.setBrightness(displayBrightness);
  Serial.begin(115200);
  dacWrite(25, 0);

  FastLED.addLeds<LED_TYPE,DATA_PIN1,COLOR_ORDER>(leds1, NUM_LEDS1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(minBrightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);

}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {sideRain, radiateCenter, fillcolor, rainbowWithGlitter, confetti, sinelon, bpm, slantBars, rainbow};
//SimplePatternList gPatterns = {radiateCenter, heart, filledheart, smile, fillcolor, radiateCenter, rainbowWithGlitter, confetti, sinelon, bpm, slantBars, fillcolor, rainbow};
uint8_t gCurrentPatternNumber = 0;
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]));

#define NUMBEROFSAMPLES 1000
uint16_t micValue[NUMBEROFSAMPLES];

void loop()
{
  if(fireleds == 1){
    fill_rainbow( leds1, NUM_LEDS1, gHue, 7);
  }else{
    fill_solid(leds1,10, CRGB::Black);
  }
  currentMillis = millis();

  // run the currently selected effect every effectDelay milliseconds
  if (currentMillis - effectMillis > effectDelay) {
    effectMillis = currentMillis;
    gPatterns[gCurrentPatternNumber]();

  }

  if (currentMillis - displayMillis > displayTimeout) {
    displayMillis = currentMillis;
    M5.Lcd.setBrightness(0);
  }

  EVERY_N_MILLISECONDS( 20 ) { gHue++; };

  if(enableMusic == 1){
    uint32_t power = 0;
    uint32_t meanValue = 0;
    for (uint32_t n = 0; n < NUMBEROFSAMPLES; n++)
    {
      int value = analogRead(M5STACKFIRE_MICROPHONE_PIN);
      micValue[n] = value;
      meanValue += value;
      delayMicroseconds(20);
    }
    meanValue /= NUMBEROFSAMPLES;

    for (uint32_t n = 0; n < NUMBEROFSAMPLES; n++)
    {
      power += (micValue[n] - meanValue) * (micValue[n] - meanValue);
    }
    power /= NUMBEROFSAMPLES;
    if(power > maxVolume){power = maxVolume;};
    if(power < minVolume){power = 0;};
    power = map(power, minVolume, maxVolume, minBrightness, maxBrightness);
    newBrightness = power;
    if(newBrightness > maxBrightness){newBrightness = maxBrightness;};
    if(newBrightness < minBrightness){newBrightness = minBrightness;};
    FastLED.setBrightness(newBrightness);
  }else{
    FastLED.setBrightness(minBrightness);
  }

  if(autocycle == 1){
    EVERY_N_SECONDS( 10 ) { nextPattern(); }
  };

  FastLED.show();
  processButtons();
  DisplayGUI();

  M5.update();
}
