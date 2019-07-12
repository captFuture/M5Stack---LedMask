/* ----------------- PATTERN STUFF ---------------------/
/  These are some patterns from different projects      /
/                                                       /
/------------------------------------------------------*/

void nextPattern()
{
  effectInit = false;
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
  Serial.print("playing pattern: ");

  Serial.println(gCurrentPatternNumber);

}

void fillcolor()
{
  if (effectInit == false) {
	   effectInit = true;
     effectDelay = 150;
  }

  static byte currentColor = 0;
  static byte currentRow = 0;
  static byte currentDirection = 0;
  uint16_t effectDelay = 45;
  currentPalette = RainbowColors_p;


  // test a bitmask to fill up or down when currentDirection is 0 or 2 (0b00 or 0b10)
  if (!(currentDirection & 1)) {
    effectDelay = 45; // slower since vertical has fewer pixels
    for (byte x = 0; x < kMatrixWidth; x++) {
      byte y = currentRow;
      if (currentDirection == 2) y = kMatrixHeight - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  // test a bitmask to fill left or right when currentDirection is 1 or 3 (0b01 or 0b11)
  if (currentDirection & 1) {
    effectDelay = 20; // faster since horizontal has more pixels
    for (byte y = 0; y < kMatrixHeight; y++) {
      byte x = currentRow;
      if (currentDirection == 3) x = kMatrixWidth - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  currentRow++;

  // detect when a fill is complete, change color and direction
  if ((!(currentDirection & 1) && currentRow >= kMatrixHeight) || ((currentDirection & 1) && currentRow >= kMatrixWidth)) {
    currentRow = 0;
    currentColor += random8(3, 6);
    if (currentColor > 15) currentColor -= 16;
    currentDirection++;
    if (currentDirection > 3) currentDirection = 0;
    effectDelay = 300; // wait a little bit longer after completing a fill
  }
}

void addGlitter( fract8 chanceOfGlitter)
{

  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void rainbow()
{
  if (effectInit == false) {
	   effectInit = true;
     effectDelay = 0;
  }

  fill_rainbow( leds, NUM_LEDS, gHue, 7);

}

void rainbowWithGlitter() // built-in FastLED rainbow, plus some random sparkly glitter
{
  rainbow();
  addGlitter(80);
}

void confetti() // random colored speckles that blink in and fade smoothly
{
  if (effectInit == false) {
	   effectInit = true;
     effectDelay = 0;
  }


  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon() // a colored dot sweeping back and forth, with fading trails
{
  if (effectInit == false) {
	   effectInit = true;
     effectDelay = 0;
  }


  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm() // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
{
  if (effectInit == false) {
	   effectInit = true;
     effectDelay = 100;
  }


  uint8_t BeatsPerMinute = 160;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);

  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() { // eight colored dots, weaving in and out of sync with each other
  if (effectInit == false) {
	   effectInit = true;
     effectDelay = 10;
  }


  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


void threeDee() { // Emulate 3D anaglyph glasses
  if (effectInit == false) {
	   effectInit = true;
     effectDelay = 10;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      if (x < 6) {
        leds[XY(x, y)] = CRGB::Blue;
      } else if (x > 6) {
        leds[XY(x, y)] = CRGB::Red;
      } else {
        leds[XY(x, y)] = CRGB::Black;
      }
    }
  }

  leds[XY(6, 0)] = CRGB::Black;
  leds[XY(9, 0)] = CRGB::Black;

}


void slantBars() { // Draw slanting bars scrolling across the array, uses current hue
  if (effectInit == false) {
	   effectInit = true;
     effectDelay = 10;
  }

  static byte slantPos = 0;
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(gHue, 255, quadwave8(x * 32 + y * 32 + slantPos));
    }
  }
  slantPos -= 4;
}

// Random pixels scroll sideways, uses current hue
#define rainDir 0
void sideRain() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 30;
  }

  scrollArray(rainDir);
  byte randPixel = random8(kMatrixHeight);
  for (byte y = 0; y < kMatrixHeight; y++) leds[XY((kMatrixWidth - 1) * rainDir, y)] = CRGB::Black;
  leds[XY((kMatrixWidth - 1)*rainDir, randPixel)] = CHSV(gHue, 255, 255);

}

void rider() { // Scanning pattern left/right, uses global hue cycle
  if (effectInit == false) {
  	effectInit = true;
    effectDelay = 10;
  }

  static byte riderPos = 0;
  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    int brightness = abs(x * (256 / kMatrixWidth) - triwave8(riderPos) * 2 + 127) * 3;
    if (brightness > 255) brightness = 255;
    brightness = 255 - brightness;
    CRGB riderColor = CHSV(gHue, 255, brightness);
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = riderColor;
    }
  }

  riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic
}

void plasma() { // RGB Plasma
  if (effectInit == false) {
  	effectInit = true;
    effectDelay = 10;
  }

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = cos8(plasVector / 256);
  int yOffset = sin8(plasVector / 256);

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 10 + xOffset - 127) + sq(((float)y - 2) * 10 + yOffset - 127)) + offset);
      leds[XY(x, y)] = CHSV(color, 255, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 16; // using an int for slower orbit (wraps at 65536)

}

