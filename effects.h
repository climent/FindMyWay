//   Graphical effects to run on the RGB Shades LED array
//   Each function should have the following components:
//    * Must be declared void with no parameters or will break function pointer array
//    * Check effectInit, if false then init any required settings and set effectInit true
//    * Set effectDelay (the time in milliseconds until the next run of this effect)
//    * All animation should be controlled with counters and effectDelay, no delay() or loops
//    * Pixel data should be written using leds[XY(x,y)] to map coordinates to the RGB Shades layout

// Triple Sine Waves
void threeSine() {

  static byte sineOffset = 0; // counter for current position of sine waves

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 20;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {

      // Calculate "sine" waves with varying periods
      // sin8 is used for speed; cos8, quadwave8, or triwave8 would also work here
      byte sinDistanceR = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 9 + x * 16)), 2);
      byte sinDistanceG = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 10 + x * 16)), 2);
      byte sinDistanceB = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 11 + x * 16)), 2);

      leds[XY(x, y)] = CRGB(255 - sinDistanceR, 255 - sinDistanceG, 255 - sinDistanceB);
    }
  }

  sineOffset++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle

}


// RGB Plasma
void plasma() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = cos8(plasVector / 256);
  int yOffset = sin8(plasVector / 256);

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 10 + xOffset - 127) + sq(((float)y - 2) * 10 + yOffset - 127)) + offset);
      leds[XY(x, y)] = CHSV(color, 255, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 16; // using an int for slower orbit (wraps at 65536)

}


// Scanning pattern left/right, uses global hue cycle
void rider() {

  static byte riderPos = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    riderPos = 0;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    int brightness = abs(x * (256 / kMatrixWidth) - triwave8(riderPos) * 2 + 127) * 3;
    if (brightness > 255) brightness = 255;
    brightness = 255 - brightness;
    CRGB riderColor = CHSV(cycleHue, 255, brightness);
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = riderColor;
    }
  }

  riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic
}


// Shimmering noise, uses global hue cycle
void glitter() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
  }

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, random8(5) * 63);
    }
  }
}


// Fills saturated colors into the array from alternating directions
void colorFill() {

  static byte currentColor = 0;
  static byte currentRow = 0;
  static byte currentDirection = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 45;
    currentColor = 0;
    currentRow = 0;
    currentDirection = 0;
    currentPalette = RainbowColors_p;
  }

  // test a bitmask to fill up or down when currentDirection is 0 or 2 (0b00 or 0b10)
  if (!(currentDirection & 1)) {
    effectDelay = 45; // slower since vertical has fewer pixels
    for (byte x = 0; x < kMatrixWidth; x++) {
      byte y = currentRow;
      if (currentDirection == 2) y = kMatrixHeight - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  // test a bitmask to fill left or right when currentDirection is 1 or 3 (0b01 or 0b11)
  if (currentDirection & 1) {
    effectDelay = 20; // faster since horizontal has more pixels
    for (byte y = 0; y < kMatrixHeight; y++) {
      byte x = currentRow;
      if (currentDirection == 3) x = kMatrixWidth - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  currentRow++;

  // detect when a fill is complete, change color and direction
  if ((!(currentDirection & 1) && currentRow >= kMatrixHeight) || ((currentDirection & 1) && currentRow >= kMatrixWidth)) {
    currentRow = 0;
    currentColor += random8(3, 6);
    if (currentColor > 15) currentColor -= 16;
    currentDirection++;
    if (currentDirection > 3) currentDirection = 0;
    effectDelay = 300; // wait a little bit longer after completing a fill
  }
}

// Emulate 3D anaglyph glasses
void threeDee() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 50;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      if (x < 7) {
        leds[XY(x, y)] = CRGB::Blue;
      } else if (x > 8) {
        leds[XY(x, y)] = CRGB::Red;
      } else {
        leds[XY(x, y)] = CRGB::Black;
      }
    }
  }

  leds[XY(6, 0)] = CRGB::Black;
  leds[XY(9, 0)] = CRGB::Black;
}

// Random pixels scroll sideways, uses current hue
#define rainDir 0
void sideRain() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 30;
  }

  scrollArray(rainDir);
  byte randPixel = random8(kMatrixHeight);
  for (byte y = 0; y < kMatrixHeight; y++) leds[XY((kMatrixWidth - 1) * rainDir, y)] = CRGB::Black;
  leds[XY((kMatrixWidth - 1)*rainDir, randPixel)] = CHSV(cycleHue, 255, 255);

}

