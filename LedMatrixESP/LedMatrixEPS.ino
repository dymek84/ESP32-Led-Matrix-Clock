/*
  LedMatrixESP - Clock
  This is a sketch made by: Bas van Breukelen, Arnhem Netherlands.
  This sketch is, "as it is" and made to run on a ESP32 (AZ-delivery ESP32-WROOM-32,  mini was used for development)
  The inital project was a 8x32 (256) ledMatrix, to run a a clock, updated via NTP and configurable via WiFi
  I have used effects as found on several webistes: like https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#FastLEDFramework
  Plus what I can create myself. The script have been modified to fit my needs
  I also used https://randomnerdtutorials.com/esp32-esp8266-input-data-html-form/ to setup the basics of the webserver
  To make things (hopefully) more efficient I run the webserver on the second core of the EPS using this tutorial: https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
  LedMatrix: 8x32 matrix (ali-express), 256 WS2812 pixels.
  FastLed library runs it all
*/

#include <FastLED.h>
#include <WiFi.h>
#include "time.h"
#include <String>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


#define NUM_LEDS 256
#define DATA_PIN 2 //GPIO2 on ESP32
#define BRIGHTNESS 255
#define XRES 32
#define YRES 8
#define VOLTAGE 5 //5 volt strip
#define AMPS 5000 //5 AMP (or 5000 mAmp powerbrick used)


// We will store PWD and SSID in these variables.
const char* ssid;
const char* password;

//in case we cannot connect to a wifi station we will create an accesspoint and serve a page to configure wifi access.
const char* ssidAP = "ESP32-LedMatrixClock";
const char* passwordAP = "1234abcd";
boolean APmode = false;
boolean newWifiSet = false;

//wifi server
AsyncWebServer server(80);
const char* PARAM_MESSAGE = "message";


// As this led matrix clock need to know the time....
const char* ntpServer = "europe.pool.ntp.org"; //this is a general Network Time Server
const long  gmtOffset_sec = 3600; // we live in UTC+1H
const int   daylightOffset_sec = 3600; // yes NL has daylight savings
int second;
int minute;
int hour;
int day;
int month;
int year;
int weekday;
long current;
struct tm timeinfo;


// We will be doing a lot with colors, and configuring colors.. And therefore need some variables to store them
CRGB timeColor = CRGB::Red;
CRGB digitColor = CRGB::Red;
CRGB backgroundColor = CRGB::Green;
String currentAnimation;
bool breakAnimation = false;
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
uint8_t hue = 0;
uint8_t mhue = 171;
uint8_t backgroundBrightness = 128;
uint8_t digitBrightness = 255;
uint8_t overAllBrightness = 255;
boolean nightMode = false;
int nightTimeHour = 22;
int nightTimeMinute = 00;
int morningTimeHour = 7;
int morningTimeMinute = 00;


// The nice thing about ESp32 is 2 cores.. so let's make use of both.
TaskHandle_t Task1;
TaskHandle_t Task2;


// We 'translate' the led matrix pixels into X and Y coordinated.. And the cursor will determine where we will place our characters..
int locationCursorX = 0; // our first pixel X is 0
int locarionCursorY = 0; // our first pixel Y is 0


