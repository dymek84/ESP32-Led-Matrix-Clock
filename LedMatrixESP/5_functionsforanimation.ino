/*
   Below are many functions that create a 'background' animation
   mostly taken from  https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#FastLEDFramework

*/

// ShowStrip is used to start the fastled.show() but allows to add a few customizable things
void showStrip() {

  FastLED.setBrightness(overAllBrightness); // affects the overall brightness of the strip.
  if (Nachtlamp == true || nightMode == true) {
    // ignore background dimming
    showTime(timeColor, 128);  //they are already dimmed a lot
  }else{
    fadeToBlackBy( leds, NUM_LEDS, backgroundBrightness);
    showTime(timeColor, digitBrightness);  // shows the time, and dims the numbers based on the setting.
  }
  FastLED.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
  // leds[Pixel].fadeToBlackBy( 128 );
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  //Serial.println("does this work");
  showStrip();

}


void oneColorBackground(CRGB color, int bgBrightness) {
  //  set background color
  for (int y = 0; y < YRES; y++) {
    for (int x = 0; x < XRES; x++) {
      leds[xyToLedNumber(x, y)] = color;
      leds[xyToLedNumber(x, y)].fadeToBlackBy( bgBrightness );
    }
  }
  showStrip();
}


void spinningRainbow() {
  // variable used for the initial hue of the rainbow
  // we start it out at 0
  // but since it's static, it'll keep it's value as we change it
  static byte initialHue = 0;

  // increase the hue by 1 each time
  initialHue = initialHue + 1;

  // the amount we want the hue to change between each LED
  // by dividing the number of hues (255), by the number of LEDs,
  // this code makes each LED a different color
  // and covers the entire rainbow spectrum (red, orange, yellow, green, blue, indigo, violet)
  byte changeInHue = 255 / NUM_LEDS;

  // use FastLED to fill the LEDs with a rainbow
  //FastLED.setBrightness(64);
  fill_rainbow(leds, NUM_LEDS, initialHue, changeInHue);
}

void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(random(NUM_LEDS), red, green, blue);
    //  showTime();
    showStrip();
    if (breakAnimation) {
      breakAnimation = false;
      break;
    }
    FastLED.delay(SpeedDelay);
    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  // showTime();
  FastLED.delay(SpeedDelay);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
    showStrip();
    if (breakAnimation) {
      breakAnimation = false;
      break;
    }
    FastLED.delay(SpeedDelay);

    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  FastLED.delay(SpeedDelay);
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel, red, green, blue);
  showStrip();
  FastLED.delay(SpeedDelay);
  setPixel(Pixel, 0, 0, 0);
  setAll(0, 0, 0);
}

void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(red, green, blue);

  int Pixel = random(NUM_LEDS);
  setPixel(Pixel, 0xff, 0xff, 0xff);
  showStrip();
  FastLED.delay(SparkleDelay);
  setPixel(Pixel, red, green, blue);
  showStrip();
  FastLED.delay(SpeedDelay);
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position = 0;

  for (int j = 0; j < NUM_LEDS * 2; j++)
  {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < NUM_LEDS; i++) {
      // sine wave, 3 offset waves make a rainbow!
      //float level = sin(i+Position) * 127 + 128;
      //setPixel(i,level,0,0);
      //float level = sin(i+Position) * 127 + 128;
      setPixel(i, ((sin(i + Position) * 127 + 128) / 255)*red,
               ((sin(i + Position) * 127 + 128) / 255)*green,
               ((sin(i + Position) * 127 + 128) / 255)*blue);
    }

    showStrip();
    if (breakAnimation) {
      // breakAnimation = false;  //this function is often used in conjunction with other functions.. set true after the function where you call it
      break;
    }
    FastLED.delay(WaveDelay);
  }
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
    showStrip();
    if (breakAnimation) {
      // breakAnimation = false;  //this function is often used in conjunction with other functions.. set true after the function where you call it
      break;
    }
    FastLED.delay(SpeedDelay);
  }
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < NUM_LEDS; i++) {
      c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c + 1), *(c + 2));
    }
    showStrip();
    FastLED.delay(SpeedDelay);
    if (breakAnimation) {
      breakAnimation = false;
      break;
    }
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}

void theaterChaseRainbow(int SpeedDelay) {
  byte *c;

  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        c = Wheel( (i + j) % 255);
        setPixel(i + q, *c, *(c + 1), *(c + 2)); //turn every third pixel on
      }
      showStrip();

      FastLED.delay(SpeedDelay);

      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        setPixel(i + q, 0, 0, 0);    //turn every third pixel off
      }
    }
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  setAll(0, 0, 0);

  for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {


    // fade brightness all LEDs one step
    for (int j = 0; j < NUM_LEDS; j++) {
      if ( (!meteorRandomDecay) || (random(10) > 5) ) {
        fadeToBlack(j, meteorTrailDecay );
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ( ( i - j < NUM_LEDS) && (i - j >= 0) ) {
        setPixel(i - j, red, green, blue);
      }
    }

    showStrip();
    if (breakAnimation) {
      breakAnimation = false;
      break;
    }
    FastLED.delay(SpeedDelay);
  }
}