// Pixels with random locations and random colors selected from a palette
// Use with the fadeAll function to allow old pixels to decay
void confetti() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
    fadingActive = true;
    fadeBaseColor = CRGB::Black;
  }

  // scatter random colored pixels at several random coordinates
  for (byte i = 0; i < 4; i++) {
    leds[XY(random16(kMatrixWidth), random8(kMatrixHeight))] = ColorFromPalette(currentPalette, random16(255), 255); //CHSV(random16(255), 255, 255);
    random16_add_entropy(1);
  }
}


// Draw slanting bars scrolling across the array, uses current hue
void slantBars() {

  static byte slantPos = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, quadwave8(x * 16 + y * 16 + slantPos));
    }
  }

  slantPos -= 4;
}


byte repCount = 0;

#define charSpacing 2
// Scroll a text string
void scrollText(byte message, byte style, CRGB fgColor, CRGB bgColor, byte repeats) {
  static byte currentMessageChar = 0;
  static byte currentCharColumn = 0;
  static byte paletteCycle = 0;
  static CRGB currentColor;
  static byte currentWordCount = 0;
  static byte currentChar;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 35;
    currentMessageChar = 0;
    currentCharColumn = 0;
    selectFlashString(message);
    repCount = repeats;
    currentChar = loadStringChar(message, currentMessageChar);
    loadCharBuffer(currentChar);
    if (style == RAINBOW) {
      currentPalette = RainbowColors_p;
    } else if (style == PALETTEWORDS) {
      currentPalette = RainbowColors_p;
    }
    paletteCycle = 0;
    if (style == NORMAL) {
      currentColor = fgColor;
    } else if (style == PALETTEWORDS) {
      currentColor = ColorFromPalette(currentPalette, paletteCycle, 255);
    } else if (style == CANDYCANE || style == HOLLY || style == HOLLY2) {
      currentColor = colorCycle(style);
    }

    fillAll(CRGB::Black);
  }

  CRGB pixelColor;

  scrollArray(1);
  if (style == RAINBOW) paletteCycle += 10;

  for (byte y = 0; y < 16; y++) { // characters are 5 pixels tall
    if ((bitRead(charBuffer[currentCharColumn], y) == 1) && currentCharColumn < 5) {
      if (style == RAINBOW) {
        pixelColor = ColorFromPalette(currentPalette, paletteCycle + y * 16, 255);
      } else {
        pixelColor = currentColor;
      }
    } else {
      pixelColor = bgColor;
    }
    leds[XY(kMatrixWidth - 1, y)] = pixelColor;
  }

  currentCharColumn++;
  if (currentCharColumn > (4 + charSpacing)) {
    currentCharColumn = 0;
    currentMessageChar++;
    char nextChar = loadStringChar(message, currentMessageChar);
    if (nextChar == 0) { // null character at end of string
      currentMessageChar = 0;
      if (repCount > 0) repCount--;
      if (repCount == 0) cyclePattern();
      nextChar = loadStringChar(message, currentMessageChar);
    }


    if (currentChar == ' ' && nextChar != ' ') {
      if (style == PALETTEWORDS) {
        paletteCycle += 15;
        currentColor = ColorFromPalette(currentPalette, paletteCycle * 15, 255);
      } else if (style == CANDYCANE || style == HOLLY) {
        currentColor = colorCycle(style);
      }
    }

    if (currentChar != ' ') {
      if (style == HOLLY2) currentColor = colorCycle(HOLLY);
    }


    loadCharBuffer(nextChar);
    currentChar = nextChar;
  }
}


// RotatingPlasma
void spinPlasma() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
    fadingActive = false;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = (cos8(plasVector) - 127) / 2;
  int yOffset = (sin8(plasVector) - 127) / 2;

  //int xOffset = 0;
  //int yOffset = 0;


  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 12 + xOffset) + sq(((float)y - 2) * 12 + yOffset)) + offset);
      leds[XY(x, y)] = ColorFromPalette(currentPalette, color, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 1; // using an int for slower orbit (wraps at 65536)

}



// setup for text scrolling
// parameters: string number, style, fg color, bg color, number of repeats
void scrollTextZero() {
  scrollText(0, HOLLY, CRGB::Red, CRGB::Black, 3);
}