/*
   HTML web page to handle our input fields.
   there are two (main) pages. The index_html is what is served to configure animations, colors and led matrix settings
   the index2_html is served only when the ESP32 is in AccessPoint mode, to configure wifi ssid and pwd.

   All variables that we use are stored in SSPID memory (stored even after power off). The writeFile() and readFile() function need to
   be updated when new variables (input fields) are added! Also the processor() function needs to be updated with the new inputs.

   As these pages need to be here before any script calls upon them, here is were we define them
*/

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP32 Led Matrix Clock</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
  <form action="/get" target="hidden-form">
    Kies je effect: nu %inputString% <br />
      <select name="inputString" id="effecten">
      <option value="%inputString%" selected> %inputString% </option>
        <option value="oneColorBackground">Fixed Background </option>
        <option value="randomEveryHour">Random ieder uur </option>
        <option value="randomEveryHalf">Random ieder half uur </option>
        <option value="randomEvery15">Random ieder 15 minuten </option>
         <option value="rainbowCycle">Regenbook achtergrond</option>
         <option value="sparkle">Glitter</option>
         <option value="runningLights">Rennend licht</option>
          <option value="colorWipe">Kleuren Wisser</option>
           <option value="Sneeuw">Sneeuw</option>
            <option value="randomTwinkle">kleuren twinkels</option>
            <option value="MeteorRain">Meteoor</option>
            <option value = "bouncing">Stuiterende Ballen</option>
            
      </select>
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br />
  
  <form action="/get" target="hidden-form">
    Klok cijfers aan/uit:  <br />
    <select name="inputInt" id="%inputInt%">
    <option value="%inputInt%" selected> %inputInt% </option>
    <option value="1">Aan (1)</option>
    <option value="0">Uit (0)</option>
    </select>
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br />
  
  <form action="/get" target="hidden-form">
    <label for="colorpicker1">Kies de kleur van je cijfers</label> <br />
    <input type="color" id="colorpicker1" name="inputDigitCol" value ="%inputDigitCol%">
    <input type="submit" value="Submit" onclick="submitMessage()">
   </form><br />
   
    <form action="/get" target="hidden-form">
    <label for="colorpicker2">Kies de kleur van je achtergrond</label> <br />
    <input type="color" id="colorpicker2" name="inputBGCol" value ="%inputBGCol%">
    <input type="submit" value="Submit" onclick="submitMessage()">
   </form><br />

   <form action="/get" target="hidden-form">
   Nachtstand gebruiken? Dit zet nachtmode handmatig aan<br />
    <select name="nightMode" id="nachtstand">
      <option value="%nightMode%" selected> %nightMode% </option>
        <option value="Ja">Ja </option>
        <option value="Nee">Nee </option>
    </select>
    <input type="submit" value="Submit" onclick="submitMessage()">
    </form>
<br />
<form action="/get" target="hidden-form">
   Nachtstand starten om: (uur) <br />
    <select name="nightTimeHour" id="nightTimeHour">
      <option value="%nightTimeHour%" selected> %nightTimeHour% </option>
        <option value="19">19</option>
        <option value="20">20</option>
        <option value="21">21</option>
        <option value="22">22</option>
        <option value="23">23</option>  
    </select>
    <input type="submit" value="Submit" onclick="submitMessage()">
     </form>
    <form action="/get" target="hidden-form">
   Nachtstand starten minuut: 
    <select name="nightTimeMinute" id="nightTimeMinute">
      <option value="%nightTimeMinute%" selected> %nightTimeMinute% </option>
        <option value="0">0</option>
        <option value="5">5</option>
        <option value="10">10</option>
        <option value="15">15</option>
        <option value="20">20</option>
        <option value="25">25</option>
        <option value="30">30</option>
        <option value="35">35</option>
        <option value="40">40</option>   
        <option value="45">45</option>
        <option value="50">50</option> 
        <option value="55">55</option>  
    </select>
    <input type="submit" value="Submit" onclick="submitMessage()">
    </form>
   <form action="/get" target="hidden-form">
   Nachtstand STOPPEN om: (uur) <br />
    <select name="morningTimeHour" id="morningTimeHour">
      <option value="%morningTimeHour%" selected> %morningTimeHour% </option>
        <option value="5">5</option>
        <option value="6">6</option>
        <option value="7">7</option>
        <option value="8">8</option>
        <option value="9">9</option>
        <option value="10">10</option>
        <option value="11">11</option>
            
    </select>
    <input type="submit" value="Submit" onclick="submitMessage()">
     </form>
    <form action="/get" target="hidden-form">
   Nachtstand STOPPEN minuut: 
    <select name="morningTimeMinute" id="morningTimeMinute">
      <option value="%morningTimeMinute%" selected> %morningTimeMinute% </option>
        <option value="0">0</option>
        <option value="5">5</option>
        <option value="10">10</option>
        <option value="15">15</option>
        <option value="20">20</option>
        <option value="25">25</option>
        <option value="30">30</option>
        <option value="35">35</option>
        <option value="40">40</option>   
        <option value="45">45</option>
        <option value="50">50</option> 
        <option value="55">55</option>  
    </select>
    <input type="submit" value="Submit" onclick="submitMessage()">
     </form>
     <br />Tussen %nightTimeHour%:%nightTimeMinute% en %morningTimeHour%:%morningTimeMinute% <br />
     zal de klok in nacht-stand staan. <br />
    <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";

// Webpage that is served when ESP is in case accessmode:
const char index2_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Matrix Clock, Accesspoint... Swtup mode</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
  <form action="/get" target="hidden-form">
    Wat is de naam van je wifi station: ( %inputWifi% ) <br /> 
    Druk op submit als je een nieuwe naam hebt ingevoerd. Voer daarna pas een wachtwoord in.<br />
    <input type="text " name="inputWifi" value="%inputWifi%">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    Wat is het wachtwoord van je wifi station <br />
    <input type="password " name="inputPWD" value="%inputPWD%">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
  Druk op deze knop als je een nieuw wachtwoord en ssid hebt opgegeven <br />
  <input type="submit" value="Start opnieuw op" onclick="submitMessage()">
  <br />
  <br /> Made by Bas van Breukelen (c) 2021
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";


