<img src="https://github.com/bvanbreukelen/ESP32-Led-Matrix-Clock/blob/main/pictures/IMG_6880.jpeg?raw=true" width="250"><img src="https://github.com/bvanbreukelen/ESP32-Led-Matrix-Clock/blob/main/pictures/IMG_6878.jpeg?raw=true" width="250">
[![](http://img.youtube.com/vi/N7SGHlUMIps/0.jpg)](http://www.youtube.com/watch?v=N7SGHlUMIps "")

# ESP32-Led-Matrix-Clock. VERSION 2.0 since March 16-2021
A led matrix clock ran on an ESP32 using FastLED and a led matrix

# How I got to this project
Based on the LED projects by Bitluni (https://bitluni.net) I got excited to use WS2812 (or alike) LED's to create something fun.
After creating my LED Pingpong Ball light I wanted something else. So I stumbled on the LED clock video https://www.youtube.com/watch?v=7urN2OS0fjs by Thomas https://www.instructables.com/Ping-Pong-Ball-LED-Clock/ and https://www.hackster.io/news/improved-led-ping-pong-ball-clock-232a0ac06643
I loved these creations. But... (always a but), this is a BIG clock, a bit too much for my livingroom. Also these clocks run on an arduino and I wanted to use an ESP32.
# Why an ESP32? 
In this project I used: Wemos Mini D1 EPS32 and the ESP32 Devkit V1 both work ). https://www.amazon.nl/ESP-32S-Wireless-Bluetooth-Esp8266-CP2102/dp/B07NW8S1ZR
Because it has built-in WIFI, Bluetooth, DualCore, RTC and much much more. Oh, and a lot of program memory.

# Leds (WS2812 8x32 LED matrix)
Also, I was looking into alternatives to ping-pong balls and LED-strips with smaller LED spacings. The ones used in the projects above are all 30 LEDS per meter. Luckely there are also LED strips with 60 and 144 LED's per meter. Downside, the pingpong balls wont fit. And while searching for these strips my eye caught an alternative, a WS2812 LED MATRIX, available in the sizes 8x8 (too small for this project, unless you buy 4), 16x16 (yes nice) and an 8x32 (great!!) version. I bought the latter. I bought mine at (https://www.tinytronics.nl/shop/en/lighting/led-digital/ws2812b-digitale-5050-rgb-led-matrix-32x8-flexible) but you can get it a lot cheaper on one of the chinese stores.  Please note. This project doesn't depend on using this specific matrix. You can create your own, or buy for example 4 8x8 LED matrices and even any other addressable LED can be used, as long as it is supported by the FastLED library (http://fastled.io).

<img src="https://github.com/bvanbreukelen/ESP32-Led-Matrix-Clock/blob/main/pictures/IMG_6789.jpeg?raw=true" width="250">

# 3D printer files, equipment and settings.
As ping pong balls don't fit and as I also just bought a 3D printer, I really wanted to make use of that. After a crash course in fusion 360 design, I created an enclosure for the matrix and ESP32. An OMG, It looks great!! The Fusion 360 files (or printable obj files) will be shared here as well. For you to use and change in anyway you like. Please know, I'm not a fusion 360 specialist, by far, so non parametric, ugly sketches and a lot of .. how did I do this moments, went into designing. 
The enclosure was printed using Black PLA and the LED diffusor using Neutral (almost transparent) PLA. (neutral: https://www.123-3d.nl/PLA/1-75-mm-PLA/Neutraal-p7318.html, black: https://www.123-3d.nl/PLA/1-75-mm-PLA/Zwart-p7316.html).  
Everything was printed on a Creality Ender 3v2: https://www.123-3d.nl/3D-printers/Creality-3D-Ender-3-V2-p16058.html accomodated with a BL-touch. (Using the superb modded Marlin firmware provided by Smith3D : https://smith3d.com/ender-3-v2-bltouch-firmware-installation-guide-by-smith3d-com/)
All fusion 360 files were sliced using CURA 4.7.  As the enclosure is a little bit bigger then the printbed (32 cm vs 22.5 cm) I designed it such that it is printed in 2 pieces and glued together afterwards. The same for the diffusor. Everything was printed at standard quality (0.4 nozzle and 0.2 layer height) except for the stands, these were printed at 0.28 layer height, which didn't affect the quality because of their simplicity. 

<img src="https://github.com/bvanbreukelen/ESP32-Led-Matrix-Clock/blob/main/pictures/IMG_6873.jpeg?raw=true" width="250">

# Coding and stuff
First of all, I'm not a software engineer. I did a lot of programming in the past, mostly in [R], Java, PHP and Perl. I'm a bioinformatician, who mostly does biological data analysis using scripts and a bit of ugly code. Don't expect a neatly programmed project. I tried to put comments in the code and splitted the Arduino.ino files into seperate files, each with their own 'focus'. So no libraries. In the end the arduino environment will concatenate all files into one big file and compile it. 
I used a lot of global variables. Some lineair thinking and ugly if loops. And when possible put repeated code in functions. Those are my design choices. Not the best, but they get the job done. I invite you, if you like this project, to make it better, faster and more efficient!!! 
## It is open source, but please if you make any improvements or create your own version in the end, please acknowledge me and share the code with me. 
- Use the Arduino programming environment and make sure to Add the ESP32 to your libraries and boards, see tutorial: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
- You can import my sketch in this environment. If correct, all files should be in there!! Don't change the file names (and yes, I used numbering and some numbers my be missing, this is deliberary a I might want to add stuff later on..) The order is important!! All files should be in TABS in the arduino project onces it is opened. 
- I have used a lot of libraties: Make sure you have them all installed in the arduino environment:
#include <FastLED.h> // https://github.com/FastLED/FastLED
#include <WiFi.h> //already in arduino after installing the ESP32 environment
#include "time.h" //already in arduino 
#include <String> //already in arduino after installing the ESP32 environment
#include <AsyncTCP.h> // https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h> // https://github.com/me-no-dev/ESPAsyncWebServer
#include "soc/soc.h" //already in arduino after installing the ESP32 environment
#include "soc/rtc_cntl_reg.h" //already in arduino after installing the ESP32 environmen
#include <Preferences.h>  // works much better than spiffs...to store parameters persistently during reboots.
  
Issues I have encountered on my Mac (OSX 11.2.1) when trying to compile in arduino are...:
- Almost after every OSX update I get error messages that compiling is not possible: Run this in the terminal --> sudo xcode-select --install
- Not seeing the ESP32 on the serial port: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers install the correct UART drivers (2102)
- Some ESP32 (not the mini I used) have two buttons, one (boot) must be pressed when uploading your compiled sketch! The ESP32 wemos mini I used does this automatically.
- This guide: https://randomnerdtutorials.com/esp32-troubleshooting-guide/ helped me basically with all issues.

Important:
The first .ino file (called LEdMatrixESP2) contains all globals and settings for the LED types used, amount of LED's, the max current and voltage used, and on which ESP32 pin(s) the LED's are connected. Make sure everything is correct. 
More over the ledArray[XRES * YRES] is THE place to configure your matrix. It basically tells which LED number is on which X,Y coordinate. As this may differ between led matrices.  Mine for example was a up/down/down/up/up...etc configuration. This part is some hand-work. For sure somebody may have a smarter and more mathematical approach. Well this works for me.   The numberMatix stores the number characters.. Which in this case are actually 6x6 pixel characters in a 8x8 array (64). You can create your own font if you like, but just changing the 0 and 1 at specific positions. 
Also in this file you find the HTML pages that are served to configure the clock once it is up and running.

#So what does it do (or what did I implement).
1. After uploading the compiled sketch to the ESP32 or after booting up/restart..
2. The ESP32 will power-up. And after a few seconds on the top left a LED will turn on. And under that light more leds will turn. One more for every time that the ESP will try to connect to (a) your wifi router. If this after 5 tries (~15 seconds) does not work, the ESP32 will boot up as ACCESSPOINT (see 2.1)
2.1 Depending on the first time boot, there will be no WIFI (SSID,PWD) configured. So the ESP32 will start as an accesspoint. You will see it appear as: ESP32-LedMatrixClock and the password is 1234abcd.  Logon to this wifi and open the browser 192.168.4.1 will probably the IP address of the ESP32. (If this doesn't work, connect to the serial monitor, the IP-address will be shown there as wel). On this webpage, enter the station SSID (you will not see a list, so know you names), press submit. Next enter your station password and press submit. Finally press the reboot button. If you entered everyting correctly. The ESP32 can now connect to your own WIfI router.
2.2. If connected to your wifi station (router). The LED on the matrix will turn green. 
3. The ESP32 will display a 1 or 2 or 3 digit number. Those are the last 3 numbers of it's IP address. Most often the IP address will be 192.168.1.xxx (where xxx is the number on the matrix). In case you have a very different router DNS config, you should connect your clock to the serial monitor, and there the full IP address will be shown.  
4. A 3rd LED will light-up orange, this is the ESP32 tryiing to get the time from the NTP server: europe.pool.ntp.org (change NTP server if need be in the sketch).
Sometimes the NTP server doesn't respond. The ESP32 will try 5 times (~15-20 seconds) before it decides to give up and reboot. A reboot often does the trick. Also have a look at the NTP server settings.. it is now set to the NL timezone with Daylight Saving enabled.
5. If network time is configured the time will be shown on the matrix. 
6. Now you can surf to the ESP's ip address and play with the configuration, change animations, LED colors, set a night mode, or choose for a random animation to be shown at specific intervalls. 

# Work in progress: 
There are so many things I want to do. But for now this is my action list.

- DONE: Configure colors background and letters.
- DONE: Choose an animation for the background or digits.
- DONE: Set a text on the website and display it on the matrix.		
- DONE: Random animation at a given time intervall op 60, 30, 15, 1 minutes.
- DONE: Nightmode with red letters (dim) and no background. 
- DONE: Sleep and wakeup times for nightmode.. (limited to specific times for now)
- DONE: Option to globally dim the display, and seperate dimming options for characters and background.
- ONGOIN: More animation
- DONE: Access Point mode to configure WIFI 
- DONE: Show IP address on bootup
- Characters can now only be placed based on X position, not Y position yet.
- DONE: Add photo's to this page
- IMPLEMENTED: a more simple method to animate background and foreground without them inteferring..(most of the time).

-- ISSUE (or not): When background is fade-down, some animations will not look as good. As a fix some background cannot be faded (even if you set it to fade) the only dimming that will always work is the overall brightness (dimming). 
-- The code for this version 2.0 is way more structured. But still at some points spaghetti.. but its faster and easier to read (more comments inline) and better use of functions().

# Wires and schema's
<img src="https://github.com/bvanbreukelen/ESP32-Led-Matrix-Clock/blob/main/pictures/ESP-schema.png?raw=true" width="400">
In the (bad) photo you see how I soldered everyting on a soldering breadboard. (5cm x 7 cm). 
On the ESP32 I soldered pins on the ground, Pin2 and 5V places. And on the right side row two pins, one on the top and one on the bottom of the outer rows. Note: Pin2 was used in version 1, for version 2 I used pin 13. You can still use 2 of any other pin, just set it correct in the code!! #define DATA_PIN .. (where .. is your data pin number)
((Pins, also called male Pin Header, <img src="https://github.com/bvanbreukelen/ESP32-Led-Matrix-Clock/blob/main/pictures/Screenshot%202021-02-11%20at%2015.35.48.png" width="100"> you can break of seperate pins ;
This makes a square of pins. In this way every corner connects to the sockets on the breadboard and this makes it more stable.
On the soldering breadbord I soldered the sockets/connectors. This makes the ESP32 plug and play in case you want to add, change stuff.
I used JST XH connectors to allow easy plugin and removal of the connections to the powersupply and the matrix.
Under the ESP, I soldered the 62 Ohm resistor, this makes the data signal cleaner. I used https://github.com/Aircoookie/WLED/wiki this schema as reference.
Also a 1000uF capacitor (rated for 5V-12V and 50Watt) was used to catch possible power fluctuations. This can happen when some funcions, or effects on the matrix draw for a short time more power. This prevent so called Brownouts on the ESP32, causing it to reboot.
With 256 LED's each consuming a max of 40 mili-Amps (fully on white), you need 10 Amps at 5V (50Watt). In the code I limited to 3 Amps. This seems to be sufficient for what this clock does. It is still bright and works like a charm.
I use powerinjection.. so prevent to not feed the Leds via the ESP32, but just like I did in this schema. The Matrix that I have has 3 connectors, I powerinject over every connector to prevent voltagedrops and hence bad color representation.


Have fun....













