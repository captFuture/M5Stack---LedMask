
#include "arduinoFFT.h"          // Standard Arduino FFT library
arduinoFFT FFT = arduinoFFT();
#include <M5Stack.h>
#include <Fastled.h>

FASTLED_USING_NAMESPACE

#define SCALE 512
#define SAMPLES 1024              // Must be a power of 2
#define SAMPLING_FREQUENCY 80000
//// Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    22
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB
#define NUM_LEDS    51
#define MILLI_AMPS  1000

CRGB leds[NUM_LEDS];
uint8_t gHue = 0;
uint8_t selected = 0;
uint8_t selectedband = 1;
uint8_t switchValue = 1;
uint8_t switchMaxValue = 7;
uint8_t modeValue = 1;
uint8_t modeMaxValue = 3;

uint8_t Brightness = 20;
uint8_t newBrightness;
#define FRAMES_PER_SECOND  120

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
  // Lower values increase sensetivity to the freq
  // Can be used to compensate for ambient noise in certain situations
  { "63Hz", 50, 0, 0, 0, 0},
  { "160Hz", 40, 0, 0, 0, 0},
  { "400Hz", 40, 0, 0, 0, 0},
  { "1KHz",  40, 0, 0, 0, 0},
  { "2.5KHz",  25, 0, 0, 0, 0},
  { "6.2KHz",  25, 0, 0, 0, 0},
  { "10KHz",  65, 0, 0, 0, 0},
  { "20KHz", 55,  0, 0, 0, 0}
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

// Tasks stuff
#define FASTLED_SHOW_CORE 0
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle = 0;

void FastLEDshowESP32()
{
  if (userTaskHandle == 0) {
    userTaskHandle = xTaskGetCurrentTaskHandle();
    xTaskNotifyGive(FastLEDshowTaskHandle);
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 200 );
    ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    userTaskHandle = 0;
  }
}

void FastLEDshowTask(void *pvParameters)
{
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    FastLED.show();
    xTaskNotifyGive(userTaskHandle);
  }
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]));

void setup() {
  Serial.begin(115200);
  M5.begin();
  dacWrite(25, 0); // Speaker OFF
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(BLUE, BLACK);
  M5.Lcd.setTextSize(1);
  //M5.Lcd.setRotation(0);

  // ADC setup
  pinMode(35, INPUT);
  analogReadResolution(50);        // Resolution of 0-511
  analogSetWidth(50);              // Resolution of 0-511
  analogSetCycles(4);             // Number of cycles per sample.  Function of the SAR ADC on the ESP32.  Increases accuracy at cost of performance
  analogSetSamples(4);            // Number of samples for the result.  Function of the SAR ADC on the ESP32.  Increases accuracy at cost of performance.
  analogSetClockDiv(1);           // Clock divider for timing
  analogSetAttenuation(ADC_0db);  // ADC Gain.   Valid:  0, 2_5, 6, 11
  adcAttachPin(35);               // Set the pin your microphone is attached to.

  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
  for(uint8_t i=0;i<tft_height;i++) {
    colormap[i] = M5.Lcd.color565(tft_height-i, tft_height+i, i);
  }
  for (byte band = 0; band <= 7; band++) {
    M5.Lcd.setCursor(bands_width*band + 2, 0);
    M5.Lcd.print(audiospectrum[band].freqname);

  }

  int core = xPortGetCoreID();
  Serial.print("Main code running on core ");
  Serial.println(core);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(Brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);

  xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2, &FastLEDshowTaskHandle, FASTLED_SHOW_CORE);
}

// Graphics for the Equalizer
void displayBand(int band, int dsize){

  if(band == selectedband){selected = 0;}else{selected = 20;};
  uint16_t hpos = bands_width*band;
  int dmax = 200;
  if(dsize>tft_height-10) {
    dsize = tft_height-10; // leave some hspace for text
  }
  if(dsize < audiospectrum[band].lastval) {
    // lower value, delete some lines
    M5.Lcd.fillRect(hpos, tft_height-audiospectrum[band].lastval,
                    bands_pad, audiospectrum[band].lastval - dsize, BLACK);
  }
  if (dsize > dmax) dsize = dmax;
  for (int s = 0; s <= dsize; s=s+4){
    M5.Lcd.drawFastHLine(hpos, tft_height-s, bands_pad, colormap[tft_height-s]);
  }
  if (dsize > audiospectrum[band].peak) {
    audiospectrum[band].peak = dsize;
  }
  audiospectrum[band].lastval = dsize;
  audiospectrum[band].lastmeasured = millis();
}


