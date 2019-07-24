// some definitions are in variables.h, the rest is here


#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     10  // Noise/hum/interference in mic signal
#define TOP (NUM_LEDS1 + 2) // Allow dot to go slightly off scale
#define MYSAMPLES 60
#define PEAK_FALL 10  // Rate of peak falling dot

int dotHangCount = 0; //Frame counter for holding peak dot
int level     = 0;
unsigned int sample;

byte
  peak      = 0,      // Used for falling dot
  dotCount  = 0,      // Frame counter for delaying dot-falling speed
  volCount  = 0;      // Frame counter for storing past volume data

int
  vol[MYSAMPLES],       // Collection of prior volume samples
  lvl       = 10,      // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;

void vuMeter(){
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;

  analogReadResolution(10);
  analogSetWidth(10);

  n   = analogRead(MICROPHONE_PIN);                        // Raw reading from mic
  //Serial.print("reading: ");
  //Serial.print(n);
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  //Serial.print(" | center: ");
  //Serial.print(n);
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  //Serial.print(" | lvl: ");
  //Serial.print(lvl);

    // Calculate bar height based on dynamic min/max levels (fixed point):
    height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);
    if(height < 0L) {
      height = 0;    // Clip output
    }else if(height > TOP){
      height = TOP;
    }
    if(height > peak){
      peak   = height;
    } // Keep 'peak' dot at top

    // Color pixels based on rainbow gradient
    int rainbowhue = 0;
    int rainbowhuedelta = 255/NUM_LEDS1;
    for(i=0; i<NUM_LEDS1-1; i++) {

      if(i >= height){
        leds1[i] = CRGB::Black;
      }else{

          leds1[i].setHue(rainbowhue);

      }
      rainbowhue += rainbowhuedelta;


    }

    if(peak > 0 && peak <= NUM_LEDS1-1){
      leds1[peak] = CRGB::Magenta;
    };

    Serial.print(" | peak: ");
    Serial.print(peak);

    // Every few frames, make the peak pixel drop by 1:
    Serial.print(" | dc: ");
    Serial.print(dotCount);
    if(++dotCount >= PEAK_FALL) { //fall rate
      if(peak > 0){
        peak--;
      }
      dotCount = 0;
    }

    vol[volCount] = n;                      // Save sample for dynamic leveling
    if(++volCount >= MYSAMPLES){
      volCount = 0; // Advance/rollover sample counter
    }
    // Get volume range of prior frames
    minLvl = maxLvl = vol[0];
    for(i=1; i<MYSAMPLES; i++) {
      if(vol[i] < minLvl)      minLvl = vol[i];
      else if(vol[i] > maxLvl) maxLvl = vol[i];
    }
    if((maxLvl - minLvl) < TOP){
      maxLvl = minLvl + TOP;
    }

    minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
    maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

    Serial.print(" | height: ");
    Serial.print(height);

    level = map(height,0,NUM_LEDS1, NUM_LEDS1, 0);

    Serial.print(" | level: ");
    Serial.println(level);

    newBrightness = map(level, 0,NUM_LEDS1, minBrightness, maxBrightness);
    //newBrightness = map(peak, 0,NUM_LEDS1, minBrightness, maxBrightness);
    if(newBrightness > maxBrightness){newBrightness = maxBrightness;};
    if(newBrightness < minBrightness){newBrightness = minBrightness;};

    if(newBrightness < 0){
      newBrightness = minBrightness;
    }
    Serial.print(" | Brightness: ");
    Serial.println(newBrightness/10);
    FastLED.setBrightness(newBrightness/10);
  }
