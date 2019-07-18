# M5Stack---LedMask
Use a M5Stack Fire to generate a Spectrum analyzer and drive WS8212B leds in a Diy FaceMask and add a second VU Meter Neopixel strip.
I recently bought an Airsoft Facemask for putting some Leds into it to have a nice accessory for our frienship summertrip to Tomorrowland 2019 in Belgium.

This basically is a combination of several Projects and libraries

Graphic Equalizer on the M5Stack platform
https://macsbug.wordpress.com/2017/12/31/audio-spectrum-display-with-m5stack/

FastLed
https://github.com/FastLED/FastLED

RGBShades
https://github.com/macetech/RGBShades
(base code with mapping to varying led matrix shapes and effects)

Adafruit AmpliTie
https://github.com/adafruit/Adafruit_Learning_System_Guides/tree/master/LED_Ampli_Tie
(rewritten to use fastled instead of Adafruit_Neopixel for performance)

In total there are 51 Leds in the Mask and currently the selected frequency band varies the brightness of the selected pattern.

The three buttons of the M5Stack fire execute different functions.

Press Left Button: Toggle through different Brightness Modes for the static (not sound reactive) mode
Long Press Left Button: Reset Brightness to minimum

Press Middle Button: cycle through the patterns (random)
Long press middle Button: disable/enable autocycling through patterns

Press Right Button: cycle through spectrum analyzer bands to be used as master for led brightness control
Long Press Right Button: Switch reactive mode (1:vu-meter, 2:spectrum analyzer, 3:not reactive)

ToDo:
More Patterns
Find a way how to set setBrightness for mask and strip individually
