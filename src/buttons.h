// Process button inputs and return button activity

#define NUMBUTTONS 3
#define MODEBUTTON 38
#define BRIGHTNESSBUTTON 39
#define SENSITIVITYBUTTON 37

#define BTNIDLE 0
#define BTNDEBOUNCING 1
#define BTNPRESSED 2
#define BTNRELEASED 3
#define BTNLONGPRESS 4
#define BTNLONGPRESSREAD 5

#define BTNDEBOUNCETIME 20
#define BTNLONGPRESSTIME 1000

unsigned long buttonEvents[NUMBUTTONS];
byte buttonStatuses[NUMBUTTONS];
byte buttonmap[NUMBUTTONS] = {BRIGHTNESSBUTTON, MODEBUTTON, SENSITIVITYBUTTON};

void updateButtons() {
  for (byte i = 0; i < NUMBUTTONS; i++) {
    //Serial.print("update: "); Serial.println(buttonmap[i]);
    switch(buttonStatuses[i]) {
      case BTNIDLE:
        if (digitalRead(buttonmap[i]) == LOW) {
          buttonEvents[i] = currentMillis;
          buttonStatuses[i] = BTNDEBOUNCING;
        }
      break;

      case BTNDEBOUNCING:
        if (currentMillis - buttonEvents[i] > BTNDEBOUNCETIME) {
          if (digitalRead(buttonmap[i]) == LOW) {
            buttonStatuses[i] = BTNPRESSED;

          }
        }
      break;

      case BTNPRESSED:
        if (digitalRead(buttonmap[i]) == HIGH) {
          buttonStatuses[i] = BTNRELEASED;
          Serial.println(buttonmap[i]);
        } else if (currentMillis - buttonEvents[i] > BTNLONGPRESSTIME) {
            buttonStatuses[i] = BTNLONGPRESS;
        }
      break;

      case BTNRELEASED:
      break;

      case BTNLONGPRESS:
      break;

      case BTNLONGPRESSREAD:
        if (digitalRead(buttonmap[i]) == HIGH) {
          buttonStatuses[i] = BTNIDLE;
        }
      break;
    }
  }
}

byte buttonStatus(byte buttonNum) {
  //Serial.print("Button: "); Serial.println(buttonNum);
  byte tempStatus = buttonStatuses[buttonNum];
  if (tempStatus == BTNRELEASED) {
    buttonStatuses[buttonNum] = BTNIDLE;
    Serial.print("Button: "); Serial.println(buttonNum);
  } else if (tempStatus == BTNLONGPRESS) {
    buttonStatuses[buttonNum] = BTNLONGPRESSREAD;
  }

  return tempStatus;

}
