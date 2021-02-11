/*
   Here we have all the codes for the 2nd processor. WHich main function is to serve a HTML page, and keep track of any changes in the parameters
   and with that updates them and sends them to the animation ;)
*/
void Task1code( void * parameter) {
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } else {
    String whichFX = readFile(SPIFFS, "/inputString.txt");
    currentAnimation = whichFX;
    Serial.println("we start with the animation: "); Serial.println(currentAnimation);
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(PARAM_STRING)) {
      inputMessage = request->getParam(PARAM_STRING)->value();
      writeFile(SPIFFS, "/inputString.txt", inputMessage.c_str());
    }
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    else if (request->hasParam(PARAM_INT)) {
      inputMessage = request->getParam(PARAM_INT)->value();
      writeFile(SPIFFS, "/inputInt.txt", inputMessage.c_str());
    }
    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
    else if (request->hasParam(PARAM_FLOAT)) {
      inputMessage = request->getParam(PARAM_FLOAT)->value();
      writeFile(SPIFFS, "/inputFloat.txt", inputMessage.c_str());
    }
    else if (request->hasParam(PARAM_COL1)) {
      inputMessage = request->getParam(PARAM_COL1)->value();
      writeFile(SPIFFS, "/inputDigitCol.txt", inputMessage.c_str());
    }
    else if (request->hasParam(PARAM_COL2)) {
      inputMessage = request->getParam(PARAM_COL2)->value();
      writeFile(SPIFFS, "/inputBGCol.txt", inputMessage.c_str());
    }
    else if (request->hasParam(PARAM_NIGHTMODE)) {
      inputMessage = request->getParam(PARAM_NIGHTMODE)->value();
      writeFile(SPIFFS, "/nightMode.txt", inputMessage.c_str());
    }
    else if (
      request->hasParam(PARAM_NIGHTHOUR)) {
      inputMessage = request->getParam(PARAM_NIGHTHOUR)->value();
      writeFile(SPIFFS, "/nightTimeHour.txt", inputMessage.c_str());
      nightTimeHour = readFile(SPIFFS, "/nightTimeHour.txt").toInt(); //store globally
    }
    else if (
      request->hasParam(PARAM_NIGHTMIN)) {
      inputMessage = request->getParam(PARAM_NIGHTMIN)->value();
      writeFile(SPIFFS, "/nightTimeMinute.txt", inputMessage.c_str());
      nightTimeMinute = readFile(SPIFFS, "/nightTimeMinute.txt").toInt(); //store globally
    }
    else if (
      request->hasParam(PARAM_MORNINGHOUR)) {
      inputMessage = request->getParam(PARAM_MORNINGHOUR)->value();
      writeFile(SPIFFS, "/morningTimeHour.txt", inputMessage.c_str());
      morningTimeHour = readFile(SPIFFS, "/morningTimeHour.txt").toInt(); //store globally
    }
    else if (
      request->hasParam(PARAM_MORNINGMINUTE)) {
      inputMessage = request->getParam(PARAM_MORNINGMINUTE)->value();
      writeFile(SPIFFS, "/morningTimeMinute.txt", inputMessage.c_str());
      morningTimeMinute = readFile(SPIFFS, "/morningTimeMinute.txt").toInt(); //store globally
    }
    else
    {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();


  // the part below can be seen as LOOP() from the main.. but now on processor 2
  for (;;) {

    String whichFX = readFile(SPIFFS, "/inputString.txt");
    String nachtmode = readFile(SPIFFS, "/nightMode.txt");
    int getallenZwart = readFile(SPIFFS, "/inputInt.txt").toInt();

 nightTimeHour = readFile(SPIFFS, "/nightTimeHour.txt").toInt();
  nightTimeMinute = readFile(SPIFFS, "/nightTimeMinute.txt").toInt();
  morningTimeHour = readFile(SPIFFS, "/morningTimeHour.txt").toInt();
  morningTimeMinute = readFile(SPIFFS, "/morningTimeMinute.txt").toInt(); 

    
    if (whichFX != currentAnimation) {
      Serial.println("We are going to switch animation");
      breakAnimation = true;
      currentAnimation = whichFX;
    }
    String colorString = readFile(SPIFFS, "/inputDigitCol.txt");
    digitColor = strtol(colorString.substring(1).c_str(), NULL, 16);

    String backgroundColor_1 = readFile(SPIFFS, "/inputBGCol.txt");
    backgroundColor = strtol(backgroundColor_1.substring(1).c_str(), NULL, 16);

    if (getallenZwart < 1) {
      timeColor = CRGB::Black;
    } else {
      timeColor = digitColor;
    }

    if (nachtmode == "Ja") {
      nightMode = true;
    } else {
      nightMode = false;
    }

    delay(2000);
  }
}

