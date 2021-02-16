/*
   Ah yes, showing the time. This is what it is all about
   It takes as an argument a CRGB color (but HSV is also allowed), and even just a hex number of the HTML color
   digitBrightness is used together with fadeToBlack (fastLED).. where 255 is black!
*/
void showTime(CRGB color, int digitBrightness = 0) {

  //update the time
  //updateLocalTime(); //15-02-2021 moved to main loop with EVERY-N-SECONDS.. to prevent too many calls.
  EVERY_N_SECONDS(1) {
    updateLocalTime();
  }
  hour = timeinfo.tm_hour; // get the hour as int.
  minute = timeinfo.tm_min; // get the minutes as int.
  second = timeinfo.tm_sec; // get the seconds

      targetPalette = RainbowColors_p;
      uint8_t thisSpeed = 5;
      uint8_t deltaHue = 10;
      uint8_t thisHue = beat8(thisSpeed, 255);
  if (getallenZwart == 2){    
      timeColor = ColorFromPalette( targetPalette, thisHue);
  }

  // the start of showing the time
  // hours
  String h = String(hour);
  if (h.length() > 1) {
    String hf = h.substring(0, 1);
    showCharacter(numberMatrix[hf.toInt()], locationCursorX, color, digitBrightness );
    String hs = h.substring(1);
    showCharacter(numberMatrix[hs.toInt()], locationCursorX + 6, color, digitBrightness );

  } else {
    showCharacter(numberMatrix[0], locationCursorX, color, digitBrightness  );
    showCharacter(numberMatrix[h.toInt()], locationCursorX + 6, color, digitBrightness);
  }

  // minutes
  String m = String(minute);
  if (m.length() > 1) {
    //  Serial.println(m);
    String mf = m.substring(0, 1);
    showCharacter(numberMatrix[mf.toInt()], locationCursorX + 17, color, digitBrightness );
    String ms = m.substring(1);
    showCharacter(numberMatrix[ms.toInt()], locationCursorX + 23, color, digitBrightness );

  } else {
    showCharacter(numberMatrix[0], locationCursorX + 17, color, digitBrightness );
    showCharacter(numberMatrix[m.toInt()], locationCursorX + 23, color, digitBrightness);
  }
  
  showDots(color, second, digitBrightness); //shows the two (blinking) dots

}

// this function takes a character Array (8x8) as input, plus the current location of the cursor and maps it onto the display matrix.
// at the moment we only use X position and don't work with Y position. Maybe in the next version we will allow also Y position, this can help in animations of the characters
void showCharacter(int *letter, int locationCursor, CRGB color, int digitBrightness) {

  /** make a character LedMatrix **/

  if (locationCursor < XRES) { // we are on the matrix
    // I prefer not to hardcode the the dimensions of our characters.But for now we use x=8 and y=8 in a [64] matrix.
    for (int y = 0; y < 8; y++) { // x and y coordinates of 8*8 matrix
      for (int x = 0; x < 8; x++) {
        if (xyToLedStatus(x, y, letter) == 1) { // see if pixel in matrix is turned on and do something.. like showing it or ..
          int locationOnMatrix = xyToLedNumber(x + locationCursor, y);
          // check if pixel is now above xres if so.. remap to start of the display
          if ((x + locationCursorX) > (XRES - 1)) {
            locationOnMatrix = xyToLedNumber(x + locationCursor - XRES , y);
          }
          // check if pixel is below xres of 0, if so remap to end of display.
          // leds[locationOnMatrix] = CHSV(hue, 255, 128);
          leds[locationOnMatrix] = color;
          // leds[locationOnMatrix].fadeToBlackBy( digitBrightness );
          leds[locationOnMatrix].fadeLightBy( digitBrightness );
        }
      }
    }
  }
}

void showDots(CRGB color, int second, int digitBrightness) {
  // We need  two dots in the middle
  // The location of the colon is dead center. Hardcoded now, x=15 and 16 and y= 1,2 and 6,5
  // we make it blink based on the seconds being even or odd
  //CRGB color = CRGB(0, 0, 0);
  if ((second & 1) == 0) {
    //  color = CHSV(hue, 255, 128);
  } else {
    color = CHSV(0, 0, 0);
  }
  leds[xyToLedNumber(15, 1)] = color;
  // leds[xyToLedNumber(15, 1)].fadeToBlackBy( digitBrightness );
  leds[xyToLedNumber(15, 1)].fadeLightBy( digitBrightness );

  leds[xyToLedNumber(16, 1)] = color;
  // leds[xyToLedNumber(16, 1)].fadeToBlackBy( digitBrightness );
  leds[xyToLedNumber(16, 1)].fadeLightBy( digitBrightness );


  leds[xyToLedNumber(15, 2)] = color;
  //  leds[xyToLedNumber(15, 2)].fadeToBlackBy( digitBrightness );
  leds[xyToLedNumber(15, 2)].fadeLightBy( digitBrightness );


  leds[xyToLedNumber(16, 2)] = color;
  // leds[xyToLedNumber(16, 2)].fadeToBlackBy( digitBrightness );
  leds[xyToLedNumber(16, 2)].fadeLightBy( digitBrightness );


  leds[xyToLedNumber(15, 5)] = color;
  // leds[xyToLedNumber(15, 5)].fadeToBlackBy( digitBrightness );
  leds[xyToLedNumber(15, 5)].fadeLightBy( digitBrightness );


  leds[xyToLedNumber(16, 5)] = color;
  // leds[xyToLedNumber(16, 5)].fadeToBlackBy( digitBrightness );
  leds[xyToLedNumber(16, 5)].fadeLightBy( digitBrightness );

  leds[xyToLedNumber(15, 6)] = color;
  // leds[xyToLedNumber(15, 6)].fadeToBlackBy( digitBrightness );
  leds[xyToLedNumber(15, 6)].fadeLightBy( digitBrightness );


  leds[xyToLedNumber(16, 6)] = color;
  // leds[xyToLedNumber(16, 6)].fadeToBlackBy( digitBrightness );
  leds[xyToLedNumber(16, 6)].fadeLightBy( digitBrightness );

}
//time function to get the local time from the NTP server
void updateLocalTime()
{
  int retryCounter = 0;
  //struct tm timeinfo;
  while (!getLocalTime(&timeinfo) && retryCounter < 6 ) {
    Serial.println("Failed to obtain time.. try again in 3 seconds");
    leds[16] = CRGB::Orange;
    FastLED.show();
    FastLED.delay(3000);
    retryCounter++;
    if (retryCounter > 5) {
      Serial.println("No time server..let us reboot");
      ESP.restart();
    }
  }
  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

/*
   To allow for animating the numbers of the time we need to create an array that has the length of all the leds that are ON in the matrix
   And subsequently, for example LED[1] in that matrix correspons to the achtual LED[x]. A mapping is needed

   Below are functions to accomodate this.
*/
