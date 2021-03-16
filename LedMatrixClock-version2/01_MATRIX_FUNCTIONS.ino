/*
    We will store all our (global) variables in  memory,
    lets get them at bootup and make them globally accessible

    Maybe lets not use SPIFFS but preferences!!
*/
void getStoredParameters() {
  Serial.println("Update all global parameters with stored info");

  /* LETS USE PREFERENCES LIBRARY */
  preferences.begin("matrixsettings", false);                       // The Matrix credentials are stored here
  // Remove all preferences under the opened namespace
  //preferences.clear();                                           // we can use this to wipe memory
  /* Or remove the counter key only */
  /* END PREFS INIT */

  whichFX = preferences.getString("effect");
  digitAnimation = preferences.getString("digitOnOff");
  digitColor = preferences.getLong("inputDigitCol");
  backgroundColor = preferences.getLong("inputBGCol");
  String inputMessage = preferences.getString("nightMode");
  if (inputMessage == "On") {
    nightMode = true;
  } else {
    nightMode = false;
  }
  nightTimeHour = preferences.getUInt("nightTimeHour", 21);
  nightTimeMinute = preferences.getUInt("nightTimeMinute", 0);
  morningTimeHour = preferences.getUInt("morningTimeHour", 7);
  morningTimeMinute = preferences.getUInt("morningTimeMinute", 0);
  overAllBrightness = preferences.getUInt("oaBrightness", 101);
  backgroundBrightness = preferences.getUInt("bgBrightness", 102);
  digitBrightness = preferences.getUInt("fgBrightness", 103);
  randomAnimation = preferences.getString("randomAnimation");
  scrolltext = preferences.getString("scrolltext");
  digitHTMLcol = preferences.getString("digitHTMLcol");
  BGHTMLcol = preferences.getString("BGHTMLcol");
  randomWhat = preferences.getString("randomWhat");


  digitColor = strtol(digitHTMLcol.substring(1).c_str(), NULL, 16);
  backgroundColor = strtol(BGHTMLcol.substring(1).c_str(), NULL, 16);
  Serial.print("Last selected animation: "); Serial.println(whichFX);
  Serial.print("When playing random animations we animate: "); Serial.println(randomWhat);
  Serial.print("Digit animation: "); Serial.println(digitAnimation);
  Serial.print("Background brightness: "); Serial.println(backgroundBrightness);
  Serial.println("Loading settings done");
  preferences.end();

}


/*
   To setup the matrix properly, and to provide two functions for easy finding the led number given a X, Y coordinate.
*/
// all mapping functions that map X,Y position to the correct LED.
int xyToLedNumber(int x, int y) { //based on X,Y position calculate the index in array
  int pos = y * XRES + x;
  return ledArray[pos];
  //return pgm_read_word_near(ledArray + pos);
}

void makeXYMatrix() {
  for (int y = 0; y < YRES; y++) {
    for (int x = 0; x < XRES; x++) {
      int pos = y * XRES + x;
      ledMatrix[y][x] = ledArray[pos];
      //       ledMatrix[y][x] = pgm_read_word_near(ledArray + pos);
    }
  }
}

// This function will return a 0 or 1 based on which pixel makes part of the 8x8 character for the time.
int xyToLedStatus(int x, int y, int *arr ) {
  return arr[y * 8 + x];
}

/*
   To be more efficient? I hope..
   Background LEDS with color info will be stored in one map
   Digits (or characters) will be stored in an seperate map
   The backrgound is ALL leds minus the led's of the digits

   Below you will find functions to facilitate using maps
*/

/*
   Get the background map (all - digits)
   Input: the digitMap (<int, CRGB>)
   Out: the backgroundMap (<int, CRGB>)
   int is led number on matrix
   CRGB is the color of this led
*/
std::map<int, CRGB> getBackgroundMap(std::map<int, CRGB> digitMap ) {
  std::map<int, CRGB> bg = makeBgMap(NUM_LEDS);                               // The new to be created background map
  std::map<int, CRGB>::iterator it;                                           // an iterator
  for (it = digitMap.begin(); it != digitMap.end(); it++) {                   // Go over the digit map, and remove those leds from background
    bg.erase(it->first);
  }
  return bg;                                                                  // return the depleted background.
} // getBackgroundMap()
/*
    function to put the complete matrix into a map function
*/
std::map<int, CRGB> makeBgMap(int matrixSize) {
  std::map<int, CRGB> mmap;
  for (int i = 0; i < matrixSize; i++) {
    mmap[i] = leds[i];
  }
  return mmap;
}

