// some definitions are in variables.h, the rest is here

#define SAMPLE_WINDOW   10    // Sample window for average level
#define PEAK_HANG 5          //Time of pause before peak dot falls
#define PEAK_FALL 3           //Rate of falling peak dot
#define INPUT_FLOOR 200       //Lower range of analogRead input
#define INPUT_CEILING 4095    //Max range of analogRead input, the lower the value the more sensitive (4095 = max)

int dotCount = 0;  //Frame counter for peak dot
int dotHangCount = 0; //Frame counter for holding peak dot
int peak = NUM_LEDS1-1;
unsigned int sample;



//Used to draw a line between two points of a given color
void drawLine(uint8_t from, uint8_t to) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int i=from; i<=to; i++){
    leds1[i] = CRGB::Black;
  }
}

// autoscaling for vu meter
float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve){
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;
  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;
  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }
  OriginalRange = originalMax - originalMin;
  if (newEnd > newBegin){
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }
  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float
  if (originalMin > originalMax ) {
    return 0;
  }
  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;
  }
  else     // invert the ranges
  {
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }
  return rangedValue;
}

void vuMeter(){
  unsigned long startMillis= millis();  // Start of sample window
  float peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  //unsigned int signalMin = 1023; //external mic
  unsigned int signalMin = 4095;
  unsigned int c, y, b;

  // collect data for length of sample window (in mS)
  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(MICROPHONE_PIN);
    if (sample < signalMin)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude

  b = fscale(INPUT_FLOOR, INPUT_CEILING, minBrightness, 255, peakToPeak, 2);

  newBrightness = b;
  if(newBrightness > maxBrightness){newBrightness = maxBrightness;};
  if(newBrightness < minBrightness){newBrightness = minBrightness;};

  if(newBrightness < 5){
    newBrightness = minBrightness;
  }

  FastLED.setBrightness(newBrightness);
  //FastLED.setBrightness(maxBrightness);

  //Fill the strip with rainbow gradient
  fill_rainbow( leds1, NUM_LEDS1, 0, 255/NUM_LEDS1 );

  //Scale the input logarithmically instead of linearly
  c = fscale(INPUT_FLOOR, INPUT_CEILING, NUM_LEDS1, 0, peakToPeak, 2);

  Serial.print("c: ");Serial.print(c); Serial.print("|");
  Serial.print("dhc1: ");Serial.print(dotHangCount); Serial.print("|");
  if(c <= peak) {
    peak = c;        // Keep dot on top
    Serial.print("dhc2: ");Serial.print(dotHangCount); Serial.print("|");
  }

  if (c <= NUM_LEDS1) { // Fill partial column with off pixels
    drawLine(NUM_LEDS1-1, NUM_LEDS1-c);
  }

  y = NUM_LEDS1 - peak;
  leds1[y-1] = CRGB::Magenta;
  Serial.print("peak: ");Serial.print(peak); Serial.print("|");
  Serial.print("dc: ");Serial.print(dotCount); Serial.print("|");

  Serial.println("dhc >=5");
    if(dotCount >= 3) { //Fall rate
      peak++;
      dotCount = 0;
    }
    dotCount++;
  Serial.print("dhc+|");
  Serial.print("dhc3: ");Serial.print(dotHangCount); Serial.print("|");
  Serial.println();
}
