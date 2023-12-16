/*
   Led Matrix Clock, using and ESP32, version 2.0
   This is a rewrite of the initial code for the LedMartrixESP code
   https://www.tutorialspoint.com/esp32_for_iot/esp32_for_iot_preferences.htm
   Clock design and programming (albeit a lot of code snippets are thanks to the great examples on internet)
    (c) Bas van Breukelen, 2021  Version 2.0
*/

/*
   Library Space
*/
#include <FastLED.h> // No animations without this library
#include <WiFi.h>    // No wifi without this library
#include "Time.h"    // And how about the time.. no idea what time it is without this library
////include <Time.h>
// include <TimeAlarms.h>
#include <String>
#include <AsyncTCP.h>          // Webserver stuff
#include <ESPAsyncWebServer.h> // More webserver stuff
#include <HTTPClient.h>        // Needed to get some information from the internetz.. .like weather data
#include <ElegantOTA.h>        // A library to allow Over The Air Updates :)
#include <Arduino_JSON.h>      // Often this data comes as JSON :)
#include "soc/soc.h"           // Used to make taksk on diferent cores
#include "soc/rtc_cntl_reg.h"
#include <Preferences.h> // Store the prefences in permanent memory in a much easier way than SPIFFS
#include "HTML.h"        // A library containing the HTML pages (included with this project)
#include "MatrixDefs.h"  // A library containing the arrays with matrix defenitions, eg xy positions etc. IMPORTANT as
#include <map>           // We will use hashmaps to store Led position and color data
// you need to edit MatrixDefs.h for YOUR matrix.. Also the digits and characters are defined here..
// SO EDIT that file!!
/*
    Define Space
*/

#define NUM_LEDS 256 // THE NUMBER OF LEDs USED IN THE MATRIX
#define DATA_PIN2 2  // ON WHICH PIN WILL THE DATA BE SEND
#define DATA_PIN12 12
#define DATA_PIN18 18
#define BRIGHTNESS 255                    // INITIAL BRIGHNESS OF THE MATRIX.
#define VOLTAGE 5                         // THE VOLTAGE OUR SYSTEM USES
#define AMPS 5000                         // THE CURRENT THAT YOUR POWERBRICK CAN DELIVER e.g 5 AMP (or 5000 mA)
#define qsubd(x, b) ((x > b) ? b : 0)     // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b) ((x > b) ? x - b : 0) // Analog Unsigned subtraction macro. if result <0, then => 0

/*
   Global spaceO
   Below all the GLOBAL Variables are stored.
   Whilest we do not want to have too many of those. It cannot always be prevented.
*/
// PREFERENCES
Preferences preferences; // We will store our variables here that we don't want to loose

// WIFI
const char *ssidAP = "ESP32-LedMatrixClock"; // IN CASE WE CANNOT CONNECT TO THE WIFI THE ESP32 WILL PRESENT ITSELF AS AN ACCESSPOINTER USING THIS NAME
const char *passwordAP = "1234abcd";         // AND YOU CAN ACCESS THAT ACCESSPOINT USING THIS SIMPLE PASSWORD
AsyncWebServer server(80);                   // Create AsyncWebServer object on port 80

// Do you have a LDR (light dependent resistor installed? if so, which pin)
const int sensorPin = 34; // AN LDR is installed on PIN 34. (10kOhm between GND and 3.3V on positive side)
boolean useLDR = true;    // THE LDR IS OPTIONAL, IT WILL BE USED TO CONTROL THE OVERALL BRIGHTNESS IS PRESENT
int lightInit;            // initial value
int lightVal;             // light reading

// pins settings from wifi
int ledpin = 0; // We can use preferences to set the PINS to some other setting.
int ldrpin = sensorPin;

// CLOCK&TIME SETTING
const char *ntpServer = "nl.pool.ntp.org"; // THIS IS THE WEB ADDRESS OF THE EUROPEAN NETWORK TIME SERVER
const long gmtOffset_sec = 3600;           // WE LIVE IN THE NETHERLANDS => UTC+1H
const int daylightOffset_sec = 3600;       // YES THE NETHERLANDS IS STILL ON DAYLIGHT SAVINGS
const char *defaultTimezone = "CET-1CEST,M3.5.0/2,M10.5.0/3";
struct tm timeinfo; // TIME OBJECT

