void vuMeter(){
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
}

byte getBand(int i) {
  if (i<=2 )             return 0;  // 125Hz
  if (i >3   && i<=5 )   return 1;  // 250Hz
  if (i >5   && i<=7 )   return 2;  // 500Hz
  if (i >7   && i<=15 )  return 3;  // 1000Hz
  if (i >15  && i<=30 )  return 4;  // 2000Hz
  if (i >30  && i<=53 )  return 5;  // 4000Hz
  if (i >53  && i<=200 ) return 6;  // 8000Hz
  if (i >200           ) return 7;  // 16000Hz
  return 8;
}

void displayBand(int band, int dsize){
  uint16_t hpos = bands_width*band;
  int dmax = 100;
  if(dsize>tft_height-50) {
    dsize = tft_height-50; // leave some hspace for text
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

  if(band == currentSensitivity){
    newBrightness = dsize;
    newBrightness = map(newBrightness, 0, 255, minBrightness, maxBrightness);
    FastLED.setBrightness(newBrightness);
  }

}

void beatDetect(){
  for (int i = 0; i < SAMPLES; i++) {
    newTime = micros()-oldTime;
    oldTime = newTime;
    vReal[i] = analogRead(M5STACKFIRE_MICROPHONE_PIN); // A conversion takes about 1uS on an ESP32
    vImag[i] = 0;
    while (micros() < (newTime + sampling_period_us)) {
      // do nothing to wait
    }
  }
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  for (int i = 2; i < (SAMPLES/2); i++){
    if (vReal[i] > 1500) {
      byte bandNum = getBand(i);
      if(bandNum!=8) {
        displayBand(bandNum, (int)vReal[i]/audiospectrum[bandNum].amplitude);
      }
    }
  }

  long vnow = millis();
  for (byte band = 0; band <= 7; band++) {
    // auto decay every 50ms on low activity bands
    if(vnow - audiospectrum[band].lastmeasured > 50) {
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
     M5.Lcd.drawFastHLine(bands_width*band, tft_height-audiospectrum[band].peak,
                           bands_pad, colormap[tft_height-audiospectrum[band].peak]);
    }
  }
}