void BouncingColoredBalls(int BallCount, byte colors[][3]) {
  float Gravity = -5.81;
  int StartHeight = 1;

  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];

  for (int i = 0 ; i < BallCount ; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
  }

  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i] / 1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if ( Height[i] < 0 ) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }

    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i], colors[i][0], colors[i][1], colors[i][2]);
    }

    showStrip();
    setAll(0, 0, 0);

    /** some animations will go indefinitly and need some hand to stop **/
    if (breakAnimation) {
      breakAnimation = false;
      break;
    }
  }
}

void nachtmode() {
  // Serial.println("good night..");
  CRGB ledcol = timeColor;
  //while (nightMode) {
  timeColor = CHSV(0, 255, 32);
  oneColorBackground(CRGB::Black, 250);
  //FastLED.delay(1000);
  //  if (breakAnimation) {
  //    breakAnimation = false;
  //    break;
  //  }
  //}
  timeColor = ledcol;
 // showStrip();
}

/* run a random animation for 1 minute every ... hour, half hour, etc
*/

void runRandomAnimation(int every) {
  boolean runAnimation = false;
  hour = timeinfo.tm_hour; // get the hour as int.
  minute = timeinfo.tm_min; // get the minutes as int.
  second = timeinfo.tm_sec; // get the seconds
  int minStart = minute;
  //Serial.print("minute ="); Serial.println(minute);

  if (every == 60) {
    if (minute == 0) {
      // Serial.println("Random animation starting every hour");
      runAnimation = true;
    } else {
      runAnimation = false;
    }
  } else if (every == 30) {
    if (minute == 0 || minute == 30) {
      //  Serial.println("Random animation starting every 30 minutes");
      runAnimation = true;
    } else {
      runAnimation = false;
    }
  } else if (every == 15) {
    //Serial.println("Random animation starting evry 15 minutes");
    if (minute == 0 || minute == 15 || minute == 30 || minute == 45) {
      runAnimation = true;
    } else {
      runAnimation = false;
    }
  } else {
    runAnimation = false;
  }


  int randomNumber = rand() % 11 + 1;
  while (runAnimation) {
    if (randomNumber == 1) {
      Serial.println("showing: Rainbow");
      rainbowCycle(20);
    }
    else if (randomNumber == 2) {
      Serial.println("showing: Sparkle");
      Sparkle(0xff, 0xff, 0xff, 0);
    }
    else if (randomNumber == 3) {
      Serial.println("showing: Running lights");
      setAll(0, 0, 0);
      RunningLights(0xff, 0, 0, 50);      // red
      RunningLights(0xff, 0xff, 0xff, 50); // white
      RunningLights(0, 0, 0xff, 50);      // blue
      breakAnimation = false;
    }
    else if (randomNumber == 4) {
      Serial.println("showing: Color Whipe");
      setAll(0, 0, 0);
      colorWipe(0x00, 0xff, 0x00, 50);
      colorWipe(0x00, 0x00, 0x00, 50);
      breakAnimation = false;
    }
    else if (randomNumber == 5) {
      Serial.println("showing: Snow Sparkle");
      SnowSparkle(0x10, 0x10, 0x10, 20, random(100, 1000));
    }
    else if (randomNumber == 6) {
      Serial.println("showing: Random Twinkle");
      setAll(0, 0, 0);
      TwinkleRandom(40, 200, false);

    }
    else if (randomNumber == 7) {
      Serial.println("showing: MeteoRain");
      meteorRain(0xff, 0xff, 0xff, 10, 64, true, 30);
    }
    else if (randomNumber == 8) {
      Serial.println("showing: Confetti");
      ChangeMe();
      EVERY_N_MILLISECONDS(thisdelay) {   // FastLED based non-blocking delay to update/display the sequence.
        confetti();
      }
      showStrip();
    }
    else if (randomNumber == 9) {
      Serial.println("showing: Plasma");
      EVERY_N_MILLISECONDS(50) {                                  // FastLED based non-blocking delay to update/display the sequence.
        plasma();
      }
      EVERY_N_SECONDS(5) {                                 // Change the target palette to a random one every 5 seconds.
        uint8_t baseC = random8();                         // You can use this as a baseline colour if you want similar hues in the next line.
        targetPalette = CRGBPalette16(CHSV(baseC + random8(32), 192, random8(128, 255)), CHSV(baseC + random8(32), 255, random8(128, 255)), CHSV(baseC + random8(32), 192, random8(128, 255)), CHSV(baseC + random8(32), 255, random8(128, 255)));
      }
      showStrip();
    }
    else if (randomNumber == 10) {
      currentPalette = OceanColors_p;                                   // Use palettes instead of direct CHSV or CRGB assignments
      targetPalette = OceanColors_p;                                    // Also support smooth palette transitioning

      EVERY_N_MILLISECONDS(50) {                                                      // Smooth palette transitioning runs continuously.
        uint8_t maxChanges = 24;
        nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
      }

      EVERY_N_SECONDS(5) {
        SetupMySimilar4Palette();                                                     // Change palette colours every 5 seconds.
      }

      EVERY_N_MILLIS(50) {                                                            // Sets the original delay time.
        rippless();                                                                   // Run the ripple routine.
      }
      showStrip();
    }
    else if (randomNumber == 11) {
      currentPalette = RainbowColors_p;
      beatwave();

      EVERY_N_MILLISECONDS(100) {
        uint8_t maxChanges = 24;
        nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
      }

      EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
        targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
      }

      showStrip();
    }

    if (minute == 0 || minute == 15 || minute == 30 || minute == 45) {

    } else {
      Serial.println("End random animation");
      runAnimation = false;
    }
  }

  // incase of no random... just standard animation of nothingness
  oneColorBackground(backgroundColor, 128);
}


