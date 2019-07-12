void processButtons(){
  static int lastswitchValue(-1);
  static unsigned long rpt(REPEAT_FIRST);
  enum states_t {WAIT, INCR, DECR};
  static states_t STATE;

  if (switchValue != lastswitchValue){
      displayMillis = currentMillis;
      M5.Lcd.setBrightness(displayBrightness);
      M5.Lcd.fillScreen(BLACK);

      lastswitchValue = switchValue;
      //Serial.println();
      Serial.print("Current value: ");
      Serial.println(switchValue, DEC);

      switch(modeValue){
          case 0: //
            gCurrentPatternNumber = switchValue;
            Serial.print("playing Pattern: ");
            Serial.println(gCurrentPatternNumber);
            currentSetting = "--Pattern--";
            effectInit = false;
            currentEmo = 0;
            autocycle = 0;
          break;
          case 1: //
            maxBrightness = switchValue;
            Serial.print("max Brightness: ");
            Serial.println(maxBrightness);
            currentSetting = "--Max Brightness--";
          break;
          case 2: //
            minBrightness = switchValue;
            Serial.print("min Brightness: ");
            Serial.println(minBrightness);
            currentSetting = "--Min Brightness--";
          break;
          case 3: //
            enableMusic = switchValue;
            Serial.print("Enable Music reaction: ");
            Serial.println(enableMusic);
            currentSetting = "--React to Music--";
          break;
          case 4: //
            fireleds = switchValue;
            Serial.print("Fireleds: ");
            Serial.println(fireleds);
            currentSetting = "--Fireleds--";
          break;
          case 5: //
            autocycle = switchValue;
            Serial.print("Autocycle: ");
            Serial.println(autocycle);
            currentSetting = "--Autocycle--";
          break;
          case 6: //
            effectDelay = switchValue;
            Serial.print("Delay: ");
            Serial.println(effectDelay);
            currentSetting = "--Delay--";
          break;
          default:
          break;
        };
  }

  switch (STATE)
      {
          case WAIT:                              // wait for a button event
              if (M5.BtnC.wasReleased())
              {
                STATE = INCR;
              }
              else if (M5.BtnB.wasReleased())
              {
                modeValue = modeValue+1;
                if(modeValue > modeMaxValue){
                 modeValue = 0;
                };
                Serial.println();
                Serial.print("Mode: ");
                Serial.print(modeValue);
                Serial.print(" - ");
                STATE = WAIT;

                switch(modeValue){
                    case 0: // Pattern to play
                      Serial.print("Change Pattern: "); Serial.println(gCurrentPatternNumber);
                      switchValue = gCurrentPatternNumber;
                      switchMinValue = 0;
                      switchMaxValue = ARRAY_SIZE(gPatterns)-2;
                      switchAmount = 1;
                    break;

                    case 1: // max Brightness of leds
                      Serial.print("Change max Brightness: "); Serial.println(maxBrightness);
                      switchMinValue = 0;
                      switchMaxValue = 255;
                      switchValue = maxBrightness;
                      switchAmount = 5;
                    break;

                    case 2: // minBrightness of leds
                      Serial.println("Change min Brightness:"); Serial.println(minBrightness);
                      switchMinValue = 0;
                      switchValue = minBrightness;
                      switchMaxValue = 50;
                      switchAmount = 5;
                    break;

                    case 3: // React to Music
                      Serial.print("React to Music: ");  Serial.println(enableMusic);
                      switchValue = enableMusic;
                      switchMinValue = 0;
                      switchMaxValue = 1;
                      switchAmount = 1;
                    break;

                    case 4: // Leds of M5stack fire
                      Serial.print("FireLEDS: "); Serial.println(fireleds);
                      switchValue = fireleds;
                      switchMinValue = 0;
                      switchMaxValue = 1;
                      switchAmount = 1;
                    break;

                    case 5: // Autocycle
                      Serial.print("Autocycle: "); Serial.println(autocycle);
                      switchValue = autocycle;
                      switchMinValue = 0;
                      switchMaxValue = 1;
                      switchAmount = 1;
                    break;

                    case 6: // effectDelay
                      Serial.print("Delay: "); Serial.println(effectDelay);
                      switchValue = effectDelay;
                      switchMinValue = 0;
                      switchMaxValue = 1000;
                      switchAmount = 10;
                    break;
                    default:
                    break;

                }
              }
              else if (M5.BtnA.wasReleased())
              {
                STATE = DECR;
              }
              break;

        case INCR:                            // increment the switchValueer
          //Serial.println("Increase: ");
          switchValue = min(switchValue, switchMaxValue);    // but not more than the specified maximum
          switchValue = switchValue + switchAmount;
          if (switchValue > switchMaxValue){
            switchValue = switchMinValue;
          }
          STATE = WAIT;
          break;

        case DECR:                            // decrement the switchValueer
          //Serial.println("Decrease ");
          switchValue = max(switchValue, switchMinValue);    // but not less than the specified minimum
          switchValue = switchValue - switchAmount;
          if (switchValue < switchMinValue){
            switchValue = switchMaxValue;
          }

          STATE = WAIT;
          break;
      }
};


void DisplayGUI(){

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10); M5.Lcd.print("Current Brightness: "); M5.Lcd.print(newBrightness);
  M5.Lcd.setCursor(10, 30); M5.Lcd.print(minBrightness); M5.Lcd.print(" < Brightness > "); M5.Lcd.print(maxBrightness);
  M5.Lcd.setCursor(10, 50); M5.Lcd.print(minVolume); M5.Lcd.print(" < Volume > "); M5.Lcd.print(maxVolume);
  M5.Lcd.setCursor(10, 70); M5.Lcd.print("Current Pattern: "); M5.Lcd.print(gCurrentPatternNumber);

  M5.Lcd.setCursor(10, 90); M5.Lcd.print("Autocycle: "); M5.Lcd.print(autocycle);
  M5.Lcd.setCursor(10, 110); M5.Lcd.print("React to Music: "); M5.Lcd.print(enableMusic);
  M5.Lcd.setCursor(10, 130); M5.Lcd.print("Effect Delay: "); M5.Lcd.print(effectDelay);


  M5.Lcd.setCursor(10, 200); M5.Lcd.print(currentSetting);
};
