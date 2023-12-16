/*
   Function(s) to get the Time from the NTP server
   AND FUNCTIONS NEEDED TO DISPLAY THE TIME ON THE MATRIX
*/

//time function to get the local time from the NTP server
boolean updateLocalTime()
{
  int retryCounter = 0;
  //struct tm timeinfo;
  while (!getLocalTime(&timeinfo) && retryCounter < 6 ) {
    Serial.println("Failed to obtain time.. try again in 3 seconds");
    matrix[16] = CRGB::Orange;
    FastLED.show();
    FastLED.delay(3000);
    retryCounter++;
    if (retryCounter > 5) {
      Serial.println("No time server..let us reboot");
      return false;
      break;
    }
  }

  // update globals
  Hour = timeinfo.tm_hour;                                              // get the hour as int.
  Minute = timeinfo.tm_min;                                             // get the minutes as int.
  Seconde = timeinfo.tm_sec;                                            // get the second

  // if we have the time, return true
  return true;
}

/*
   To show the time, we will map the LEDs that form together the digits onto the matrix.
   As we also want to be able to animate the background, separately from the digits we need somehow to
   store the digit LED's with their respective color information and further down the 'road' combine/merge this with the background LED's

   input parameters: the character, the x and y location of this character and the hashmap (digits<int, CRGB>): int = led number, and CRGB is FastLED color information
   int *letter is the array containing a single charachter (8*8, presented as a [64] lentgh array).
   int x, x location on the matrix, default 0
   int y, y location on the matrix, default 0
   std::map<int, CRGB> is the hashmap of the created digits.
   boolean clear: default false. When true the hashmap will be resetted (cleared).

*/
/*
   Show the time.. or better create the time 'matrix' allow for animations if possible
   input, animation type (String) default none
   dcolor = CRGB object of the color of the digits, all red is default
   int digitBrightness, the brightness of the digits

   Returns a MAP of all LED positions that are part of the time which can be used for further animations or  displaying on matrix.
*/
std::map<int, CRGB> getTimeMatrix(String animation = "none", CRGB dcolor = CRGB::Red) {
  // create the timeMatrix.

  if (timeChangeAnimation) {
    //  Serial.println("Time change animation");
    //  Serial.println(Minute);

    if (OldHour != Hour) {
      hourhaschanged = true;
    }
    if (Minute == 10 || Minute == 20 || Minute == 30 || Minute == 40 || Minute == 50 || Minute == 0) {
      tenminuteJump = true;
    } else {
      tenminuteJump = false;
    }

    if (Hour == 0 || Hour == 10 || Hour == 20 ) {
      hourjump = true;
    } else {
      hourjump = false;
    }

    EVERY_N_MILLISECONDS(50) {
      yloc++;
      if (hourhaschanged) {
        if (hourjump) {
          hydigit1 = yloc;
          hydigit2 = yloc;
        } else {
          hydigit1 = yloc;
          hydigit2 = 0;
        }
      }
      if (tenminuteJump) {
        ydigit1 = yloc;
        ydigit2 = yloc;
      } else {
        ydigit1 = 0;
        ydigit2 = yloc;
      }
      if (yloc >= 8) {                     // keep animating until number has changed
        if (tenminuteJump) {
          ydigit1 = yloc - 16;
          ydigit2 = yloc - 16;
        } else {
          ydigit1 = 0;
          ydigit2 = yloc - 16;
        }
        if (hourhaschanged) {
          if (hourjump) {
            hydigit1 = yloc - 16;
            hydigit2 = yloc - 16;
          } else {
            hydigit1 = yloc;
            hydigit2 = 0;
          }
        }
        OldHour = Hour;
        OldMinute = Minute;
      }
      if (yloc == 16) {
        timeChangeAnimation = false;
        yloc = 0;
        ylocmin = 0;
        ydigit1 = 0;
        ydigit2 = 0;
        hydigit1 = 0;
        hydigit2 = 0;
        ylochour = 0;
        hourhaschanged = false;
      }
    }
  }

  std::map<int, CRGB> test;
  test = makeDigits(numberMatrix[7], test, 0, 0, true, CRGB::Red);
  String h = String(OldHour);
  if (h.length() > 1) {
    String hf = h.substring(0, 1);
    test = makeDigits(numberMatrix[hf.toInt()], test, 0, hydigit1, true, dcolor);
    String hs = h.substring(1);
    test = makeDigits(numberMatrix[hs.toInt()], test, 6, hydigit2, false, dcolor);
  } else {
    test = makeDigits(numberMatrix[0], test, 0, hydigit1, true, dcolor);
    test = makeDigits(numberMatrix[h.toInt()], test, 6, hydigit2, false, dcolor);
  }

  // Two dots in the middle
  if ((Seconde & 1) == 0) {                                                            // Make the dots blink.. check if seconds is even or odd
    //  Serial.println("tik");
    test = makeDigits(letterMatrix[0], test, 12, 0, false, dcolor);
  } else {
    //   Serial.println("tok");
    test = makeDigits(letterMatrix[0], test, 12, 0, false, CHSV(0, 0, 0));
    /* if (animation == "dotanimation-inoutmatrix"){
       test = makeDigits(letterMatrix[0], test, 12, 0, false, CHSV(0,0,0));
      }else{
       test = makeDigits(letterMatrix[0], test, 12, 0, false, CHSV(0,0,0));
      }*/
  }


  // minutes
  String m = String(OldMinute);
  if (m.length() > 1) {
    //  Serial.println(m);
    String mf = m.substring(0, 1);
    test = makeDigits(numberMatrix[mf.toInt()], test, 17, ydigit1, false, dcolor);
    String ms = m.substring(1);
    test = makeDigits(numberMatrix[ms.toInt()], test, 23, ydigit2, false, dcolor);
  } else {
    test = makeDigits(numberMatrix[0], test, 17, ydigit1, false, dcolor);
    test = makeDigits(numberMatrix[m.toInt()], test, 23, ydigit2, false, dcolor);
  }
  return test;
} // showTime()


void wakeupclock() {
  Serial.println("Nachtmode aan");
  nightMode = true;
}

/*
   This is just a check if the clock should be in nightmode or not. It uses global variables so nothing in or ourgoing here
*/
void nightModeOnOrOff() {
  if (timeinfo.tm_hour > 12 && timeinfo.tm_hour <= 23 && timeinfo.tm_hour >= nightTimeHour && timeinfo.tm_min >= nightTimeMinute ) { // it is evening, and the NightLamp time is passed.
    EveningSleep = true;
    // store nightmode status in prefs.
    if (!nightMode) {
      preferences.begin("matrixsettings", false);
      preferences.putString("nightMode", "On");
      preferences.end();
    }
    nightMode = true;
  }
  if (EveningSleep) {                                          // The matrix is in everningsleep mode.. This is ended when it's time to wakeup.
    if (timeinfo.tm_hour >= 0 && timeinfo.tm_hour < 12 && timeinfo.tm_hour >= morningTimeHour && timeinfo.tm_min >= morningTimeMinute) {
      EveningSleep = false;
      // preferences.begin("matrixsettings", false);
      // whichFX = preferences.getString("effect");               // Get the last stored FX
      // preferences.end();
      nightMode = false;
      preferences.begin("matrixsettings", false);
      preferences.putString("nightMode", "Off");
      preferences.end();
    } else {
      nightMode = true;
    }
  }
}
