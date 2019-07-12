
uint16_t tft_width  = 320; // ILI9341_TFTWIDTH;
uint16_t tft_height = 240; // ILI9341_TFTHEIGHT;
#define M5STACKFIRE_MICROPHONE_PIN 34
#define M5STACKFIRE_SPEAKER_PIN 25

// FastLED internal definitions
#define DATA_PIN1    15
#define NUM_LEDS1    10

// FastLED exzernal definitions
#define DATA_PIN    21 //sda - PORTA
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB
#define NUM_LEDS    91
#define MILLI_AMPS  1000
#define FRAMES_PER_SECOND  120

unsigned long effectMillis = 0; // store the time of last effect function run
unsigned long displayMillis = 0; // store the time of last display
unsigned long displayTimeout = 10000;
unsigned long currentMillis; // store current loop's millis value
int displayBrightness = 10;

int minVolume = 500;
int maxVolume = 50000;

int fireleds = 0;

int enableMusic = 1;
int enableMusicOLD = 0;
int enableLeds = 1;
int autocycle = 1;

uint8_t gHue = 0;
int selected = 0;
int selectedband = 1;

int modeValue = 0;
int modeMaxValue = 6;

int minBrightness = 10;
int maxBrightness = 100;
int power;
int newBrightness = minBrightness;

int switchValue = 0;
int switchAmount = 1;
int switchMaxValue = 10;
int switchMinValue = 0;
String currentSetting = "Pattern";

String emoNames[] = {"heart", "filledheart", "smile"};
int currentEmo = 0;

// Effect variables
bool effectInit = false;
int effectDelay = 10;


const unsigned long
    REPEAT_FIRST(500),
    REPEAT_INCR(100);