void fadeToBlack(int ledNo, byte fadeValue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 256);

  strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[ledNo].fadeToBlackBy( fadeValue );
#endif
}

// EFFECTS ARE TAKEN FROM https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#FastLEDFramework

// The next fx come from https://github.com/atuline/FastLED-Demos/blob/master/confetti/confetti.ino
void confetti() {                                             // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, thisfade);                    // Low values = slower fade.
  int pos = random16(NUM_LEDS);                               // Pick an LED at random.
  leds[pos] += CHSV((thishue + random16(huediff)) / 4 , thissat, thisbri); // I use 12 bits for hue so that the hue increment isn't too quick.
  thishue = thishue + thisinc;                                // It increments here.
} // confetti()

void ChangeMe() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.
  uint8_t secondHand = (millis() / 1000) % 15;                // IMPORTANT!!! Change '15' to a different value to change duration of the loop.
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch (secondHand) {
      case  0: thisinc = 1; thishue = 192; thissat = 255; thisfade = 2; huediff = 256; break; // You can change values here, one at a time , or altogether.
      case  5: thisinc = 2; thishue = 128; thisfade = 8; huediff = 64; break;
      case 10: thisinc = 1; thishue = random16(255); thisfade = 1; huediff = 16; break; // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
      case 15: break;                                                                // Here's the matching 15 for the other one.
    }
  }
} // ChangeMe() for confetti

void plasma() {                                                 // This is the heart of this program. Sure is short. . . and fast.

  int thisPhase = beatsin8(6, -64, 64);                         // Setting phase change for a couple of waves.
  int thatPhase = beatsin8(7, -64, 64);

  for (int k = 0; k < NUM_LEDS; k++) {                          // For each of the LED's in the strand, set a brightness based on a wave as follows:

    int colorIndex = cubicwave8((k * 23) + thisPhase) / 2 + cos8((k * 15) + thatPhase) / 2; // Create a wave and add a phase change and add another wave with its own phase change.. Hey, you can even change the frequencies if you wish.
    int thisBright = qsuba(colorIndex, beatsin8(7, 0, 96));                               // qsub gives it a bit of 'black' dead space by setting sets a minimum value. If colorIndex < current value of beatsin8(), then bright = 0. Otherwise, bright = colorIndex..

    leds[k] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending);  // Let's now add the foreground colour.
  }

}

void rippless() {

  for (int i = 0; i < maxRipples; i += 2) {                                       // Check to see if ripple has expired, and if so, create a new one.
    if (random8() > 224 && !ripples[i].exist) {                                   // Randomly create a new ripple if this one has expired.
      ripples[i].Init(192, 10);                                                   // Initialize the ripple array with Fade and MaxLife values.
      ripples[i + 1] = ripples[i];                                                // Everything except velocity is the same for the ripple's other half. Position starts out the same for both halves.
      ripples[i + 1].velocity *= -1;                                              // We want the other half of the ripple to go opposite direction.
    }
  }

  for (int i = 0; i < maxRipples; i++) {                                          // Move the ripple if it exists
    if (ripples[i].exist) {
      leds[ripples[i].pos] = ColorFromPalette(currentPalette, ripples[i].color, ripples[i].brightness, LINEARBLEND);
      ripples[i].Move();
    }
  }

  fadeToBlackBy(leds, NUM_LEDS, 160);

} // rippless()



void SetupMySimilar4Palette() {                                                   // Create a palette with colours close to each other.

  uint8_t thishue = random8();

  targetPalette = CRGBPalette16(CHSV(thishue + random8(32), 255, random8(128, 255)),
                                CHSV(thishue + random8(32), 255, random8(192, 255)),
                                CHSV(thishue + random8(32), 192, random8(192, 255)),
                                CHSV(thishue + random8(32), 255, random8(128, 255)));

} // SetupMySimilar4Palette()

void beatwave() {

  uint8_t wave1 = beatsin8(9, 0, 255);                        // That's the same as beatsin8(9);
  uint8_t wave2 = beatsin8(8, 0, 255);
  uint8_t wave3 = beatsin8(7, 0, 255);
  uint8_t wave4 = beatsin8(6, 0, 255);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, i + wave1 + wave2 + wave3 + wave4, 255, currentBlending);
  }

} // beatwave()
