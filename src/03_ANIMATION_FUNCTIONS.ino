/*
   Here we collect our animations and effects
   Best practices:
   - is to build animations without loops... (yeah not always possible).
   -- If there is a loop involved, we need somthing to 'break' this loop on demand.
   - Also, is possible, the function doesn't call for fastLED.show.


   Below are many functions that create a 'background' animation nut also other functions
   mostly taken from  https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#FastLEDFramework

*/

std::map<int, CRGB> beatWave(std::map<int, CRGB> Matrix)
{

  uint8_t wave1 = beatsin8(9, 0, 255); // That's the same as beatsin8(9);
  uint8_t wave2 = beatsin8(8, 0, 255);
  uint8_t wave3 = beatsin8(7, 0, 255);
  uint8_t wave4 = beatsin8(6, 0, 255);

  for (auto const &t : Matrix)
  { // make from the map and array int[lednumber]=realposition
    Matrix[t.first] = ColorFromPalette(currentPalette, t.first + wave1 + wave2 + wave3 + wave4, 255, currentBlending);
    //    matrix[t.first].fadeLightBy( dbrightness );
  }
  return Matrix;
} // beatWave()

std::map<int, CRGB> displayTimeNoAnimation(std::map<int, CRGB> timeMatrix)
{
  for (auto const &t : timeMatrix)
  {
    timeMatrix[t.first] = t.second;
    //   matrix[t.first].fadeLightBy( dbrightness );
  }
  return timeMatrix;
} // displayNoAnimation()

std::map<int, CRGB> oneColorBackground(std::map<int, CRGB> bg, CRGB bgcolor)
{
  for (auto const &t : bg)
  {
    bg[t.first] = bgcolor;
  }
  return bg;
} // oneColorBackground()

// Ripples can only be used for the background!!
std::map<int, CRGB> rippless(std::map<int, CRGB> Matrix)
{

  for (int i = 0; i < maxRipples; i += 2)
  { // Check to see if ripple has expired, and if so, create a new one.
    if (random8() > 224 && !ripples[i].exist)
    {                                // Randomly create a new ripple if this one has expired.
      ripples[i].Init(192, 12);      // Initialize the ripple array with Fade and MaxLife values.
      ripples[i + 1] = ripples[i];   // Everything except velocity is the same for the ripple's other half. Position starts out the same for both halves.
      ripples[i + 1].velocity *= -1; // We want the other half of the ripple to go opposite direction.
    }
  }

  for (int i = 0; i < maxRipples; i++)
  { // Move the ripple if it exists
    if (ripples[i].exist)
    {
      //    matrix[ripples[i].pos] = ColorFromPalette(currentPalette, ripples[i].color, ripples[i].brightness, LINEARBLEND);
      Matrix[ripples[i].pos] = ColorFromPalette(currentPalette2, ripples[i].color, ripples[i].brightness, LINEARBLEND);
      ripples[i].Move();
    }
  }
  return Matrix;
  // fadeToBlackBy(leds, NUM_LEDS, 160);

} // rippless()

void SetupMySimilar4Palette()
{ // Create a palette with colours close to each other.

  uint8_t thishue = random8();

  targetPalette2 = CRGBPalette16(CHSV(thishue + random8(32), 255, random8(128, 255)),
                                 CHSV(thishue + random8(32), 255, random8(192, 255)),
                                 CHSV(thishue + random8(32), 192, random8(192, 255)),
                                 CHSV(thishue + random8(32), 255, random8(128, 255)));

} // SetupMySimilar4Palette()

/*
   create a flasing led effect all over the matrix
*/

/*
   This animation will make one pixel flash for a short moment in the CRGB pixelcolor.
   It needs some globals because we need to remember which pixel flashed and turn it back off.
*/
std::map<int, CRGB> Sparkle(std::map<int, CRGB> Matrix, CRGB pixelcolor, boolean OnOff, int whichPixel)
{

  // int Pixel = random(Matrix.size());
  auto it = Matrix.begin();
  std::advance(it, rand() % Matrix.size());

  if (OnOff)
  {
    Matrix[it->first] = pixelcolor;
  }
  else
  {
    Matrix[whichPixel] = pixelcolor;
  }
  return Matrix;
} // sparkle();