// this will contain (later) all options for the animations
const char* PARAM_STRING = "inputString";
const char* PARAM_INT = "inputInt";
const char* PARAM_FLOAT = "inputFloat";
const char* PARAM_WIFI = "inputWifi";
const char* PARAM_PWD = "inputPWD";
const char* PARAM_COL1 = "inputDigitCol";
const char* PARAM_COL2 = "inputBGCol";
const char* PARAM_NIGHTMODE = "nightMode";
const char* PARAM_NIGHTHOUR = "nightTimeHour";
const char* PARAM_NIGHTMIN = "nightTimeMinute";
const char* PARAM_MORNINGHOUR = "morningTimeHour";
const char* PARAM_MORNINGMINUTE = "morningTimeMinute";


// end stuff for webserver



/*
    preapare the matrix
    Below we create a (one dimentional) array where we basically for each x,y position put the LED number. In this way, you can acoomodate for
    any setup of leds. ZigZag, Or Left to right, right to left, up/down or combination of multiple matrices.

*/
CRGB leds[NUM_LEDS];


const int ledArray[XRES * YRES] = // the order of led-id's in this case every row is left to right.

{
  0, 15, 16, 31, 32, 47, 48, 63, 64, 79, 80, 95, 96, 111, 112, 127, 128, 143, 144, 159, 160, 175, 176, 191, 192, 207, 208, 223, 224, 239, 240, 255,
  1, 14, 17, 30, 33, 46, 49, 62, 65, 78, 81, 94, 97, 110, 113, 126, 129, 142, 145, 158, 161, 174, 177, 190, 193, 206, 209, 222, 225, 238, 241, 254,
  2, 13, 18, 29, 34, 45, 50, 61, 66, 77, 82, 93, 98, 109, 114, 125, 130, 141, 146, 157, 162, 173, 178, 189, 194, 205, 210, 221, 226, 237, 242, 253,
  3, 12, 19, 28, 35, 44, 51, 60, 67, 76, 83, 92, 99, 108, 115, 124, 131, 140, 147, 156, 163, 172, 179, 188, 195, 204, 211, 220, 227, 236, 243, 252,
  4, 11, 20, 27, 36, 43, 52, 59, 68, 75, 84, 91, 100, 107, 116, 123, 132, 139, 148, 155, 164, 171, 180, 187, 196, 203, 212, 219, 228, 235, 244, 251,
  5, 10, 21, 26, 37, 42, 53, 58, 69, 74, 85, 90, 101, 106, 117, 122, 133, 138, 149, 154, 165, 170, 181, 186, 197, 202, 213, 218, 229, 234, 245, 250,
  6, 9, 22, 25, 38, 41, 54, 57, 70, 73, 86, 89, 102, 105, 118, 121, 134, 137, 150, 153, 166, 169, 182, 185, 198, 201, 214, 217, 230, 233, 246, 249,
  7, 8, 23, 24, 39, 40, 55, 56, 71, 72, 87, 88, 103, 104, 119, 120, 135, 136, 151, 152, 167, 168, 183, 184, 199, 200, 215, 216, 231, 232, 247, 248
};

