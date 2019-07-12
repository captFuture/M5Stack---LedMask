#define SAMPLES 512
#define SAMPLING_FREQUENCY 40000

// includes for beat beatdetection
struct eqBand {
  const char *freqname;
  uint16_t amplitude;
  int peak;
  int lastpeak;
  uint16_t lastval;
  unsigned long lastmeasured;
};

eqBand audiospectrum[8] = {
  //Adjust the amplitude values to fit your microphone
  { "125Hz", 500, 0, 0, 0, 0},
  { "250Hz", 200, 0, 0, 0, 0},
  { "500Hz", 200, 0, 0, 0, 0},
  { "1KHz",  200, 0, 0, 0, 0},
  { "2KHz",  200, 0, 0, 0, 0},
  { "4KHz",  100, 0, 0, 0, 0},
  { "8KHz",  100, 0, 0, 0, 0},
  { "16KHz", 50,  0, 0, 0, 0}
};

unsigned int sampling_period_us;
unsigned long microseconds;
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime, oldTime;
uint16_t tft_width  = 320; // ILI9341_TFTWIDTH;
uint16_t tft_height = 240; // ILI9341_TFTHEIGHT;
uint8_t bands = 8;
uint8_t bands_width = floor( tft_width / bands );
uint8_t bands_pad = bands_width - 10;
uint16_t colormap[255]; // color palette for the band meter (pre-fill in setup)

// RGB Mask data output to LEDs is on pin 21
#define LED_PIN  21

// RGB Mask color order (Green/Red/Blue)
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B

// definitions for mic input amd enabling music brightness reaction
#define M5STACKFIRE_MICROPHONE_PIN 34
#define M5STACKFIRE_SPEAKER_PIN 25
int enableMusic = true;

int minVolume = 500;
int maxVolume = 50000;
int minSensitivity = 0;

int minBrightness = 10;
int maxBrightness = 255;
int power;
int newBrightness = minBrightness;

byte currentBrightness = minBrightness; // 0-255 will be scaled to 0-MAXBRIGHTNESS
byte currentSensitivity = minSensitivity;

#define NUMBEROFSAMPLES 1000
uint16_t micValue[NUMBEROFSAMPLES];

#include <M5Stack.h>
#include <FastLED.h>
#include "messages.h"
#include "font.h"
#include "XYmap.h"
#include "utils.h"
#include "effects.h"

#include "arduinoFFT.h"
arduinoFFT FFT = arduinoFFT();

#include "beatdetection.h"
#include <Adafruit_NeoPixel.h>

// includes for beat beatdetection result show
int num_led = 10;
int led_pin = 15;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(num_led, led_pin, NEO_GRB + NEO_KHZ800);

// Runs one time at the start of the program (power up or reset)
void setup() {
  M5.begin();
  M5.Lcd.setBrightness(20);
  dacWrite(25, 0);
  Serial.begin(115200);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, LAST_VISIBLE_LED + 1);
  FastLED.setBrightness( scale8(minBrightness, maxBrightness) );

  // includes for beat beatdetection
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(YELLOW, BLACK);
  M5.Lcd.setTextSize(1);
  //M5.Lcd.setRotation(0);
  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
  delay(2000);
  for(uint8_t i=0;i<tft_height;i++) {
    colormap[i] = M5.Lcd.color565(tft_height-i*.5, i*1.1, 0);
  }
  /*for (byte band = 0; band <= 7; band++) {
    M5.Lcd.setCursor(bands_width*band + 2, 0);
    M5.Lcd.print(audiospectrum[band].freqname);
  }*/
}

// list of functions that will be displayed
functionList effectList[] = {heart, threeSine,
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

// Timing parameters
#define cycleTime 15000
#define hueTime 30

void loop()
{

  if(enableMusic == true){
    //vuMeter();
    beatDetect();
  }else{
    FastLED.setBrightness(currentBrightness);
  }

  currentMillis = millis(); // save the current timer value

  if(M5.BtnA.wasPressed()){
    currentBrightness += 51; // increase the brightness (wraps to lowest)
    FastLED.setBrightness(scale8(currentBrightness,maxBrightness));
    Serial.print("Brightness: "); Serial.println(currentBrightness);
  }

  if(M5.BtnB.wasPressed()){
    cycleMillis = currentMillis;
    if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
    effectInit = false; // trigger effect initialization when new effect is selected
  }

  if(M5.BtnC.wasPressed()){
    if (++currentSensitivity >= 8) currentSensitivity = 0; // Change FFT frequency band to listen for
    Serial.print("Band: "); Serial.println(currentSensitivity);
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
    enableMusic = !enableMusic; // toggle auto cycle mode
    confirmBlink(); // one blue blink: auto mode. two red blinks: manual mode.
    Serial.print("Music: "); Serial.println(enableMusic);
    delay(500);
  }

  // switch to a new effect every cycleTime milliseconds
  if (currentMillis - cycleMillis > cycleTime && autoCycle == true) {
    cycleMillis = currentMillis;
    if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
    effectInit = false; // trigger effect initialization when new effect is selected
  }

  // increment the global hue value every hueTime milliseconds
  if (currentMillis - hueMillis > hueTime) {
    hueMillis = currentMillis;
    hueCycle(1); // increment the global hue value
  }

  // run the currently selected effect every effectDelay milliseconds
  if (currentMillis - effectMillis > effectDelay) {
    effectMillis = currentMillis;
    effectList[currentEffect](); // run the selected effect function
    random16_add_entropy(1); // make the random values a bit more random-ish
  }

  // run a fade effect too if the confetti effect is running
  if (effectList[currentEffect] == confetti) fadeAll(1);

  FastLED.show(); // send the contents of the led memory to the LEDs
  M5.update();
}
