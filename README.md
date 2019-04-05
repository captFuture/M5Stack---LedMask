# M5Stack---LedMask
Use a M5Stack Fire to generate a Spectrum analyzer and drive WS8212B leds in a Diy FaceMask
I recently bought an Airsoft Facemask for putting some Leds into it to have a nice accessory for our frienship summertrip to Tomorrowland in Belgium.

This basically is a combination of several Projects and libraries

Graphic Equalizer on the M5Stack platform
https://github.com/ElectroMagus/M5-FFT

FastLed
https://github.com/FastLED/FastLED

RGBShades
https://github.com/macetech/RGBShades

In total there are 51 Leds in the Mask and currently the selected frequency band varies the brightness of the selected pattern.
The Fastled processing is running on Core 0, The sketch is running on core 1

The three buttons of the M5Stack fire execute different functions.

Left Button: Decrease value
Middle Button: cycle through the modes (select frequency band, change max brightness, select pattern)
Right Button: Increase value

ToDo:
More Patterns
Change Microphone gain in runtime