void scrollTextOne() {
  scrollText(1, CANDYCANE, 0, CRGB::Black, 10);
}

void scrollTextTwo() {
  scrollText(2, HOLLY2, CRGB::Green, CRGB::Black, 3);
}

void scrollTextThree() {
  scrollText(3, RAINBOW, 0, 0, 3);
}

void scrollTextFour() {
  scrollText(4, PALETTEWORDS, CRGB::Magenta, CRGB::Black, 6);
}


// Display bursts of sparks
void fireworks() {

  byte sparksDone = 0;
  static int sparkLife = 50;
  static boolean boom = false;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    gSkyburst = 1;
    fadingActive = false;
  }


  if (boom) {
    FastLED.clear();
    boom = false;
  } else {
    fadeAll(40);
  }

  if (sparkLife > 0) sparkLife--;


  for ( byte b = 0; b < NUM_SPARKS; b++) {
    if (sparkLife <= 0) gSparks[b].show = 0;
    gSparks[b].Move();
    gSparks[b].Draw();
    sparksDone += gSparks[b].show;
  }

  if (sparksDone == 0) gSkyburst = 1;
  //Serial.println(sparksDone);

  if ( gSkyburst) {
    effectDelay = 5;
    sparkLife = random(16, 150);
    CRGB color;
    hsv2rgb_rainbow( CHSV( random8(), 255, 255), color);
    accum88 sx = random(127 - 64, 127 + 64) << 8;
    accum88 sy = random(127 - 16, 127 + 16) << 8;
    for ( byte b = 0; b < NUM_SPARKS; b++) {
      gSparks[b].Skyburst(sx, sy, 0, color);
    }
    gSkyburst = 0;
    sparksDone = 0;
    fillAll(CRGB::Gray);
    boom = true;
  }

}


// Show alternating red and green lenses
void xmasThreeDee() {

  static boolean swap = false;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 250;
    fadingActive = false;
  }

  swap = !swap;

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      if (x < 7) {
        leds[XY(x, y)] = swap ? CRGB::Blue : CRGB::Red;
      } else if (x > 8) {
        leds[XY(x, y)] = swap ? CRGB::Red : CRGB::Blue;
      } else {
        leds[XY(x, y)] = CRGB::Black;
      }
    }
  }
  //  leds[XY(6, 0)] = CRGB::Black;
  //  leds[XY(9, 0)] = CRGB::Black;
}


// Smoothly falling white dots
void snow() {

  static unsigned int snowCols[kMatrixWidth] = {0};

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 20;
    fadingActive = false;
  }

  CRGB snowColor = CRGB::White;

  FastLED.clear();

  for (int i = 0; i < kMatrixWidth; i++) {
    if (snowCols[i] > 0) {
      snowCols[i] += random(4, 16);
    } else {
      if (random8(0, 100) == 0) snowCols[i] = 1;
    }
    byte tempY = snowCols[i] >> 8;
    byte tempRem = snowCols[i] & 0xFF;
    if (tempY <= kMatrixHeight) leds[deg(XY(i, tempY - 1))] = snowColor % dim8_raw(255 - tempRem);
    if (tempY < kMatrixHeight) leds[deg(XY(i, tempY))] = snowColor % dim8_raw(tempRem);
    if (tempY > kMatrixHeight) snowCols[i] = 0;
  }
}


// Draw slanting bars scrolling across the array, uses current hue
void candycaneSlantbars() {

  static byte slantPos = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    fadingActive = false;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = blend(CRGB::Red, CRGB::White, cubicwave8(x * 32 + y * 32 + slantPos));
    }
  }

  slantPos -= 4;

}

// Crossfading alternate colors
DEFINE_GRADIENT_PALETTE( checkermap_gp) {
  0,   0,   0,  0,
  63, 255,   0,  0,
  127,   0,   0,  0,
  191,   0, 255,  0,
  255,   0,   0,  0
};

void flash() {
  uint8_t number = random8();
  fadeToBlackBy(leds, NUM_LEDS, 255);
  leds[number] = CRGB::White;
}