//  SETTINGS FOR THE MATRIX LIKE COLORS, BRIGHTNESS, BACKGROUND ETC, THAT CAN BE CHANGES DURING RUNTIME
CRGB digitColor = CRGB::Red;                    // THE INITIAL COLOR OF THE DIGITS IS RED
CRGB backgroundColor = CRGB::Green;             // THE INITIAL COLOR OF THE BACKGROUND IS GREEN
String currentAnimation;                        // THE NAME OF THE ANIMATION THAT PLAYS
bool breakAnimation = false;                    // A BOOLEAN VALUE USED TO STOP AN ANIMATION BEFORE IT REALLY HAS ENDED
CRGBPalette16 currentPalette = RainbowColors_p; // CONTAINER FOR THE COLOR PALETTE USED FOR ANIMATIONS
CRGBPalette16 currentPalette2 = RainbowColors_p;
CRGBPalette16 targetPalette;                // AS ABOVE
CRGBPalette16 targetPalette2;               // AS ABOVE
TBlendType currentBlending = LINEARBLEND;   // CONTAINER FOT THE BLENDINGTYPE USED IN ANIMATIONS
extern CRGBPalette16 myRedWhiteBluePalette; // AN IMPORTED COLOR PALETTE FOR ANIMATION

uint8_t backgroundBrightness = 128; // THE BACKGROUND BRIGHTNESS (ALL LEDS EXCEPT THE DIGITS)
uint8_t digitBrightness = 128;      // THE DIGIT BRIGHNESS (ALL LEDS EXCEPT THE BACKGROUND)
uint8_t overAllBrightness = 128;    // THE OVERALL BRIGHTNESS AFFECTS ALL LEDS.
int ledClockBrightNess = 128;       // IS USED TO SCALE THE LEDCLOCK WHEN A LDR IS USED
boolean nightMode = false;          // IS THE MATRIX IN NIGHT (BEDTIME) MODE
boolean useOld = false;             // WHEN TRUE THE TIMEMATIX WILL TAKE THE LEDs COLOR THAT WAS ALREADY THERE
int scrollspeed = 150;              // DEFAULT TEXT SCROLLING SPEED. LOWER IS FASTER. ITS DEFINED AS A STEP PER XX MILLISECONDS

int nightTimeHour = 22; // THE NEXT SETTINGS ARE USED TO SET A TIME IN BETWEEN THE MATRIX IS IN NIGHTMODE
int nightTimeMinute = 00;
int morningTimeHour = 7;
int morningTimeMinute = 00;
boolean EveningSleep = false;
String digitAnimation = "Aan"; // Aan (=On) means, digits are visible. (Uit is OFF and Regenboog is rainbow fx)
int iRebooted = 0;
int Seconde;
int OldSeconde;
int Minute;
int OldMinute;
int Hour;
int OldHour;
int Day;
int Month;
int Year;
boolean timeChangeAnimation = false; // Animate the time change...
int yloc = 0;                        // y location general counter
int ylocmin = 0;                     // y location for upscrolling time minutes
int ylochour = 0;                    // y location for upscrolling hours
String animatechange = "Yes";        // do we want to use this animation
boolean tenminuteJump = false;       // every 10 minutes we need to move 2 digits!
boolean hourjump = false;            // 10 hours we need to move 2 digits
int ydigit1 = 0;
int ydigit2 = 0;
boolean hourhaschanged = false;
int hydigit1 = 0;
int hydigit2 = 0;

String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=";
String city = "Arnhem";  // This needs to be set via the webinterface.
String apikey;           // This API key will be set via the webinterface
String units = "metric"; // Set to standard, metric or imperial in case you need degrees celsius of F.
String weatherJSON = "{}";

String BGHTMLcol = "#000000";
String digitHTMLcol = "#000000";
// String digitAnimation = "none";
String randomAnimation = "No random animation"; // The random Animation is no
String randomWhat = "Both";                     // The random animator will default randomize both digit and background animations
int randomNumber = 0;
int randomDigitNumber = 0;
String scrolltext = "love";

// MULTI CPU TASKS
TaskHandle_t Wifimode; // CONTAINER USED FOR TASKS
TaskHandle_t APmode;   // CONTAINER USED FOR TASKS (aka threads, but slightly different)

// THE LED MATRIX 'CURSORS'
int locationCursorX = 0; // our first pixel X is 0
int locarionCursorY = 0; // our first pixel Y is 0

CRGB matrix[NUM_LEDS];          // THE TOTAL NUMBER OF LEDS USED IN THE MATRIX (should be XRES * YRES)
std::map<int, CRGB> timeMatrix; // This creates a Map (sort of array) containing, the led numbers that are part of the digits
std::map<int, CRGB> bgMatrix;   // This creates a Map of the bg LEDs (= all matrix - digits)
String whichFX;                 // Which effect on the background will we play
// std::map<int, CRGB> digits;                         // A hashmap will be made to store the time digirs with color information.
String oldFX;           // We also store the old or previous fx
int myhue = 0;          // Needed for some FX's
uint8_t onOffBG = 0;    // counter for some FX's
uint8_t onOffFG = 0;    // counter for some FX's
int thatOneBGPixel = 0; // sometimes we need a global that tells which pixel was lid
int thatOneFGPixel = 0; // idem
int textScroller = 0;   // this determines the position of the scroller for the Text animation

