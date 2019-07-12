// Helper functions for a two-dimensional XY matrix of pixels.
// Special credit to Mark Kriegsman https://github.com/macetech/RGBShades

// Pixel layout
//
//      0      1      2      3      4       5       6       7       8       9       10      11     12
//   +------------------------------------------------------------------------------------------------
// 0 |  .      .      .      .      .       .       22      .       .       .       .       .      .
// 1 |  .      .      .      .      .       21      23      34      .       .       .       .      .
// 2 |  0      3      4      10     11      20      24      33      35      43      44      48     59
// 3 |  1      2      5      9      12      19      25      32      36      42      45      47     50
// 4 |  .      .      6      8      13      18      26      31      37      41      46      .      .
// 5 |  .      .      .      7      14      17      27      30      38      40      .       .      .
// 6 |  .      .      .      .      15      16      28      29      39      .       .       .      .

// Params for width and height
const uint8_t kMatrixWidth = 13;
const uint8_t kMatrixHeight = 7;
#define LAST_VISIBLE_LED 50
uint8_t XY( uint8_t x, uint8_t y)
{
  // any out of bounds address maps to the first hidden pixel
  if( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t ShadesTable[] = {
    51,60,61,67,68,70,22,71,72,76,77,85,86,
    52,59,62,66,69,21,23,34,73,75,78,84,87,
    0,3,4,10,11,20,24,33,35,43,44,48,59,
    1,2,5,9,12,19,25,32,36,42,45,47,50,
    53,58,6,8,13,18,26,31,37,41,46,83,88,
    54,57,63,7,14,17,27,30,38,40,79,82,89,
    55,56,64,65,15,16,28,29,39,74,80,81,90
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = ShadesTable[i];
  return j;
}
