// Helper functions for a two-dimensional XY matrix of pixels.
// Special credit to Mark Kriegsman
//
// 2014-10-18 - Special version for RGB Shades Kickstarter
//              https://www.kickstarter.com/projects/macetech/rgb-led-shades
//              2014-10-18 - code version 2c (local table, holes are r/w), 
//              by Mark Kriegsman 
//
//              This special 'XY' code lets you program the RGB Shades
//              as a plain 16x5 matrix.  
//
//              Writing to and reading from the 'holes' in the layout is 
//              also allowed; holes retain their data, it's just not displayed.
//
//              You can also test to see if you're on or off the layout
//              like this
//                if( XY(x,y) > LAST_VISIBLE_LED ) { ...off the layout...}
//
//              X and Y bounds checking is also included, so it is safe
//              to just do this without checking x or y in your code:
//                leds[ XY(x,y) ] == CRGB::Red;
//              All out of bounds coordinates map to the first hidden pixel.
//
//     XY(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;


// Params for width and height
const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds[ NUM_LEDS ];
#define LAST_VISIBLE_LED 255
uint16_t XY2 (uint8_t x, uint8_t y) {
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t XYTable[] = {
     0,  16,  32,  48,  64,  80,  96, 112, 128, 144, 160, 176, 192, 208, 224, 240,
     1,  17,  33,  49,  65,  81,  97, 113, 129, 145, 161, 177, 193, 209, 225, 241,
     2,  18,  34,  50,  66,  82,  98, 114, 130, 146, 162, 178, 194, 210, 226, 242,
     3,  19,  35,  51,  67,  83,  99, 115, 131, 147, 163, 179, 195, 211, 227, 243,
     4,  20,  36,  52,  68,  84, 100, 116, 132, 148, 164, 180, 196, 212, 228, 244,
     5,  21,  37,  53,  69,  85, 101, 117, 133, 149, 165, 181, 197, 213, 229, 245,
     6,  22,  38,  54,  70,  86, 102, 118, 134, 150, 166, 182, 198, 214, 230, 246,
     7,  23,  39,  55,  71,  87, 103, 119, 135, 151, 167, 183, 199, 215, 231, 247,
     8,  24,  40,  56,  72,  88, 104, 120, 136, 152, 168, 184, 200, 216, 232, 248,
     9,  25,  41,  57,  73,  89, 105, 121, 137, 153, 169, 185, 201, 217, 233, 249,
    10,  26,  42,  58,  74,  90, 106, 122, 138, 154, 170, 186, 202, 218, 234, 250,
    11,  27,  43,  59,  75,  91, 107, 123, 139, 155, 171, 187, 203, 219, 235, 251,
    12,  28,  44,  60,  76,  92, 108, 124, 140, 156, 172, 188, 204, 220, 236, 252,
    13,  29,  45,  61,  77,  93, 109, 125, 141, 157, 173, 189, 205, 221, 237, 253,
    14,  30,  46,  62,  78,  94, 110, 126, 142, 158, 174, 190, 206, 222, 238, 254,
    15,  31,  47,  63,  79,  95, 111, 127, 143, 159, 175, 191, 207, 223, 239, 255
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XYTable[i];
  return j;
}

// Trivial XY function for the SmartMatrix; use a different XY
// function for different matrix grids.  See XYMatrix example for code.
uint8_t XY( uint8_t x, uint8_t y) {
  return (y * kMatrixWidth) + x;
}

int deg(int i) {
  return ((i / 16) + ((i + 16) % 16 * 16));
} // This function is dependent on 16x16 matrix