// FORWARD DECLERATION OF FUNCTIONS
std::map<int, CRGB> makeDigits(int *, std::map<int, CRGB>, int, int, boolean, CRGB);
std::map<int, CRGB> getTimeMatrix(String, CRGB);
std::map<int, CRGB> showText(String, CRGB, int);
std::map<int, CRGB> makeBgMap(int);
void mergeMapsToLeds(std::map<int, CRGB>, std::map<int, CRGB>, int, int, boolean, boolean);
std::map<int, CRGB> getBackgroundMap(std::map<int, CRGB>);
std::map<int, CRGB> oneColorBackground(std::map<int, CRGB>, CRGB);
std::map<int, CRGB> plasma(std::map<int, CRGB>, boolean);
std::map<int, CRGB> dot_beat(std::map<int, CRGB>);
std::map<int, CRGB> beatWave(std::map<int, CRGB>);
std::map<int, CRGB> sinelon(std::map<int, CRGB>);
std::map<int, CRGB> rippless(std::map<int, CRGB>);
std::map<int, CRGB> displayTimeNoAnimation(std::map<int, CRGB>);
std::map<int, CRGB> Sparkle(std::map<int, CRGB>, CRGB, boolean, int);
std::map<int, CRGB> confetti(std::map<int, CRGB>);
std::map<int, CRGB> recolorDigits(std::map<int, CRGB>, CRGB);

/* some strucs for animation */
#define maxRipples 6
struct ripple
{ // Reko Meriö's structures

  // Local definitions

  // Persistent local variables

  // Temporary local variables
  uint8_t brightness; // 0 to 255
  int8_t color;       // 0 to 255
  int16_t pos;        // -1 to NUM_LEDS  (up to 127 LED's)
  int8_t velocity;    // 1 or -1
  uint8_t life;       // 0 to 20
  uint8_t maxLife;    // 10. If it's too long, it just goes on and on. . .
  uint8_t fade;       // 192
  bool exist;         // 0 to 1

  void Move()
  {
    pos += velocity;
    life++;
    if (pos > NUM_LEDS - 1)
    { // Bounce back from far end.
      velocity *= -1;
      pos = NUM_LEDS - 1;
    }
    if (pos < 0)
    { // Bounce back from 0.
      velocity *= -1;
      pos = 0;
    }
    brightness = scale8(brightness, fade); // Adjust brightness accordingly to strip length
    if (life > maxLife)
      exist = false; // Kill it once it's expired.
  }                  // Move()

  void Init(uint8_t Fade, uint8_t MaxLife)
  {                                                       // Typically 216, 20
    pos = random8(NUM_LEDS / 8, NUM_LEDS - NUM_LEDS / 8); // Avoid spawning too close to edge.
    velocity = 1;                                         // -1 or 1
    life = 0;                                             // 0 to Maxlife
    maxLife = MaxLife;                                    // 10 called
    exist = true;                                         // 0, 1
    brightness = 255;                                     // 0 to 255
    color = millis();                                     // hue;
    fade = Fade;                                          // 192 called
  }                                                       // Init()
};                                                        // struct ripple

typedef struct ripple Ripple;

Ripple ripples[maxRipples];

/* some variable for an other FX library */
uint8_t thisfade = 8;  // How quickly does it fade? Lower = slower fade rate.
int thishue = 50;      // Starting hue.
uint8_t thisinc = 1;   // Incremental value for rotating hues
uint8_t thissat = 100; // The saturation, where 255 = brilliant colours.
uint8_t thisbri = 255; // Brightness of a sequence. Remember, max_bright is the overall limiter.
int huediff = 256;     // Range of random #'s to use for hue
uint8_t thisdelay = 5; // We don't need much delay (if any)

// dots_beat
uint8_t count = 0;     // Count up to 255 and then reverts to 0
uint8_t fadeval = 224; // Trail behind the LED's. Lower => faster fade.

uint8_t bpm = 30;

/*
   Initialize everything we need
   We start by getting all paramters that were stored in the permanent memory
   Followed by initializing the matrix and matrix
   Connecting to the WIFI network, and in case this doesnt work we start an accesspoint to get wifi credentials
   Next we get the Network TIme..
   All set.. we can go to the loop
*/