void checkerboard() {
  static byte checkerFader = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    currentPalette = checkermap_gp;
    fadingActive = false;
  }

  checkerFader += 2;


  CRGB colorOne = ColorFromPalette(currentPalette, checkerFader);
  CRGB colorTwo = ColorFromPalette(currentPalette, checkerFader + 64);

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = (((x % 2) + y) % 2) ? colorOne : colorTwo;
      //leds[XY(x,y)] = (y % 2) ? colorOne : colorTwo;
      //leds[XY(x,y)] = (x % 2) ? colorOne : colorTwo;
    }
  }
}

void blurpattern()
{
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    fadingActive = false;
  }

  // Apply some blurring to whatever's already on the matrix
  // Note that we never actually clear the matrix, we just constantly
  // blur it repeatedly.  Since the blurring is 'lossy', there's
  // an automatic trend toward black -- by design.
  uint8_t blurAmount = beatsin8(2, 10, 255);
  blur2d( leds, kMatrixWidth, kMatrixHeight, blurAmount);

  // Use two out-of-sync sine waves
  uint8_t  i = beatsin8( 27, 0, kMatrixHeight);
  uint8_t  j = beatsin8( 41, 0, kMatrixWidth);
  // Also calculate some reflections
  uint8_t ni = (kMatrixWidth - 1) - i;
  uint8_t nj = (kMatrixWidth - 1) - j;

  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();
  leds[XY( i, j)] += CHSV( ms / 11, 200, 255);
  leds[XY( j, i)] += CHSV( ms / 13, 200, 255);
  leds[XY(ni, nj)] += CHSV( ms / 17, 200, 255);
  leds[XY(nj, ni)] += CHSV( ms / 29, 200, 255);
  leds[XY( i, nj)] += CHSV( ms / 37, 200, 255);
  leds[XY(ni, j)] += CHSV( ms / 41, 200, 255);
}

const uint8_t kBorderWidth = 0;
const uint8_t kSquareWidth = 16;

void blurpattern2()
{
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    fadingActive = false;
  }

  // Apply some blurring to whatever's already on the matrix
  // Note that we never actually clear the matrix, we just constantly
  // blur it repeatedly.  Since the blurring is 'lossy', there's
  // an automatic trend toward black -- by design.
  uint8_t blurAmount = dim8_raw( beatsin8(3, 64, 64) );
  blur2d( leds, kMatrixWidth, kMatrixWidth, blurAmount);

  // Use three out-of-sync sine waves
  uint8_t  i = beatsin16(  91 / 2, kBorderWidth, kSquareWidth - kBorderWidth);
  uint8_t  j = beatsin16( 109 / 2, kBorderWidth, kSquareWidth - kBorderWidth);
  uint8_t  k = beatsin16(  73 / 2, kBorderWidth, kSquareWidth - kBorderWidth);

  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();
  leds[XY( i, j)] += CHSV( ms / 29, 200, 255);
  leds[XY( j, k)] += CHSV( ms / 41, 200, 255);
  leds[XY( k, i)] += CHSV( ms / 73, 200, 255);
}

void waves() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    fadingActive = false;
  }

  uint8_t blurAmount = dim8_raw( beatsin8(3, 64, 192) );      // A sinewave at 3 Hz with values ranging from 64 to 192.
  blur1d(leds, NUM_LEDS, blurAmount);                         // Apply some blurring to whatever's already on the strip, which will eventually go black.
  //  blur2d(leds, 16, 16, blurAmount);

  uint8_t  i = beatsin16( 9, 0, NUM_LEDS);
  uint8_t  j = beatsin16( 7, 0, NUM_LEDS);
  uint8_t  k = beatsin16( 5, 0, NUM_LEDS);

  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();
  leds[deg((i + j) / 2)] = CHSV( ms / 29, 200, 255);
  leds[deg((j + k) / 2)] = CHSV( ms / 41, 200, 255);
  leds[deg((k + i) / 2)] = CHSV( ms / 73, 200, 255);
  leds[deg((k + i + j) / 3)] = CHSV( ms / 53, 200, 255);
} // loop()