// Replaces placeholder with stored values in the HTML pages. Everytime you add new fields to the webpage
// make sure to add them here as well.
String processor(const String & var) {
  //Serial.println(var);
  if (var == "inputString") {
    return readFile(SPIFFS, "/inputString.txt");
  }
  else if (var == "inputInt") {
    return readFile(SPIFFS, "/inputInt.txt");
  }
  else if (var == "inputFloat") {
    return readFile(SPIFFS, "/inputFloat.txt");
  }
  else if (var == "inputBGCol") {
    return readFile(SPIFFS, "/inputBGCol.txt");
  }
  else if (var == "inputDigitCol") {
    return readFile(SPIFFS, "/inputDigitCol.txt");
  }
  else if (var == "inputWifi") {
    return readFile(SPIFFS, "/inputWifi.txt");
  }
  else if (var == "inputPWD") {
    return readFile(SPIFFS, "/inputPWD.txt");
  }
  else if (var == "nightMode") {
    return readFile(SPIFFS, "/nightMode.txt");
  }
  else if (var == "nightTimeHour"){
    return readFile(SPIFFS, "/nightTimeHour.txt");
  }
  else if (var == "nightTimeMinute"){
    return readFile(SPIFFS, "/nightTimeMinute.txt");
  }
  else if (var == "morningTimeHour"){
    return readFile(SPIFFS, "/morningTimeHour.txt");
  }
  else if (var == "morningTimeMinute"){
    return readFile(SPIFFS, "/morningTimeMinute.txt");
  }
  
  return String();
}

void writeFile(fs::FS & fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}
void notFound(AsyncWebServerRequest * request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS & fs, const char * path) {
  //Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    //   Serial.println("- empty file or failed to open file");
    return String();
  }
  // Serial.println("- read from file:");
  String fileContent;
  while (file.available()) {
    fileContent += String((char)file.read());
  }
  // Serial.println(fileContent);
  return fileContent;
}


// AccessPpin code
void Task2code( void * parameter) {
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } else {
    String wifistation = readFile(SPIFFS, "/inputWifi.txt");

    Serial.println("we start with the station: "); Serial.println(wifistation);
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index2_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(PARAM_WIFI)) {
      inputMessage = request->getParam(PARAM_WIFI)->value();
      writeFile(SPIFFS, "/inputWifi.txt", inputMessage.c_str());
    }
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    else if (request->hasParam(PARAM_PWD)) {
      inputMessage = request->getParam(PARAM_PWD)->value();
      writeFile(SPIFFS, "/inputPWD.txt", inputMessage.c_str());
    }
    else {
      inputMessage = "No message sent";
      ESP.restart();
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();


  // the part below can be seen as LOOP() from the main.. but now on processor 2
  for (;;) {

    String Wifinaam = readFile(SPIFFS, "/inputWifi.txt");
    String Wachtwoord = readFile(SPIFFS, "/inputPWD.txt");
    // Serial.println(Wifinaam);
    // Serial.println(Wachtwoord);
    delay(2000);
  }
}