void setup()
{

  delay(3000);          // power-up safety delay
  Serial.begin(115200); // Make sure we can print to the serial monitor
  Serial.println("Starting LED MATRIX CLOCK (c) BvB2021 ");
  Serial.print("Setup runs on core: ");
  Serial.println(xPortGetCoreID());
  getStoredParameters(); // MAKE SURE WE LOAD IN ALL OUR SETTINGS TO THE GLOBALS. (might replace with Preferences lib).
  if (ledpin == 0)
  {
    ledpin = 2; // DATA_PIN;
  }
  const int pin = ledpin;
  makeXYMatrix(); // make LedMartrixWithXY coordinates
  if (ledpin == 2)
  {
    FastLED.addLeds<WS2812B, DATA_PIN2, GRB>(matrix, NUM_LEDS).setCorrection(TypicalLEDStrip);
  }
  else if (ledpin == 12)
  {
    FastLED.addLeds<WS2812B, DATA_PIN12, GRB>(matrix, NUM_LEDS).setCorrection(TypicalLEDStrip);
  }
  else if (ledpin == 18)
  {
    FastLED.addLeds<WS2812B, DATA_PIN18, GRB>(matrix, NUM_LEDS).setCorrection(TypicalLEDStrip);
  }
  else
  {
    FastLED.addLeds<WS2812B, DATA_PIN2, GRB>(matrix, NUM_LEDS).setCorrection(TypicalLEDStrip);
  }

  // FastLED.addLeds<WS2812B, DATA_PIN, GRB>(matrix, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setDither(0);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTAGE, AMPS); // set limit to aamount of AMPs we use..
  FastLED.setBrightness(overAllBrightness);              // set the initial brightness off the whole matrix.
  FastLED.setCorrection(TypicalSMD5050);                 // correction for colors
  matrix[0] = CRGB::Green;                               // Make first led green to show matrix is on and running
  FastLED.show();

  if (useLDR)
  {                                    // Here we start using or not the LDR
    lightInit = analogRead(sensorPin); // we will take a single reading from the light sensor and store it in the lightCal variable. This will give us a prelinary value to compare against in the loop
  }
  else
  {
    lightInit = 0;
  }

  /*
     Let's setup a wifi connection
     False: no wifi connection to router. If that is the case, we will setup an AP
     True: yeah... we have a network connection. An now we can proceed!!
     We will run our webserver stuff on a different core, so all the led stuff can stay on the other core..
  */
  boolean connectedToNetwork = false; // We want to know if we have a network before proceeding

  preferences.begin("wificreds", false); // The WIFI credentials are stored here
  delay(1000);
  unsigned int counter = preferences.getUInt("counter", 0); // Counter to see how often we boot up
  preferences.getString("ssid");
  counter++;

  Serial.printf("Current restart counts: %u\n", counter);
  // Store the counter to the Preferences
  preferences.putUInt("counter", counter);
  iRebooted = counter;
  // Connect to network
  // This can be false... no WIFI access and we need a way to set this via ACCESSPOINT more.
  if (connectToNetwork(preferences.getString("ssid"), preferences.getString("password")))
  {
    matrix[1] = CRGB::Green;
    FastLED.show();
    xTaskCreatePinnedToCore(
        RunWebserver,   // Function to implement the task
        "RunWebserver", // Name of the task
        10000,          // Stack size in words
        NULL,           // Task input parameter
        0,              // Priority of the task
        &Wifimode,      // Task handle.
        0);             // Core where the task should run

    FastLED.delay(1000); // to allow to start the 2nd processor.
    connectedToNetwork = true;
  }
  else
  {
    Serial.println("No WIFI, let's offer an accesspoint");
    matrix[1] = CRGB::Red;
    FastLED.show();
    // We can start as ACCESSPOINT
    xTaskCreatePinnedToCore(
        RunAPmode,
        "RunAPmode",
        10000,
        NULL,
        0,
        &APmode,
        0);

    FastLED.delay(1000); // to allow to start the 2nd processor.
    connectedToNetwork = false;
  }
  while (!connectedToNetwork)
  { // When we are not connected to a router (wifi point) we should not proceed. But only show the accesspoint website
    EVERY_N_SECONDS(30)
    {
      // Serial.println("ESP Matrix Clock is in AccessPoint mode. Please enter your SSID and Key in the Accesspoint website (connect to the AP-wifi first)");
      displayIPAP();
      // matrix[1] = CRGB::Orange;
      // FastLED.show();
      // FastLED.delay(1000);
      // matrix[1] = CRGB::Red;
      // FastLED.show();
    }
  }
  configTzTime(defaultTimezone, ntpServer); // sets TZ and starts NTP sync
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if (updateLocalTime())
  {
    Serial.println("We have network time");
    matrix[2] = CRGB::Green;
    FastLED.show();
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    displayIP(); // Display the last digits of the IP on the matrix.
  }
  else
  {
    Serial.println("No Network time was obtained, therefore we will reboot to try again");
    ESP.restart();
  }
  preferences.end();                                          // You quit prefs for now.. need to open BEGIN it again when needed.
  timeMatrix = getTimeMatrix("none", CRGB::Black);            // This is the time matrix.. lets make sure it exsists
  weatherJSON = getTheWeather(endpoint, apikey, city, units); // Get the initial weather data
  Serial.println("Setup of matrixclock finished. Let's rock and roll");
  Serial.println(" ");
  OldHour = Hour;
  OldMinute = Minute;

} // setup()

