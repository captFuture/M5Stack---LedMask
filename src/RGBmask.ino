#include "variables.h"
#include "FS.h"
#include "SPIFFS.h"
#include <M5Stack.h>
#include <FastLED.h>
CRGB leds1[NUM_LEDS1];

#include "messages.h"
#include "font.h"
#include "XYmap.h"
#include "utils.h"
#include "effects.h"

//includes for spectrum analyzer

#include "arduinoFFT.h"
#include "spectrum.h"

//includes for vu meter
#include <math.h>
#include "vumeter.h"

#define imgTml tmllogo
#define PicArray extern unsigned char
PicArray imgTml[];

void setup() {
  M5.begin();
  M5.Lcd.setBrightness(60);
  dacWrite(SPEAKER_PIN, 0); // switch off speaker
  Serial.begin(115200);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.addLeds<CHIPSET, LED_PIN1, COLOR_ORDER>(leds1, NUM_LEDS1);
  FastLED.setBrightness( scale8(minBrightness, maxBrightness) );

  M5.Lcd.fillScreen(TFT_BLACK);
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS Mount Failed");
  }else{
    Serial.println("SPIFFS Mount Success");
    M5.Lcd.drawJpgFile(SPIFFS, "/tml_logo.jpg", 0, 0);
  }
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.print("Mode: ");
  M5.Lcd.print(enableMusic);

  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
  delay(2000);

  for(uint8_t i=0;i<tft_height;i++) {
    colormap[i] = M5.Lcd.color565(tft_height-i*.5, i*1.1, 0);
    selectedColormap[i] = M5.Lcd.color565(tft_height-i*.5, 0, i*1.1);
  }
  randomSeed(analogRead(1));
}

// list of functions that will be displayed
functionList effectList[] = {
                              //sad,
                             filledheart,
                             smile,
                             heart,
                             threeSine,
                             radiateCenter,
                             scrollTextZero,
                             threeDee,
                             plasma,
                             confetti,
                             rider,
                             scrollTextOne,
                             glitter,
                             slantBars,
                             scrollTextTwo,
                             colorFill,
                             sideRain };

void loop()
{
  if(enableMusic == 1){
    vuMeter();
    //Serial.println("Musicmode-VUmeter");
  }else if(enableMusic == 2){
    /*for (byte band = 0; band <= 7; band++) {
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor(bands_width*band + 2, 0);
      M5.Lcd.print(audiospectrum[band].freqname);
    }*/
    beatDetect();
    //Serial.println("Musicmode-FFT");
  }else{
    FastLED.setBrightness(currentBrightness);
    fill_rainbow(leds1, NUM_LEDS1, cycleHue);
    //Serial.println("Musicmode-Fix");
  }

  currentMillis = millis(); // save the current timer value

  if(M5.BtnA.wasPressed()){
    currentBrightness += 51; // increase the brightness (wraps to lowest)
    FastLED.setBrightness(scale8(currentBrightness,maxBrightness));
    maxBrightness = currentBrightness;
    Serial.print("Brightness: "); Serial.println(currentBrightness);
  }

  if(M5.BtnB.wasPressed()){
    cycleMillis = currentMillis;
    if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
    selectRandomPalette();
    effectInit = false; // trigger effect initialization when new effect is selected
  }

  if(M5.BtnC.wasPressed()){
    if (++currentBand >= 8) currentBand = 0; // Change FFT frequency band to listen for
    Serial.print("Band: "); Serial.println(currentBand);
    FastLED.setBrightness(scale8(currentBrightness,maxBrightness));
  }


  if(M5.BtnA.pressedFor(1000)){
    currentBrightness = minBrightness; // reset brightness to startup value
    FastLED.setBrightness(scale8(currentBrightness,maxBrightness));
    Serial.print("Brightness: "); Serial.println(currentBrightness);
    confirmBlink();
    delay(500);
  }

  if(M5.BtnB.pressedFor(1000)){
    autoCycle = !autoCycle; // toggle auto cycle mode
    confirmBlink(); // one blue blink: auto mode. two red blinks: manual mode.
    Serial.print("AutoCycle: "); Serial.println(autoCycle);
    delay(500);
  }

  if(M5.BtnC.pressedFor(1000)){
    enableMusic += 1;
    if(enableMusic > 2){enableMusic = 0;};

    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.drawJpgFile(SPIFFS, "/tml_logo.jpg", 0, 0);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30, 10);
    M5.Lcd.print("Mode: ");
    M5.Lcd.print(enableMusic);

    confirmBlink(); // one blue blink: auto mode. two red blinks: manual mode.
    Serial.print("Music: "); Serial.println(enableMusic);
    delay(500);
  }

  // switch to a new effect every cycleTime milliseconds
  if (currentMillis - cycleMillis > cycleTime && autoCycle == true) {
    cycleMillis = currentMillis;
    selectRandomPalette();
    //if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
    currentEffect = random(0, numEffects-1);
    effectInit = false;
  }

  // increment the global hue value every hueTime milliseconds
  if (currentMillis - hueMillis > hueTime) {
    hueMillis = currentMillis;
    hueCycle(1); // increment the global hue value
  }

  if(enableMusic == 1){
    if (currentMillis - effectMillis > musicEffectDelay) {
      effectMillis = currentMillis;
      effectList[currentEffect]();
      random16_add_entropy(1);
    }
  }else if(enableMusic == 2){
    if (currentMillis - effectMillis > musicEffectDelay) {
      effectMillis = currentMillis;
      effectList[currentEffect]();
      random16_add_entropy(1);
    }
  }else{
    if (currentMillis - effectMillis > effectDelay) {
      effectMillis = currentMillis;
      effectList[currentEffect]();
      random16_add_entropy(1);
    }
  }

  // run a fade effect too if the confetti effect is running
  if (effectList[currentEffect] == confetti) fadeAll(1);

  FastLED.show(); // send the contents of the led memory to the LEDs
  M5.update();
}