// plasma()
std::map<int, CRGB> plasma(std::map<int, CRGB> fg, boolean isTime = false)
{                                       // This is the heart of this program. Sure is short. . . and fast.
  int thisPhase = beatsin8(6, -64, 64); // Setting phase change for a couple of waves.
  int thatPhase = beatsin8(7, -64, 64);

  int k = 0;
  for (auto const &t : fg)
  { // make from the map and array int[lednumber]=realposition
    int colorIndex = cubicwave8((k * 23) + thisPhase) / 2 + cos8((k * 15) + thatPhase) / 2;
    int thisBright = qsuba(colorIndex, beatsin8(7, 0, 96));
    fg[t.first] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending);
    k++;
  }
  if ((Seconde & 1) == 0 && isTime)
  {
    fg[xyToLedNumber(15, 1)] = matrix[xyToLedNumber(15, 0)];
    fg[xyToLedNumber(16, 1)] = matrix[xyToLedNumber(16, 0)];
    fg[xyToLedNumber(15, 2)] = matrix[xyToLedNumber(15, 3)];
    fg[xyToLedNumber(16, 2)] = matrix[xyToLedNumber(16, 3)];

    fg[xyToLedNumber(15, 5)] = matrix[xyToLedNumber(15, 4)];
    fg[xyToLedNumber(15, 6)] = matrix[xyToLedNumber(16, 4)];
    fg[xyToLedNumber(16, 5)] = matrix[xyToLedNumber(15, 7)];
    fg[xyToLedNumber(16, 6)] = matrix[xyToLedNumber(16, 7)];
  }
  return fg;
} // plasma()

/*
 * Confetti animation
 */
std::map<int, CRGB> confetti(std::map<int, CRGB> Matrix)
{
  // random colored speckles that blink in and fade smoothly
  // fadeToBlackBy(leds, NUM_LEDS, thisfade);                    // Low values = slower fade.

  auto it = Matrix.begin();
  std::advance(it, random16() % Matrix.size()); // Pick an LED at random.
  CRGB col = it->second;
  Matrix[it->first] = col + CHSV((thishue + random16(huediff)) / 4, thissat, thisbri); // I use 12 bits for hue so that the hue increment isn't too quick.
  thishue = thishue + thisinc;                                                         // It increments here.
  return Matrix;
} // confetti()

void ChangeMe()
{                                              // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.
  uint8_t secondHand = (millis() / 1000) % 15; // IMPORTANT!!! Change '15' to a different value to change duration of the loop.
  static uint8_t lastSecond = 99;              // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand)
  { // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch (secondHand)
    {
    case 0:
      thisinc = 1;
      thishue = 192;
      thissat = 255;
      thisfade = 2;
      huediff = 256;
      break; // You can change values here, one at a time , or altogether.
    case 5:
      thisinc = 2;
      thishue = 128;
      thisfade = 8;
      huediff = 64;
      break;
    case 10:
      thisinc = 1;
      thishue = random16(255);
      thisfade = 1;
      huediff = 16;
      break; // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
    case 15:
      break; // Here's the matching 15 for the other one.
    }
  }
} // ChangeMe() for confetti

// This functions sets a single LED to a color based on RGB values..
void setPixel(int Pixel, byte red, byte green, byte blue)
{
  // FastLED
  matrix[Pixel].r = red;
  matrix[Pixel].g = green;
  matrix[Pixel].b = blue;
} // setPixel();

