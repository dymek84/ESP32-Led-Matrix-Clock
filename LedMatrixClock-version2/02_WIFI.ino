/*
   Functions concerning WIFI
*/

boolean connectToNetwork(String s, String p ) {
  const char* ssid = s.c_str();
  const char* password = p.c_str();

  Serial.print("ACCESSING WIFI: "); Serial.println(ssid);
  WiFi.begin(ssid, password);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting to WiFi..");
    if (timeout == 5) {
      return false;
      break;
    }
    timeout++;
  }
  //Serial.println(WiFi.localIP());
  return true;
} // network()

/*
   A function to show all available NETWORKS and returns this as a HTML formatted STRING
*/
String scanNetworks() {
  // scan for nearby networks:
  String networkList = "<h3> == Available Networks == </h2>";
  byte numSsid = WiFi.scanNetworks();
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    networkList += "<br/> ";
    networkList += thisNet;
    networkList += " Network: ";
    networkList += WiFi.SSID(thisNet);
  }
  return networkList;
} // scanNetworks()

/* Function to run in ACCESSPOINT MODE */
// RunAPmode code
void RunAPmode( void * parameter) {
  WiFi.disconnect(true);                                                      // End All connections.
  AsyncWebServer server(80);
  WiFi.softAP(ssidAP, passwordAP);                                            // Start ACCESSPOINT MODE with basic credantials
  IPAddress IP = WiFi.softAPIP();                                             // GET THE ACCESSPOINT IP
  Serial.println("The IP of the settings page is: 192.168.1.4");                             // SHOW IP IN SERIAL MONITOR
  //Serial.println(WiFi.localIP());
  preferences.begin("wificreds", false);                                      // Make sure we have something to store our preferences in
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {              // The Home page so to say.
    request->send_P(200, "text/html", INDEXAP_HTML, processor);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {          // WHEN SOMEONE SUBMITS SOMETHING.... Like the credentials :)
    String inputMessage;
    const char* PARAM_WIFI = "SSIDname";
    const char* PARAM_PWD = "SSIDpwd";
    preferences.begin("wificreds", false);

    if (request->hasParam(PARAM_WIFI)) {
      inputMessage = request->getParam(PARAM_WIFI)->value();
      preferences.putString("ssid", inputMessage);
    }
    else if (request->hasParam(PARAM_PWD)) {
      inputMessage = request->getParam(PARAM_PWD)->value();
      preferences.putString("password", inputMessage);
    }
    else {
      inputMessage = "Restarting and using new credentials";
      ESP.restart();
    }
    Serial.println(inputMessage);                                              // This prints the submitted variable on the serial monitor.. as a check
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();
  Serial.print("AP mode runs on core: "); Serial.println(xPortGetCoreID());

  for (;;) {
    preferences.begin("wificreds", false);
    delay(5000);
    Serial.print(".");
  //  Serial.println(preferences.getString("ssid"));
  //  Serial.println(preferences.getString("password"));
    preferences.end();
  }
}

/*
   Find and replace method, to inject variables into a HTML page
*/
String processor(const String & var) {
  //String "#";
  if (var == "wifilist") {
    return scanNetworks();
  }
  else if (var == "effect") {
    // return readFile(SPIFFS, "/inputInt.txt");
    return whichFX;
  }
  else if (var == "digitOnOff") {
    return digitAnimation;
  }
  else if (var == "digitHTMLcol") {
    return digitHTMLcol;
  }
  else if (var == "BGHTMLcol") {
    return BGHTMLcol;
  }
  else if (var == "nightMode") {
    if (nightMode) {
      return "On";
    } else
      return  "Off";
  }
  else if (var == "nightTimeHour") {
    return String(nightTimeHour);
  }
  else if (var == "nightTimeMinute") {
    return String(nightTimeMinute);
  }
  else if (var == "morningTimeHour") {
    return String(morningTimeHour);
  }
  else if (var == "morningTimeMinute") {
    return String(morningTimeMinute);
  }
  else if (var == "overAllBrightness") {
    return String(overAllBrightness);
  }
  else if (var == "backgroundBrightness") {
    return String(backgroundBrightness);
  }
  else if (var == "digitBrightness") {
    return String(digitBrightness);
  }
  else if (var == "iRebooted") {
    return String(iRebooted);
  }
  else if (var == "randomAnimation") {
    return String(randomAnimation);
  }
  else if (var == "randomWhat") {
    return String(randomWhat);
  }
  else if (var == "scrolltext") {
    return String(scrolltext);
  } else if (var == "scrollspeed") {
    return String(scrollspeed);
  }
  else if (var == "city") {
    return String(city);
  }
  else if (var == "apikey") {
    return String(apikey);
  }
  return String();
}

/*
   Sometimes a page just doesn't exists.. we need to tell them
*/
void notFound(AsyncWebServerRequest * request) {
  request->send(404, "text/plain", "Not found");
} //runAPmode()


void displayIP() {
  String ip = WiFi.localIP().toString();
  int textScroller = -34;
  int counter = 0;
  String scrolltextip = "ip: " + ip;
  int textlentgh = scrolltextip.length();

  while (counter < (textlentgh * 10 * 2)) {
    EVERY_N_MILLISECONDS(80) {
      
      if (textScroller >= textlentgh * 7) {
        textScroller = -34;
      }
      timeMatrix = showText(scrolltextip, CRGB::Red, textScroller);
      bgMatrix = getBackgroundMap(timeMatrix);
      bgMatrix = oneColorBackground(bgMatrix, CRGB::Black);  
      textScroller++;
      counter++;
    }
  mergeMapsToLeds(bgMatrix, timeMatrix, 128, 128, false, false);     // Merge both matrices. before we display.
  FastLED.show();
  }
}

void displayIPAP() {
  String ip = WiFi.localIP().toString();
  int textScroller = -34;
  int counter = 0;
  String scrolltextip = "accesspoint ip: 192.168.4.1";
  int textlentgh = scrolltextip.length();

  while (counter < (textlentgh * 8 * 2)) {
    EVERY_N_MILLISECONDS(80) {
      
      if (textScroller >= textlentgh * 7) {
        textScroller = -34;
      }
      timeMatrix = showText(scrolltextip, CRGB::Red, textScroller);
      bgMatrix = getBackgroundMap(timeMatrix);
      bgMatrix = oneColorBackground(bgMatrix, CRGB::Black);  
      textScroller++;
      counter++;
    }
  mergeMapsToLeds(bgMatrix, timeMatrix, 128, 128, false, false);     // Merge both matrices. before we display.
  FastLED.show();
  }
}


/*
   This function runs the main webpage and handles all webtrafic.
   New settings will be stored in the globals AND in memory
*/
void RunWebserver( void * parameter) {
  AsyncWebServer server(80);                                // Start the webserver
  Serial.print("The IP of ledclock: ");                     // SHOW IP IN SERIAL MONITOR
  Serial.println(WiFi.localIP());
  Serial.print("The wifi server runs on core: ");
  Serial.println(xPortGetCoreID());                         // Webserver should run on second core (0)

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", INDEX_HTML, processor);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    preferences.end();                                      // In case preferences was still 'open' but in a different 'domain'
    String inputMessage;
    preferences.begin("matrixsettings", false);             // Open the matrixsettings preferences 'domain'

    /*
       The website will send the parameters that the user selects
       This part catches the paramters and stores them in preferences.
       This can later be picked-up by the rest of the code.
       These user parameters also will to be put in the GLOBALS.
    */
    if (request->hasParam("effect")) {
      inputMessage = request->getParam("effect")->value();
      preferences.putString("effect", inputMessage);
      //writeFile(SPIFFS, "/inputString.txt", inputMessage.c_str());
      whichFX = inputMessage;
    }
    else if (request->hasParam("digitOnOff")) {
      inputMessage = request->getParam("digitOnOff")->value();
      preferences.putString("digitOnOff", inputMessage);
      digitAnimation = inputMessage;
    }
    else if (request->hasParam("inputDigitCol")) {
      inputMessage = request->getParam("inputDigitCol")->value();
      //preferences.putString("inputDigitCol", inputMessage);
      digitColor = strtol(inputMessage.substring(1).c_str(), NULL, 16);
      Serial.print("submitted color digit long: "); Serial.println(digitColor);
      preferences.putLong("inputDigitCol", digitColor);
      preferences.putString("digitHTMLcol", inputMessage);
      digitHTMLcol = inputMessage;
    }
    else if (request->hasParam("inputBGCol")) {
      inputMessage = request->getParam("inputBGCol")->value();
      backgroundColor = strtol(inputMessage.substring(1).c_str(), NULL, 16);
      preferences.putLong("inputBGCol", backgroundColor);
      preferences.putString("BGHTMLcol", inputMessage);
      BGHTMLcol = inputMessage;
    }
    else if (request->hasParam("nightMode")) {
      inputMessage = request->getParam("nightMode")->value();
      preferences.putString("nightMode", inputMessage);
      if (inputMessage == "On") {
        nightMode = true;
      } else {
        nightMode = false;
      }
    }
    else if (
      request->hasParam("nightTimeHour")) {
      inputMessage = request->getParam("nightTimeHour")->value();
      nightTimeHour = inputMessage.toInt(); //store globally
      preferences.putUInt("nightTimeHour", inputMessage.toInt());
    }
    else if (
      request->hasParam("nightTimeMinute")) {
      inputMessage = request->getParam("nightTimeMinute")->value();
      nightTimeMinute = inputMessage.toInt(); //store globally
      preferences.putUInt("nightTimeMinute", inputMessage.toInt());
    }
    else if (
      request->hasParam("morningTimeHour")) {
      inputMessage = request->getParam("morningTimeHour")->value();
      morningTimeHour = inputMessage.toInt(); //store globally
      preferences.putUInt("morningTimeHour", inputMessage.toInt());
    }
    else if (
      request->hasParam("morningTimeMinute")) {
      inputMessage = request->getParam("morningTimeMinute")->value();
      morningTimeMinute = inputMessage.toInt(); //store globally
      preferences.putUInt("morningTimeMinute", inputMessage.toInt());
    }
    else if (
      request->hasParam("overAllBrightness")) {
      inputMessage = request->getParam("overAllBrightness")->value();
      overAllBrightness = inputMessage.toInt();
      preferences.putUInt("oaBrightness", overAllBrightness);
    }
    else if (
      request->hasParam("backgroundBrightness")) {
      inputMessage = request->getParam("backgroundBrightness")->value();
      backgroundBrightness = inputMessage.toInt();
      preferences.putUInt("bgBrightness", backgroundBrightness);
    }
    else if (
      request->hasParam("digitBrightness")) {
      inputMessage = request->getParam("digitBrightness")->value();
      preferences.putUInt("fgBrightness", inputMessage.toInt());
      digitBrightness = inputMessage.toInt();
    }
    else if ( // randomAnimation
      request->hasParam("randomAnimation")) {
      inputMessage = request->getParam("randomAnimation")->value();
      preferences.putString("randomAnimation", inputMessage);
      randomAnimation = inputMessage;
    }
    else if (
      request->hasParam("scrolltext")) {
      inputMessage = request->getParam("scrolltext")->value();
      preferences.putString("scrolltext", inputMessage);
      scrolltext = inputMessage;
    }
    else if (
      request->hasParam("randomWhat")) {
      inputMessage = request->getParam("randomWhat")->value();
      preferences.putString("randomWhat", inputMessage);
      randomWhat = inputMessage;
    }
    else if (
      request->hasParam("scrollspeed")) {
      inputMessage = request->getParam("scrollspeed")->value();
      preferences.putString("scrollspeed", inputMessage);
      scrollspeed = inputMessage.toInt();
    }
    else if (
      request->hasParam("restart")) {
      inputMessage = "Restarting and useing new credentials";
      ESP.restart();
    }
    else if (
      request->hasParam("apikey")) {
      inputMessage = request->getParam("apikey")->value();
      preferences.putString("apikey", inputMessage);
      apikey = inputMessage;
    }
    else if (
      request->hasParam("city")) {
      inputMessage = request->getParam("city")->value();
      preferences.putString("city", inputMessage);
      city = inputMessage;
    }
    else
    {
      inputMessage = "No message sent";
    }
    Serial.print("New variable received: "); Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
    preferences.end();                                          // don't leave the door open :) always close when you leave.
  });

  server.onNotFound(notFound);
  server.begin();

  for (;;) {

  }
}
