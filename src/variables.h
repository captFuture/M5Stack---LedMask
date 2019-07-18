/* definitions for Hardware setup */
#define LED_PIN  21             // RGB Mask data output to LEDs is on pin 21
#define COLOR_ORDER GRB         // RGB Mask color order (Green/Red/Blue)
#define CHIPSET     WS2812B
#define MICROPHONE_PIN 34       //internal mic M5stack fire
#define SPEAKER_PIN 25

int minBrightness = 2;              //minimal brightness of Mask LEDs when reacting to music
int maxBrightness = 255;            //maximal brightness of Mask LEDs when reacting to music
int newBrightness = minBrightness;  //set the LED brightness initially
byte currentBrightness = maxBrightness;
int enableMusic = 2;                //set the mode to music reactive initially
int power;                          //
int minBand = 0;                    //
byte currentBand = minBand;         //spectrum analyzer band to react to

// Timing parameters
#define cycleTime 15000             //time to keep actual pattern in autoCycle mode
#define hueTime 30

/* definitions for second Ledstrip on PORTC */
#define NUM_LEDS1  22          // Number of pixels in strand
#define LED_PIN1    16         // NeoPixel LED strand is connected to PORTC of M5Stack