/*
   The actual program.. that loops and loops and loops and loops and.....
*/

void loop()
{

  // TimeChange animation
  if (Minute == OldMinute && Hour == OldHour)
  {
  }
  else
  {
    // Serial.println("Animation time ");
    if (animatechange == "Yes")
    {
      timeChangeAnimation = true;
    }
    else
    {
      OldHour = Hour;
      OldMinute = Minute;
    }
  }

  EVERY_N_SECONDS(1)
  {
    if (useLDR)
    { // If we have a LDR, we can here 'read' the amount of light and adjust the overall brightness
      //  Serial.println(analogRead(sensorPin));
      int MeasuredValue = analogRead(sensorPin);
      if (MeasuredValue > 4000)
      {
        ledClockBrightNess = overAllBrightness;
      }
      else if (MeasuredValue > 3000 && MeasuredValue <= 4000)
      {
        ledClockBrightNess = overAllBrightness * 3 / 4;
      }
      else if (MeasuredValue > 2000 && MeasuredValue <= 3000)
      {
        ledClockBrightNess = overAllBrightness / 2;
      }
      else if (MeasuredValue > 1000 && MeasuredValue <= 2000)
      {
        ledClockBrightNess = overAllBrightness / 4;
      }
      else if (MeasuredValue >= 0 && MeasuredValue <= 1000)
      {
        ledClockBrightNess = overAllBrightness / 5;
      }
      // ledClockBrightNess = overAllBrightness;
    }
    else
    {
      ledClockBrightNess = overAllBrightness;
    }
  }

  String activity = whichFX;               // Default animation
  String digitActivity = digitAnimation;   // Default animation of digits
  int bgBrightness = backgroundBrightness; // This can be set via the webpage, but some animations can override this
  int fgBrightness = digitBrightness;      // This can be set via webpage
  boolean bgToBlack = false;               // if background dimmming is a fade to black or not
  boolean fgToBlack = false;               // if digits (foreground) dimming is a fade to black of not

  FastLED.setBrightness(ledClockBrightNess); // SET the MAtrix brightness to the global setting

  EVERY_N_MILLISECONDS(500)
  { // update the global time variables
    updateLocalTime();
  }

  EVERY_N_SECONDS(60)
  { // Every minute we check if the clock has to go to nightmode, a mode with low light and no animations.
    nightModeOnOrOff();
  }

  int arrlength = 9;
  String animationArray[arrlength] = {// Here we set all the effects that can be used as random animation.
                                      "Sweep", "plasma", "rainbowWaves", "dotsBeat", "confetti", "ripples", "sparkle", "showText", "showWeather"};

  int arrl = 4;
  String animationDigits[arrl] = {
      "Rainbow", "Plasma", "Sweep", "Sparkle"};

  EVERY_N_SECONDS(90)
  { // Every 90seconds we we generate a random number, which is the random animation we will show.
    randomNumber = rand() % arrlength;
    randomDigitNumber = rand() % arrl;
  }
  String animation = animationArray[randomNumber];
  String digiani = animationDigits[randomDigitNumber];

  if (randomAnimation == "No Random animation")
  { //  If we want a random animation every ... minutes.. let's check if its time.
    activity = whichFX;
    digitActivity = digitAnimation;
  }
  else if (randomAnimation == "Random every 60 minutes")
  {
    if (runRandomAnimation(60))
    {
      activity = animation;
      digitActivity = digiani;
      /* some animations just look better with a specific digit animation */
      if (animation == "rainbowWaves")
      {
        digitActivity = "Dark digits";
      }
      else if (animation == "showText")
      {
        digitActivity = "Rainbow";
      }
    }
  }
  else if (randomAnimation == "Random every 30 minutes")
  {
    if (runRandomAnimation(30))
    {
      activity = animation;
      digitActivity = digiani;
      /* some animations just look better with a specific digit animation */
      if (animation == "rainbowWaves")
      {
        digitActivity = "Dark digits";
      }
      else if (animation == "showText")
      {
        digitActivity = "Rainbow";
      }
    }
  }
  else if (randomAnimation == "Random every 15 minutes")
  {
    if (runRandomAnimation(15))
    {
      activity = animation;
      digitActivity = digiani;
      /* some animations just look better with a specific digit animation */
      if (animation == "rainbowWaves")
      {
        digitActivity = "Dark digits";
      }
      else if (animation == "showText")
      {
        digitActivity = "Rainbow";
      }
    }
  }
  else if (randomAnimation == "Random every 1 minute")
  {
    activity = animation;
    digitActivity = digiani;
    /* some animations just look better with a specific digit animation */
    if (animation == "rainbowWaves")
    {
      digitActivity = "Dark digits";
    }
    else if (animation == "showText")
    {
      digitActivity = "Rainbow";
    }
  }

  /**
     Below you find the call's to the different animations.. or actions as I call them. Not always it will be an animation
     First part is for the background led's and the second part is the animation of the foreground, or digits/texts
  */

  if (nightMode)
  {                                                       // If the clock is in Nightmode, we don't need much logic. only display the digits in faint red.
    FastLED.setBrightness(96);                            // lower the brightness of the matrix
    timeMatrix = getTimeMatrix("none", CHSV(0, 255, 32)); // This is the time matrix.. letters are red and faint (32)
    bgMatrix = getBackgroundMap(timeMatrix);              // We need a background matrix as well..  default
    bgMatrix = oneColorBackground(bgMatrix, CRGB::Black); // And we will give it a black background.
    bgBrightness = 255;
    fgBrightness = 0;
    useOld = false;
    mergeMapsToLeds(bgMatrix, timeMatrix, bgBrightness, fgBrightness, false, false); // Merge both matrices. before we display.
  }
  else
  {
    // Normal mode
    if (digitAnimation == "Sweep")
    { // Some digit animations require the timeMatrix not to be updated with a color
      useOld = true;
    }
    else
    {
      useOld = false;
    }

    //    timeMatrix = getTimeMatrix("none", digitColor);

    if (activity == "dotsBeat")
    {
      timeMatrix = getTimeMatrix(digitAnimation, digitColor);
      bgMatrix = getBackgroundMap(timeMatrix);
      bgMatrix = dot_beat(bgMatrix);
      bgBrightness = 0;
      //  mergeMapsToLeds(bgMatrix, timeMatrix, backgroundBrightness, digitBrightness);
    }
    else if (activity == "oneColorBackground")
    {
      timeMatrix = getTimeMatrix(digitAnimation, digitColor);   // This is the time matrix.. letters are red and faint (32)
      bgMatrix = getBackgroundMap(timeMatrix);                  // We need a background matrix as well..  default
      bgMatrix = oneColorBackground(bgMatrix, backgroundColor); // And we will give it a black background.
    }
    else if (activity == "plasma")
    {
      currentPalette = RainbowColors_p;
      EVERY_N_MILLISECONDS(50)
      {                                                         // FastLED based non-blocking delay to update/display the sequence.
        timeMatrix = getTimeMatrix(digitAnimation, digitColor); // This is the time matrix.. letters are red and faint (32)
        bgMatrix = getBackgroundMap(timeMatrix);                // We need a background matrix as well..  default
        bgMatrix = plasma(bgMatrix, false);
      }
      EVERY_N_SECONDS(5)
      {                            // Change the target palette to a random one every 5 seconds.
        uint8_t baseC = random8(); // You can use this as a baseline colour if you want similar hues in the next line.
        targetPalette = CRGBPalette16(CHSV(baseC + random8(32), 192, random8(128, 255)), CHSV(baseC + random8(32), 255, random8(128, 255)), CHSV(baseC + random8(32), 192, random8(128, 255)), CHSV(baseC + random8(32), 255, random8(128, 255)));
      }
    }
    else if (activity == "rainbowWaves")
    {
      currentPalette = RainbowColors_p;
      EVERY_N_MILLISECONDS(5)
      {
        timeMatrix = getTimeMatrix(digitAnimation, digitColor);
        bgMatrix = getBackgroundMap(timeMatrix);
        bgMatrix = beatWave(bgMatrix);
      }
      EVERY_N_MILLISECONDS(100)
      {
        uint8_t maxChanges = 24;
        nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges); // AWESOME palette blending capability.
      }
      EVERY_N_SECONDS(5)
      { // Change the target palette to a random one every 5 seconds.
        targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
      }
    }
    else if (activity == "Sweep")
    {
      timeMatrix = getTimeMatrix("none", digitColor);
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;

      EVERY_N_MILLISECONDS(100)
      {
        uint8_t maxChanges = 24;
        nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
      }

      EVERY_N_SECONDS(5)
      {                                   // Change the target palette to a random one every 5 seconds.
        static uint8_t baseC = random8(); // You can use this as a baseline colour if you want similar hues in the next line.
        targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
      }

      EVERY_N_MILLISECONDS(50)
      { // FastLED based non-blocking delay to update/display the sequence.
        bgMatrix = getBackgroundMap(timeMatrix);
        bgMatrix = sinelon(bgMatrix); // Call our sequence.
        bgBrightness = 32;            // It now determines the fading of the BG
      }
      bgBrightness = 32; // It now determines the fading of the BG
      bgToBlack = true;
    }
    else if (activity == "ripples")
    {
      timeMatrix = getTimeMatrix("none", digitColor);
      currentPalette2 = OceanColors_p;  // Use palettes instead of direct CHSV or CRGB assignments
      targetPalette2 = RainbowColors_p; // Also support smooth palette transitioning
      EVERY_N_MILLISECONDS(50)
      { // Smooth palette transitioning runs continuously.
        uint8_t maxChanges = 24;
        nblendPaletteTowardPalette(currentPalette2, targetPalette2, maxChanges);
      }
      EVERY_N_SECONDS(5)
      {
        SetupMySimilar4Palette(); // Change palette colours every 5 seconds.
      }
      EVERY_N_MILLIS(50)
      { // Sets the original delay time.
        bgMatrix = getBackgroundMap(timeMatrix);
        bgMatrix = rippless(bgMatrix); // Run the ripple routine.
      }
      bgBrightness = 128;
      bgToBlack = true;
    }
    else if (activity == "showText")
    {

      if (digitActivity == "Sparkle")
      {
        digitActivity = "No animation";
      }
      else if (digitActivity == "Sweep")
      {
        digitActivity = "No animation";
      }

      String minuten = String(Minute);
      if (minuten.length() < 2)
      {
        minuten = "0" + minuten;
      }
      String tijd = String(Hour) + ":" + minuten;
      String newScrolltext = tijd + " - " + scrolltext;
      int textlength = newScrolltext.length();

      EVERY_N_MILLISECONDS(scrollspeed)
      {
        textScroller++;
        if (textScroller >= textlength * 7)
        {
          textScroller = -34;
        }
        timeMatrix = showText(newScrolltext, digitColor, textScroller);
        bgMatrix = getBackgroundMap(timeMatrix);
        bgMatrix = oneColorBackground(bgMatrix, backgroundColor); // And we will give it a black background.
      }
    }
    else if (activity == "showWeather")
    {

      if (digitActivity == "Sparkle")
      {
        digitActivity = "No animation";
      }
      else if (digitActivity == "Sweep")
      {
        digitActivity = "No animation";
      }

      String tempOutside = returnFromJSON(weatherJSON, "main", "temp");
      String maxtempOutside = returnFromJSON(weatherJSON, "main", "temp_max");
      String mintempOutside = returnFromJSON(weatherJSON, "main", "temp_min");
      String textForDisplay = " " + tempOutside + "'C" + " ^" + maxtempOutside + "'C %" + mintempOutside + "'C";
      String minuten = String(Minute);

      EVERY_N_SECONDS(60)
      {
        weatherJSON = getTheWeather(endpoint, apikey, city, units);
        tempOutside = returnFromJSON(weatherJSON, "main", "temp");

        maxtempOutside = returnFromJSON(weatherJSON, "main", "temp_max");
        mintempOutside = returnFromJSON(weatherJSON, "main", "temp_min");

        textForDisplay = " " + tempOutside + "'C" + " ^" + maxtempOutside + "'C %" + mintempOutside + "'C";
        minuten = String(Minute);
      }

      if (minuten.length() < 2)
      {
        minuten = "0" + minuten;
      }
      String tijd = String(Hour) + ":" + minuten;
      textForDisplay = tijd + " - " + textForDisplay;

      int textlength = textForDisplay.length();

      EVERY_N_MILLISECONDS(scrollspeed)
      {
        textScroller++;
        if (textScroller >= textlength * 7)
        {
          textScroller = -34;
        }
        // timeMatrix.clear();
        timeMatrix = showText(textForDisplay, digitColor, textScroller);
        bgMatrix = getBackgroundMap(timeMatrix);
        bgMatrix = oneColorBackground(bgMatrix, backgroundColor); // And we will give it a black background.
      }
      // digitActivity = "Rainbow";
    }
    else if (activity == "sparkle")
    {
      timeMatrix = getTimeMatrix("none", digitColor);
      bgMatrix = getBackgroundMap(timeMatrix);
      bgMatrix = oneColorBackground(bgMatrix, backgroundColor);
      EVERY_N_MILLIS(10)
      {
        if ((onOffBG & 1) == 0)
        { // even or odd.. blink of not
          int rndNum = rand() % 5;
          if (rndNum < 2)
          { // by introducing a random change.. the blinks will become random
            bgMatrix = Sparkle(bgMatrix, CRGB::White, true, thatOneBGPixel);
            onOffBG++;
          }
        }
        else
        {
          onOffBG = 0;
          bgMatrix = Sparkle(bgMatrix, backgroundColor, false, thatOneBGPixel);
        }
      }
    }
    else if (activity == "confetti")
    {
      timeMatrix = getTimeMatrix("none", digitColor);
      currentPalette = RainbowColors_p;
      EVERY_N_MILLISECONDS(5)
      { // FastLED based non-blocking delay to update/display the sequence.
        bgMatrix = getBackgroundMap(timeMatrix);
        bgMatrix = confetti(bgMatrix);
      }
      bgBrightness = 8;
      bgToBlack = true;
    }
    else
    {
      timeMatrix = getTimeMatrix(digitAnimation, digitColor);   // This is the time matrix.. letters are red and faint (32)
      bgMatrix = getBackgroundMap(timeMatrix);                  // We need a background matrix as well..  default
      bgMatrix = oneColorBackground(bgMatrix, backgroundColor); // And we will give it a black background.
    }
  }

  /*
     Below all the animation on the level of the digits are implemented
  */

  // animations of the digits
  if (!nightMode)
  { // in nightmode no digit animation will happen.
    if (digitActivity == "Plasma")
    {
      EVERY_N_MILLIS(5)
      {
        currentPalette = RainbowColors_p;
        timeMatrix = plasma(timeMatrix, true);
        currentPalette = OceanColors_p;
      }

      EVERY_N_SECONDS(5)
      {                            // Change the target palette to a random one every 5 seconds.
        uint8_t baseC = random8(); // You can use this as a baseline colour if you want similar hues in the next line.
        targetPalette = CRGBPalette16(CHSV(baseC + random8(32), 192, random8(128, 255)), CHSV(baseC + random8(32), 255, random8(128, 255)), CHSV(baseC + random8(32), 192, random8(128, 255)), CHSV(baseC + random8(32), 255, random8(128, 255)));
      }
    }
    else if (digitActivity == "Rainbow")
    {
      currentPalette = RainbowColors_p;
      timeMatrix = beatWave(timeMatrix);
      if (activity != "rainbowWaves")
      { // in case the activity is also rainbowWaves for the background, we can skip this
        EVERY_N_MILLISECONDS(100)
        {
          uint8_t maxChanges = 24;
          nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges); // AWESOME palette blending capability.
        }
        EVERY_N_SECONDS(5)
        { // Change the target palette to a random one every 5 seconds.
          targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
        }
      }
    }
    else if (digitActivity == "Sweep")
    {
      EVERY_N_MILLISECONDS(50)
      { // FastLED based non-blocking delay to update/display the sequence.
        timeMatrix = sinelon(timeMatrix);
      }
      fgBrightness = 32; // It now determines the fading of the BG
      fgToBlack = false;

      if (activity != "Sweep")
      {
        currentPalette = RainbowColors_p;
        currentBlending = LINEARBLEND;
        EVERY_N_MILLISECONDS(100)
        {
          uint8_t maxChanges = 24;
          nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
        }
        EVERY_N_SECONDS(5)
        {                                   // Change the target palette to a random one every 5 seconds.
          static uint8_t baseC = random8(); // You can use this as a baseline colour if you want similar hues in the next line.
          targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
        }
      }
    }
    else if (digitActivity == "Sparkle")
    {
      EVERY_N_MILLIS(10)
      {
        if ((onOffFG & 1) == 0)
        { // even or odd.. blink of not
          int rndNum = rand() % 12;
          if (rndNum < 2)
          { // by introducing a random change.. the blinks will become random
            timeMatrix = Sparkle(timeMatrix, CRGB::White, true, thatOneFGPixel);
            onOffFG++;
          }
        }
        else
        {
          onOffFG = 0;
          timeMatrix = Sparkle(timeMatrix, digitColor, false, thatOneFGPixel);
        }
      }
    }
    else if (digitActivity == "No animation")
    {
      // timeMatrix = recolorDigits(timeMatrix, digitColor);
      fgToBlack = false;
    }
    else if (digitActivity == "Dark digits")
    {
      timeMatrix = recolorDigits(timeMatrix, CRGB::Black);
      fgToBlack = false;
    }

    else
    { // no digit animation.. just get the time Matrix
      timeMatrix = getTimeMatrix("none", CRGB::White);
      fgToBlack = false;
    }
    mergeMapsToLeds(bgMatrix, timeMatrix, bgBrightness, fgBrightness, bgToBlack, fgToBlack); // Merge both matrices. before we display.
  }

  FastLED.show();
} // loop()