/*
   Below a two dimentional array with all the numbers in an Int array (2-D). Here you can custom make your own 'font'
   for now, all characters are fixed in a 8x8 matrix (64) matrix. The character matrix itself is 1D, and subsequenly bundled into a numberMatrix.

*/
int numberMatrix[10][64] =
{
  { // 0
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  { // 1
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  { // 2
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  { // 3
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  { //4
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 0,
    0, 0, 0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  { //5
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  { //6
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  { // 7
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 1, 1, 1, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  { //8
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  { // 9
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  }
};

/*
   The ledMatrix functions will combine the x,y positions with the Led number
*/
int ledMatrix[YRES][XRES]; // is used to make an x.y coordinate map of led numbers
boolean EveningSleep = false;

/*
   Here we combine everything to do the initial configurations/setup
   A bit hardcoded is the
*/
void setup() {
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  delay( 3000 ); //power-up safety delay
  Serial.begin(115200);


  makeXYMatrix(); //make LedMartrixWithXY coordinates

  // fastled Stuff
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setDither(0);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTAGE, AMPS); // set limit to aamount of AMPS we use..
  FastLED.setBrightness(BRIGHTNESS); // set the initial brightness off the whole matrix.
  FastLED.setCorrection(TypicalSMD5050); // correction for colors
  currentPalette =  RainbowColors_p;

  /*
      lets connect to network. And show some light on the matrix
      But.. in case connectToNetwork isn't working, it will start in AccessPoint mode (APmode = true)
  */
  connectToNetwork(); // connect to WiFi.

  /* Ah not connected to a wifi station. So we show an accesspoint and ask for WIFI credentials */
  if (APmode) {
    xTaskCreatePinnedToCore(
      Task2code, /* Function to implement the task */
      "Task2", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task2,  /* Task handle. */
      0); /* Core where the task should run */

    FastLED.delay(1000); // to allow to start the 2nd processor.
  }
  /*
      Ok, we have WIFI, so let's start normally
  */
  else {
    // get time form NTP server;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    updateLocalTime();

    // setup second core tasks
    xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */

    FastLED.delay(1000); // to allow to start the 2nd processor.
  }
}

/*
   This is where the reral action takes place.
   SPIFFS is the memory of the ESP32, which stores all variables even after power-off
   We use this to retreive SSID/PWD info and also which animation and color settings were last used.
*/
void loop() {

  // All our parameters that can be changes from the webpage are stored in SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }


  /* nightMode can be switched on manually
      but also set for a start and end time (hours and minutes)
  */
  boolean Nachtlamp;
 
  if (timeinfo.tm_hour > 12 && timeinfo.tm_hour <= 23 && timeinfo.tm_hour >= nightTimeHour && timeinfo.tm_min >= nightTimeMinute ){ // it is evening, and the NightLamp time is passed. 
    EveningSleep = true;
  }else{
    EveningSleep = false;
  }
  if (EveningSleep == true && timeinfo.tm_hour >= 0 && timeinfo.tm_hour < 12 && timeinfo.tm_hour >= morningTimeHour && timeinfo.tm_min <= morningTimeMinute){ // wake-up
    Nachtlamp = false;
    EveningSleep = false;
  }else if(EveningSleep == false){
    Nachtlamp = false;
  }else{
    Nachtlamp = true;
  //  Serial.println("Nachtlamp = true");
  }

  /*
     Based on the animation selected.. let's display this
     A lot of the interrupts (when we select a diferent animation or setting is done by the second processor..
     In the function called Task1code
  */

  if (Nachtlamp == true || nightMode == true) {
    //Serial.println("Nachtmode");
    nachtmode();
  } else {

    String whichFX = currentAnimation;
    if (whichFX == "oneColorBackground") {
      // setAll(0, 0, 0);
      oneColorBackground(backgroundColor, 128);
    } else if (whichFX == "rainbowCycle") {
      rainbowCycle(20);
    } else if (whichFX == "sparkle") {
      //setAll(0, 0, 0);
      Sparkle(0xff, 0xff, 0xff, 0);
    } else if (whichFX == "runningLights") {
      setAll(0, 0, 0);
      RunningLights(0xff, 0, 0, 50);      // red
      RunningLights(0xff, 0xff, 0xff, 50); // white
      RunningLights(0, 0, 0xff, 50);      // blue
      breakAnimation = false;
    } else if (whichFX == "colorWipe") {
      setAll(0, 0, 0);
      colorWipe(0x00, 0xff, 0x00, 50);
      colorWipe(0x00, 0x00, 0x00, 50);
      breakAnimation = false;
    } else if (whichFX == "Sneeuw") {
      //    setAll(0,0,0);
      SnowSparkle(0x10, 0x10, 0x10, 20, random(100, 1000));
    } else if (whichFX == "randomTwinkle") {
      setAll(0, 0, 0);
      TwinkleRandom(40, 200, false);
    } else if (whichFX == "MeteorRain") {
      meteorRain(0xff, 0xff, 0xff, 10, 64, true, 30);
    } else if (whichFX == "bouncing") {
      byte colors[3][3] = { {0xff, 0, 0},
        {0xff, 0xff, 0xff},
        {0   , 0   , 0xff}
      };
      BouncingColoredBalls(3, colors);
      
    } else if (whichFX == "randomEveryHour") {
      runRandomAnimation(60);
    } else if (whichFX == "randomEveryHalf") {
      runRandomAnimation(30);
    } else if (whichFX == "randomEvery15") {
      runRandomAnimation(15);
    } else {
      //  setAll(0, 0, 0);
      oneColorBackground(CHSV(10, 255, 64), 128);
    }
  }
}
