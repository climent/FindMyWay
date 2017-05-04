//   Copyright (c) 2015 macetech LLC
//   This software is provided under the MIT License (see license.txt)
//   Special credit to Mark Kriegsman for XY mapping code

//   Use Version 3.0 or later https://github.com/FastLED/FastLED
//   ZIP file https://github.com/FastLED/FastLED/archive/master.zip
//
//   Use Arduino IDE 1.0 or later
//
//   [Press] the SW1 button to cycle through available effects
//   [Press and hold] the SW1 button (one second) to switch between auto and manual mode
//     * Auto Mode (one blue blink): Effects automatically cycle over time
//     * Manual Mode (two red blinks): Effects must be selected manually with SW1 button
//
//   [Press] the SW2 button to cycle through available brightness levels
//   [Press and hold] the SW2 button (one second) to reset brightness to startup value
//
//   Brightness, selected effect, and auto-cycle are saved in EEPROM after a delay
//   The RGB Shades will automatically start up with the last-selected settings


// RGB Shades data output to LEDs is on pin 5
#define LED_PIN  6

// RGB Shades color order (Green/Red/Blue)
#define COLOR_ORDER GRB
#define CHIPSET     WS2812

// Global maximum brightness value, maximum 255
#define MAXBRIGHTNESS 255
#define STARTBRIGHTNESS 102

// Cycle time (milliseconds between pattern changes)
#define cycleTime 10000

// Hue time (milliseconds between hue increments)
#define hueTime 30

// Time after changing settings before settings are saved to EEPROM
#define EEPROMDELAY 15000

// Include FastLED library and other useful files
#include <FastLED.h>
#include <EEPROM.h>
#include "messages.h"
#include "font.h"
#include "XYmap.h"
#include "utils.h"
#include "FireworksXY.h"
#include "effects.h"
#include "buttons.h"


// list of functions that will be displayed

// Normal patterns
functionList effectListOne[] = {
  fireworks,
  blurpattern,
  blurpattern2,
  sinisterSpiral,
  threeSine,
  snow,
  waves,
  waves2,
  //  waves3,
  xmasThreeDee,
  candycaneSlantbars,
  blurpattern,
  flash,
  checkerboard,
  rider,
  plasma,
  slantBars,
  confetti,
  sideRain,
  colorFill,
  glitter,
  spinPlasma,
};

// Christmas patterns
functionList effectListTwo[] = {
  scrollTextZero,
  scrollTextOne,
  scrollTextTwo,
  scrollTextThree,
  scrollTextFour,
};

byte numEffects;

// Runs one time at the start of the program (power up or reset)
void setup() {

  // check to see if EEPROM has been used yet
  // if so, load the stored settings
  byte eepromWasWritten = EEPROM.read(0);
  if (eepromWasWritten == 99) {
    currentEffect = EEPROM.read(1);
    autoCycle = EEPROM.read(2);
    currentBrightness = EEPROM.read(3);
    runMode = EEPROM.read(4);
  }

  // check if invalid value in EEPROM (not previously initialized)
  if (runMode > 3) {
    runMode = 0;
    saveEEPROMvals();
  }

  // write FastLED configuration data
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, LAST_VISIBLE_LED + 1);

  // set global brightness value
  FastLED.setBrightness( scale8(currentBrightness, MAXBRIGHTNESS) );

  // configure input buttons
  pinMode(MODEBUTTON, INPUT_PULLUP);
  pinMode(BRIGHTNESSBUTTON, INPUT_PULLUP);

  // read buttons once on startup
  if (getStartupButtons() != 0b11) {
    runMode = getStartupButtons();
    saveEEPROMvals();
  }

  // initialize effect count
  switch (runMode) {
    case 0: // normal patterns
      numEffects = (sizeof(effectListOne) / sizeof(effectListOne[0]));
      break;

    case 1: // Christmas patterns
      numEffects = (sizeof(effectListTwo) / sizeof(effectListTwo[0]));
      break;
  }

  if (currentEffect > (numEffects - 1)) currentEffect = 0;
  effectInit = false;
}


// Runs over and over until power off or reset
void loop()
{
  currentMillis = millis(); // save the current timer value

  // wait for any buttons pressed before powerup to be released
  if (initialized == false) {
    if (getStartupButtons() == 0b11) initialized = true; // no buttons are pressed
  } else {
    updateButtons();          // read, debounce, and process the buttons
    doButtons();              // perform actions based on button state
  }

  checkEEPROM();            // update the EEPROM if necessary

  // increment the global hue value every hueTime milliseconds
  if (currentMillis - hueMillis > hueTime) {
    hueMillis = currentMillis;
    hueCycle(1); // increment the global hue value
  }

  // run the currently selected effect every effectDelay milliseconds
  if (currentMillis - effectMillis > effectDelay) {
    effectMillis = currentMillis;

    switch (runMode) {
      case 0:
        effectListOne[currentEffect]();
        break;

      case 1:
        effectListTwo[currentEffect]();
        break;
    }

    random16_add_entropy(1); // make the random values a bit more random-ish
  }

  // switch to a new effect every cycleTime milliseconds
  if (repCount == 0) {
    if (currentMillis - cycleMillis > cycleTime && autoCycle == true) {
      cyclePattern();
    }
  }

  // run a fade effect
  if (fadingActive) fadeTo(fadeBaseColor, 1);

  FastLED.show(); // send the contents of the led memory to the LEDs
}

