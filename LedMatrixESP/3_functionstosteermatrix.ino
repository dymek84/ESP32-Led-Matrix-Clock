// FUNCTIONS

/*
 * Here we start our network connection and check if we actually have an connection.. if not, we go to accesspoint mode.
 */
void connectToNetwork() {

  // Read the WIFI ssid and pwd from memory
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  delay(200);
  // read wifi credentials from memory
  password = readFile(SPIFFS, "/inputPWD.txt").c_str();
  delay(200);
  ssid = readFile(SPIFFS, "/inputWifi.txt").c_str();
  delay(200);
  Serial.println("ESP32 said: I used this for SSID and PWD");
  Serial.println(ssid);
  String wifistation = readFile(SPIFFS, "/inputWifi.txt"); // no idea.. but without this line it doesnt work... weird
  Serial.println(wifistation);
  int netWorkTrys = 0;
  leds[0] = CRGB (0, 10, 20);
  FastLED.show();
  int ledOn = 1;
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED ) { // a max try should be specified
    delay(2000);
    leds[ledOn] = CRGB (0, 10, 20);
    Serial.println("Establishing connection to WiFi..");
    ledOn ++;
    FastLED.show();

    if (netWorkTrys > 5) {
      WiFi.disconnect(true);
      Serial.println("No internet, starting in AP");
      /** Ok no internet, maybes start now in AP mode? */
      WiFiServer server(80);
      Serial.print("SNow setting-up soft access point mode...");
      WiFi.softAP(ssidAP, passwordAP);
      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP);
      server.begin();
      APmode = true;
      break;
    }
    netWorkTrys++;
  }
  leds[0] = CRGB::Green;
  Serial.println("Connected to network");
  showIPonLED();
  FastLED.show();
}

/* A function to show the IP-address on the LED ... last 3 digits
 *  very handy, so you don't need the serial monitor for this
 *  most often the ip address iss: 192.168.1.xxx where xxx is shown on the matrix
 *  or xx if only 2 digits are used.
*/
void showIPonLED() {
  /* show last digits IP address */
  String ip = WiFi.localIP().toString();
  String iplast0 = ip.substring(ip.length() - 1);
  String iplast1 = ip.substring(ip.length() - 2, ip.length() - 1);
  String iplast2 = ip.substring(ip.length() - 3, ip.length() - 2);

  int digitBrightness = 128;

  CRGB color = timeColor;
  if (iplast2 == ".") { // a two digit IP
    showCharacter(numberMatrix[iplast1.toInt()], locationCursorX + 16, color, digitBrightness );
    showCharacter(numberMatrix[iplast0.toInt()], locationCursorX + 24, color, digitBrightness );
    // showCharacter(numberMatrix[iplast.toInt()], locationCursorX, color );
  } else if (iplast1 == "."){ // a one digit ip
    showCharacter(numberMatrix[iplast0.toInt()], locationCursorX + 24, color , digitBrightness);
  }
  else { // a 3 digit IP
    showCharacter(numberMatrix[iplast2.toInt()], locationCursorX + 8, color , digitBrightness);
    showCharacter(numberMatrix[iplast1.toInt()], locationCursorX + 16, color, digitBrightness );
    showCharacter(numberMatrix[iplast0.toInt()], locationCursorX + 24, color, digitBrightness );
  }

  // Serial.println(iplast0); Serial.println(iplast1); Serial.println(iplast2);  //debugging

  FastLED.show();
  FastLED.delay(5000); // IP shown for 5 seconds and than we move on
}

/**
 * To setup the matrix properly, and to provide two functions for easy finding the led number given a X, Y coordinate.
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