void radiateCenter() {
  static byte offset  = 9; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

   // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 0;
}

  int xOffset = 15;
  int yOffset = -20;

   // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 12 + xOffset) + sq(((float)y - 2) * 12 + yOffset)) + offset);
      leds[XY(x, y)] = ColorFromPalette(RainbowColors_p, color, 255);
    }
  }
  offset--; // wraps at 255 for sin8
  plasVector += 1; // using an int for slower orbit (wraps at 65536)
}



#define NORMAL 0
#define RAINBOW 1
#define charSpacing 2
// Scroll a text string
void scrollText(byte message, byte style, CRGB fgColor, CRGB bgColor) {
  static byte currentMessageChar = 0;
  static byte currentCharColumn = 0;
  static byte paletteCycle = 0;
  static CRGB currentColor;
  static byte bitBuffer[16] = {0};
  static byte bitBufferPointer = 0;


  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 35;
    currentMessageChar = 0;
    currentCharColumn = 0;
    selectFlashString(message);
    loadCharBuffer(loadStringChar(message, currentMessageChar));
    currentPalette = RainbowColors_p;
    for (byte i = 0; i < kMatrixWidth; i++) bitBuffer[i] = 0;
  }


  paletteCycle += 15;

  if (currentCharColumn < 5) { // characters are 5 pixels wide
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = charBuffer[currentCharColumn]; // character
  } else {
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = 0; // space
  }

  CRGB pixelColor;
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < 5; y++) { // characters are 5 pixels tall
      if (bitRead(bitBuffer[(bitBufferPointer + x) % kMatrixWidth], y) == 1) {
        if (style == RAINBOW) {
          pixelColor = ColorFromPalette(currentPalette, paletteCycle+y*16, 255);
        } else {
          pixelColor = fgColor;
        }
      } else {
        pixelColor = bgColor;
      }
      leds[XY(x, y)] = pixelColor;
    }
  }

  currentCharColumn++;
  if (currentCharColumn > (4 + charSpacing)) {
    currentCharColumn = 0;
    currentMessageChar++;
    char nextChar = loadStringChar(message, currentMessageChar);
    if (nextChar == 0) { // null character at end of strong
      currentMessageChar = 0;
      nextChar = loadStringChar(message, currentMessageChar);
    }
    loadCharBuffer(nextChar);
  }

  bitBufferPointer++;
  if (bitBufferPointer > 15) bitBufferPointer = 0;

}


//Show emoticon

void filledheart(){
  const uint8_t pattern[] = {8,9,11,12,13,17,18,19,20,25,26,27,28,29,30,31,32,35,36,37,38,41,42,43,45,46};
  int patternSize = ARRAY_SIZE(pattern);
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    FastLED.clear();
  }
   for (int x = 0; x < patternSize; x++) {
    leds[pattern[x]] = CHSV(gHue, 255, 255);
   }
}

void heart(){
  const uint8_t pattern[] = {8,9,11,13,17,20,25,28,29,32,35,38,43,41,45,46};
  int patternSize = ARRAY_SIZE(pattern);
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    FastLED.clear();
  }
   for (int x = 0; x < patternSize; x++) {
    leds[pattern[x]] = CHSV(gHue, 255, 255);
   }
}

void smile(){
  const uint8_t pattern[] = {5,8,13,17,27,30,38,41,45};
  int patternSize = ARRAY_SIZE(pattern);
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    FastLED.clear();
  }
   for (int x = 0; x < patternSize; x++) {
    leds[pattern[x]] = CHSV(gHue, 255, 255);
   }
}