// Associates the frequency to the appropriate band being displayed.
// If you modify the resolution of the ADC, be sure to update these appropriately.
// These were loosley calibrated using a tone generator.
byte getBand(int i) {
  if (i<=4 )              return 0;  // 125Hz
  if (i >6   && i<=10 )   return 1;  // 250Hz
  if (i >10   && i<=14 )  return 2;  // 500Hz
  if (i >14   && i<=30 )  return 3;  // 1000Hz
  if (i >30  && i<=60 )   return 4;  // 2000Hz
  if (i >60  && i<=106 )  return 5;  // 4000Hz
  if (i >106  && i<=400 ) return 6;  // 8000Hz
  if (i >400           )  return 7;  // 16000Hz
  return 8;
}



void loop() {
  for (int i = 0; i < SAMPLES; i++) {
    adcStart(35);
    newTime = micros()-oldTime;
    oldTime = newTime;
    vReal[i] = adcEnd(35);
    vImag[i] = 0;
  }
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  for (int i = 2; i < (SAMPLES/2); i++){
    // Each array element represents a frequency and its value the amplitude.
    if (vReal[i] > 500) { // Add a crude noise filter
      byte bandNum = getBand(i);
      if(bandNum!=8) {
        displayBand(bandNum, (int)vReal[i]/audiospectrum[bandNum].amplitude);
      }
    }
  }
  long vnow = millis();
  for (byte band = 0; band <= 7; band++) {
    // auto decay every 135ms on low activity bands
    if(vnow - audiospectrum[band].lastmeasured > 135) {
      displayBand(band, audiospectrum[band].lastval>4 ? audiospectrum[band].lastval-4 : 0);
    }
    if (audiospectrum[band].peak > 0) {
      audiospectrum[band].peak -= 2;
      if(audiospectrum[band].peak<=0) {
        audiospectrum[band].peak = 0;
      }
    }
    // only draw if peak changed
    if(audiospectrum[band].lastpeak != audiospectrum[band].peak) {
    // delete last peak
      M5.Lcd.drawFastHLine(bands_width*band,tft_height-audiospectrum[band].lastpeak,bands_pad,BLACK);
      audiospectrum[band].lastpeak = audiospectrum[band].peak;
      M5.Lcd.drawFastHLine(bands_width*band, tft_height-audiospectrum[band].peak,bands_pad, colormap[tft_height-audiospectrum[band].peak]);
    }


    if(band == selectedband){
      //newBrightness = audiospectrum[band].peak;
      newBrightness = audiospectrum[band].lastval;
      newBrightness = map(newBrightness, 0, 255, 0, Brightness);
      if(modeValue == 1){ Serial.print("EQ for Band: "); Serial.print(band); Serial.print(" peak: "); Serial.println(audiospectrum[band].peak); };

    }

  }

  //rainbow();
  gPatterns[gCurrentPatternNumber]();
  FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND);
  EVERY_N_MILLISECONDS( 20 ) { gHue++; };
  FastLED.setBrightness(newBrightness);
  FastLEDshowESP32();


  if(M5.BtnA.wasReleased()){
    switchValue = switchValue-1;
    if(switchValue <= 0){
      switchValue = switchMaxValue;
    };
    Serial.print("switchValue: ");
    Serial.println(switchValue);

    switch (modeValue){
      case 1:
        // Change Spectrum analyzer Band
        selectedband = switchValue;
        break;

      case 2:
        // Change overall Brightness
        Brightness = switchValue;
        break;

      case 3:
        // Change playing pattern
        gCurrentPatternNumber = switchValue-1;
        Serial.print("playing Pattern: ");
        Serial.println(gCurrentPatternNumber);
        break;

      default:

        break;
    }
  };

  if(M5.BtnB.wasReleased()){
    modeValue = modeValue+1;
    if(modeValue > modeMaxValue){
      modeValue = 1;
    };
    Serial.print("modeValue: ");
    Serial.println(modeValue);

    switch (modeValue){
      case 1:
        // Change Spectrum analyzer Band
        switchValue = selectedband;
        switchMaxValue = 7;
        break;

      case 2:
        // Change overall Brightness
        switchMaxValue = 255;
        switchValue = Brightness;
        break;

      case 3:
        // Change playing pattern
        switchValue = gCurrentPatternNumber+1;
        switchMaxValue = ARRAY_SIZE(gPatterns);
        break;

      default:
        switchValue = 1;
        switchMaxValue = 2;
        break;
    }
  };

  if(M5.BtnC.wasReleased()){
    switchValue = switchValue+1;
    if(switchValue > switchMaxValue){
      switchValue = 1;
    };
    Serial.print("switchValue: ");
    Serial.println(switchValue);

    switch (modeValue){
      case 1:
        // Change Spectrum analyzer Band
        selectedband = switchValue;
        break;

      case 2:
        // Change overall Brightness
        Brightness = switchValue;
        break;

      case 3:
        // Change playing pattern
        gCurrentPatternNumber = switchValue-1;
        Serial.print("playing Pattern: ");
        Serial.println(gCurrentPatternNumber);
        break;

      default:

        break;
    }
  };
  M5.update();
}





// PATTERN STUFF -------------------
void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void rainbow()
{
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