std::map<int, CRGB> dot_beat(std::map<int, CRGB> bg)
{
  std::map<int, CRGB>::iterator it;
  int numleds = bg.size();
  bpm = 12;
  uint8_t inner = beatsin8(bpm, numleds / 4, numleds / 4 * 3);  // Move 1/4 to 3/4
  uint8_t outer = beatsin8(bpm, 0, numleds - 1);                // Move entire length
  uint8_t middle = beatsin8(bpm, numleds / 3, numleds / 3 * 2); // Move 1/3 to 2/3

  it = bg.begin();
  std::advance(it, middle);
  bg[it->first] = CRGB::Purple;
  it = bg.begin();
  std::advance(it, inner);
  bg[it->first] = CRGB::Blue;
  it = bg.begin();
  std::advance(it, outer);
  bg[it->first] = CRGB::Aqua;

  for (auto const &t : bg)
  {
    CRGB item = t.second;
    bg[t.first] = item.nscale8(fadeval);
    // bg[t.first] = item.fadeToBlackBy( 1 );
  }
  // nscale8(leds, NUM_LEDS, fadeval);                           // Fade the entire array. Or for just a few LED's, use  nscale8(&matrix[2], 5, fadeval);
  return bg;
} // dot_beat()

std::map<int, CRGB> sinelon(std::map<int, CRGB> Matrix)
{
  uint8_t thisbeat = 23; // Beats per minute for first part of dot.
  uint8_t thatbeat = 28; // Combined the above with this one.
  uint8_t thisfade = 32; // How quickly does it fade? Lower = slower fade rate.
  uint8_t thissat = 255; // The saturation, where 255 = brilliant colours.
  uint8_t thisbri = 255; // Brightness of a sequence.

  std::map<int, CRGB>::iterator it;
  int numleds = Matrix.size(); // number of leds in this map

  it = Matrix.begin(); // iterator
  // int myhue = random8();

  int pos1 = beatsin16(thisbeat, 0, numleds);
  int pos2 = beatsin16(thatbeat, 0, numleds);
  int newpos = (pos1 + pos2) / 2;

  std::advance(it, newpos); // find this position in the Map
  CRGB item = it->second;   // current color on this map positon

  Matrix[it->first] = item + ColorFromPalette(currentPalette, myhue++, thisbri, currentBlending);
  return Matrix;
} // sinelon()

/* a function to just recolor the digits */
std::map<int, CRGB> recolorDigits(std::map<int, CRGB> Matrix, CRGB color)
{
  for (auto const &t : Matrix)
  {
    Matrix[t.first] = color;
  }
  return Matrix;
}

/* a function to show a text instead of just the time */
std::map<int, CRGB> showText(String text, CRGB dcolor = CRGB::Red, int scroller = 0)
{
  // step 1, how long is our String
  std::map<int, CRGB> textmap;
  std::map<String, int>::iterator it;

  int strlength = text.length();
  // step 2, calculate the x resolution based on 7 pixels per char on x axis
  int xres = strlength * 7;
  int position = 0 - scroller;

  for (int i = 0; i <= strlength; i++)
  {
    String letter = text.substring(i, i + 1);
    int letterposinarray = 0;
    if (letter == "i")
    {
      position = position - 2;
    }
    //  Serial.print("letter :"); Serial.println(letter);
    it = charToInt.find(letter);
    if (it != charToInt.end())
    {
      letterposinarray = it->second;
      textmap = makeDigits(letterMatrix[letterposinarray], textmap, position, 0, false, dcolor);
      position = position + 7;
    }
  }
  return textmap;
}

/*
    intput is the time (minutes) we need to check. output is if a random animation should run.
*/

boolean runRandomAnimation(int every)
{
  boolean runAnimation = false;

  Hour = timeinfo.tm_hour;   // get the hour as int.
  Minute = timeinfo.tm_min;  // get the minutes as int.
  Seconde = timeinfo.tm_sec; // get the seconds

  if (every == 60)
  {
    if (Minute == 0)
    {
      runAnimation = true;
    }
    else
    {
      runAnimation = false;
    }
  }
  else if (every == 30)
  {
    if (Minute == 0 || Minute == 30)
    {
      runAnimation = true;
    }
    else
    {
      runAnimation = false;
    }
  }
  else if (every == 15)
  {
    if (Minute == 0 || Minute == 15 || Minute == 30 || Minute == 45)
    {
      runAnimation = true;
    }
    else
    {
      runAnimation = false;
    }
  }
  else if (every == 1)
  {
    runAnimation = true;
  }
  else
  {
    runAnimation = false;
  }

  return runAnimation;
} // runRandomAnimation()