/*
    Get the CRGB from matrix using  int, CRGB map
*/
CRGB getCRGBNumber(int lednr, std::map<int, CRGB> matrix) {
  CRGB col = CRGB::Black;
  std::map<int, CRGB>::iterator it;
  it = matrix.find(lednr);
  if (it != matrix.end()) {
    col = it->second;
  }
  return col;
}

/*
   A function to merge the maps to display on the matrix.
   It takes both the background and foreground maps, and maps it to the leds[] array
*/
void mergeMapsToLeds(std::map<int, CRGB> bg, std::map<int, CRGB> fg, int bgbrightness, int fgbrightness, boolean bgtoBlack = false, boolean fgtoBlack = false) {

  if (!fgtoBlack) {
    for (auto const& t : fg) {
      leds[t.first] = t.second;
      leds[t.first].fadeLightBy( fgbrightness );
    }
  } else {
    for (auto const& t : fg) {
      leds[t.first] = t.second;
      leds[t.first].fadeToBlackBy( fgbrightness );
    }
  }

  if (!bgtoBlack) {
    for (auto const& t : bg) {
      leds[t.first] = t.second;
      leds[t.first].fadeLightBy( bgbrightness );
    }
  } else {
    for (auto const& t : bg) {
      leds[t.first] = t.second;
      leds[t.first].fadeToBlackBy( bgbrightness );
    }
  }
} // mergeMapsToLeds()


/*
 * This function takes a character of digit array [64], and maps the leds that should turn on, onto the matrix.
 * Input is the letter array (int[64], the matrix map<int, CRGB>, start x position of the cursor, start y position of the cursus. 
 * clear true/false determines if we want to empty our map befor filling it again., digitcolor is in case we fill it new we give eacht led a default color.
 * it returns the matrix map, with the led positions and color for further displaying.
 */
std::map<int, CRGB> makeDigits(int *letter, std::map<int, CRGB> digits, int x = 0, int y = 0,  boolean clear = false, CRGB digitColor = CRGB::Red) {

  if (clear) {
    digits.clear();
  }

  // check if cursor is on the matrix
  // we do not yet take into account that the character can be 8 pixels in X and Y.. so they still can fall off the edge

  if (x < -7 || (x > (XRES - 1)) || y < -7 || y > (YRES - 1)) {
    // not on matrix
  } else {

    for (int dy = 0; dy < 8; dy++) {                              // dy is the row position in the character array
      for (int dx = 0; dx < 8; dx++) {                            // dx is the column x position in the character array all translated to a int[64] array (1 dimentional)
        if ((dx + x) <= (XRES - 1) && (dy + y) <= (YRES - 1) && (x + dx) >= 0) {     // check if this LED position is still on the matrix, if not, no need to continue
          // ok not on matrix
          if (xyToLedStatus( dx, dy, letter) == 1 ) {             // xyToLedStatus will see in the character array a 1 (this led should be ON) or 0, this LED should be off
            int locationOnMatrix = xyToLedNumber(dx + x, dy + y);
            if (digits.find(locationOnMatrix) == digits.end()) {
              if (useOld){
                digits[locationOnMatrix] = leds[locationOnMatrix];                // new LED, give it a default color.
              }else{
                digits[locationOnMatrix] = digitColor;  
              }
            } else {
              digits[locationOnMatrix] = leds[locationOnMatrix];     // LED already exists.. leave it.
            } // led already in map
          } // LED should be displayed
        } //LED is on matrix
      } //xx
    } // dy
  } // on matrix

  return digits;
} // makeDigits()

/*
   Debug functions
*/