void waves2() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    fadingActive = true;
    fadeBaseColor = CRGB::Black;
  }

  fadeAll(1);
  uint8_t blurAmount = dim8_raw( beatsin8(3, 64, 192) );      // A sinewave at 3 Hz with values ranging from 64 to 192.
  blurAmount = 10;
  //    blur1d(leds, NUM_LEDS, blurAmount);                         // Apply some blurring to whatever's already on the strip, which will eventually go black.
  blur2d(leds, 16, 16, blurAmount);
  //  blurpattern();

  uint8_t  i = beatsin16( 9, 0, NUM_LEDS);
  uint8_t  j = beatsin16( 7, 0, NUM_LEDS);
  uint8_t  k = beatsin16( 5, 0, NUM_LEDS);

  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();
  leds[deg((i + j) / 2)] = CHSV( ms / 29, 200, 255);
  leds[deg((j + k) / 2)] = CHSV( ms / 41, 200, 255);
  leds[deg((k + i) / 2)] = CHSV( ms / 73, 200, 255);
  //  leds[deg((k + i + j) / 3)] = CHSV( ms / 53, 200, 255);
}

void waves3() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    fadingActive = false;
  }

  fadeAll(1);
  uint8_t blurAmount = dim8_raw( beatsin8(3, 64, 192) );      // A sinewave at 3 Hz with values ranging from 64 to 192.
  blurAmount = 10;
  ///    blur1d(leds, NUM_LEDS, blurAmount);                         // Apply some blurring to whatever's already on the strip, which will eventually go black.
  blur2d(leds, 16, 16, blurAmount);
  //  blurpattern();

  uint8_t  i = beatsin16( 9, 0, NUM_LEDS);
  uint8_t  j = beatsin16( 7, 0, NUM_LEDS);
  uint8_t  k = beatsin16( 5, 0, NUM_LEDS);

  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();
  leds[deg((i + j) / 2)] = CHSV( ms / 29, 200, 255);
  leds[deg((j + k) / 2)] = CHSV( ms / 41, 200, 255);
  leds[deg((k + i) / 2)] = CHSV( ms / 73, 200, 255);
  //  leds[deg((k + i + j) / 3)] = CHSV( ms / 53, 200, 255);
}


void sinisterSpiral()
{
  //Play with these values to customize the spiral
  static byte pulseWaveTick = 0;
  static byte vert = 1; //down (use -1 for up)
  static byte wavelength = 8;
  static byte frequencyMultiplier = 1;
  static byte hFreq = 7;
  static byte rFreq = 4;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    fadingActive = false;
  }

  uint16_t ms = millis();

  //Pixels up
  for (byte x = 0; x < kMatrixHeight; x++)
  {
    //Pixels around beacon
    for (byte y = 0; y < kMatrixWidth; y++)
    {
      byte sinCalc = ((y * wavelength * rFreq) + (vert * pulseWaveTick) + (x * wavelength * hFreq)) * frequencyMultiplier;
      byte sinVal = sin8(sinCalc);

      //If end brt val is less than 15, set to 0 as LEDs can’t do low values well
      if (sinVal < 15)
      {
        sinVal = 0;
      }

      //Up/Down Waves
      leds[XY( x, y)] = CHSV( ms / 37 + (x * 5), 255, sinVal);

      //Sweeper Waves
      //leds[XYsafe( x, y)] = CHSV( ms / 37 + (y * 5), 255, sinVal));
    }
  }

  pulseWaveTick = pulseWaveTick + 8;
}

void matrixConsole() {
  EVERY_N_MILLIS(75) // falling speed
  {
    // move code downward
    // start with lowest row to allow proper overlapping on each column
    for (int8_t row = kMatrixHeight - 1; row >= 0; row--)
    {
      for (int8_t col = 0; col < kMatrixWidth; col++)
      {
        if (leds[deg(XY(col, row))] == CRGB(175, 255, 175))
        {
          leds[deg(XY(col, row))] = CRGB(27, 130, 39); // create trail
          if (row < kMatrixHeight - 1) leds[deg(XY(col, row + 1))] = CRGB(175, 255, 175);
        }
      }
    }

    // fade all leds
    for (int i = 0; i < NUM_LEDS; i++) {
      if (leds[i].g != 255) leds[i].nscale8(192); // only fade trail
    }

    // check for empty screen to ensure code spawn
    bool emptyScreen = true;
    for (int i = 0; i < NUM_LEDS; i++) {
      if (leds[i])
      {
        emptyScreen = false;
        break;
      }
    }

    // spawn new falling code
    if (random8(3) == 0 || emptyScreen) // lower number == more frequent spawns
    {
      int8_t spawnX = random8(kMatrixWidth);
      leds[deg(XY(spawnX, 0))] = CRGB(175, 255, 175 );
    }
  }
}

