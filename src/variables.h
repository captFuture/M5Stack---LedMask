// FFT Spectrum analyzer definitions
#define SCALE 512
#define SAMPLES 1024              // Must be a power of 2
#define SAMPLING_FREQUENCY 80000
//// Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.

unsigned int sampling_period_us;
unsigned long microseconds;
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime, oldTime;
uint16_t tft_width  = 320; // ILI9341_TFTWIDTH;
uint16_t tft_height = 240; // ILI9341_TFTHEIGHT;
uint8_t bands = 8;
uint8_t bands_width = floor( tft_width / bands );
uint8_t bands_pad = bands_width - 10;
uint16_t colormap[255];

int enableMusic = 1;

uint8_t gHue = 0;
uint8_t selected = 0;
uint8_t selectedband = 1;
uint8_t switchValue = 1;
uint8_t switchAmount = 1;
uint8_t switchMaxValue = 7;
uint8_t modeValue = 1;
uint8_t modeMaxValue = 4;
uint8_t Brightness = 20;
uint8_t newBrightness;
int effectSpeed = 1000;
