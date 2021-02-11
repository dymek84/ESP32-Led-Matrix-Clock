/*
   Below are many functions that create a 'background' animation
   mostly taken from  https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#FastLEDFramework

*/

// ShowStrip is used to start the fastled.show() but allows to add a few customizable things
void showStrip() {
  // FastLED

  showTime(timeColor); //inject the time.
  FastLED.setBrightness(64);
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
    FastLED.delay(1000);
  //  if (breakAnimation) {
  //    breakAnimation = false;
  //    break;
  //  }
  //}
  timeColor = ledcol;
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
  }else{
    runAnimation = false;
  }

 
  int randomNumber = rand() % 7 + 1;
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
      Serial.println("showing: Bouncing balls");
      byte colors[3][3] = { {0xff, 0, 0},
        {0xff, 0xff, 0xff},
        {0   , 0   , 0xff}
      };
      BouncingColoredBalls(3, colors);
    }
    
    if (minute == 0 || minute == 15 || minute == 30 || minute == 45) {
      
    }else{
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
