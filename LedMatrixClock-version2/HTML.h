const char INDEXAP_HTML[] =
  "<!DOCTYPE HTML>"
  "<html>"
  "<head>"
  "<meta content=\"text/html; charset=ISO-8859-1\""
  " http-equiv=\"content-type\">"
  "<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
  "<title>ESP8266 Web Form Demo</title>"
  "<style>"
  "\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; text-align:center;}\""
  "</style>"
  "</head>"
  "<body>"
  "<h3>Enter your WiFi credentials</h3>"
  "<form action=\"/get\" target=\"hidden-form\">"
  "<p>"
  "<label>SSID:&nbsp;</label>"
  "<input maxlength=\"30\" name=\"SSIDname\"><br>"
  "<input type=\"submit\" value=\"Save\">"
  "</form>"
  "<form action=\"/get\" target=\"hidden-form\">"
  "<label>Key:&nbsp;&nbsp;&nbsp;&nbsp;</label><input maxlength=\"30\" name=\"SSIDpwd\"><br>"
  "<input type=\"submit\" value=\"Save\">"
  "</p>"
  "</form>"
  "<form action=\"/get\" target=\"hidden-form\">"
  "  Press this button to reboot with the new Password and SSID name<br />"
  "  <input type=\"submit\" value=\"Reboot\">"
  "  <br />"
  "  </form>"
  "<p> %wifilist% </p>"
  "<iframe style=\"display:none\" name=\"hidden-form\"></iframe>"
  "</body>"
  "</html>";

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
<style>
.slider {
  -webkit-appearance: none;
  width: auto;
  height: 15px;
  border-radius: 5px;   
  background: #d3d3d3;
  outline: none;
  opacity: 0.7;
  -webkit-transition: .2s;
  transition: opacity .2s;
}

.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 25px;
  height: 25px;
  border-radius: 5px; 
  background: #4CAF50;
  cursor: pointer;
}

.slider::-moz-range-thumb {
  width: 25px;
  height: 25px;
  border-radius: 5px;
  background: #4CAF50;
  cursor: pointer;
}
</style>

  <title>ESP32 Led Matrix Clock</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
  <form action="/get" target="hidden-form">
    Choose your background animation. The current effect: - %effect% <br />
      <select name="effect" id="effecten" onchange="this.form.submit()">
        <option value="%effect%" selected> %effect% </option>
        <option value="oneColorBackground">Fixed Background </option>
        <option value="rainbowCycle">Rainbow Background</option>
        <option value="sparkle">Sparkle</option>
        <option value = "confetti">Confetti</option>
        <option value = "plasma">Plasma</option>
        <option value = "ripples">Ripples</option>
        <option value = "rainbowWaves">Rainbow Waves</option>
        <option value = "dotsBeat">Dots</option>
        <option value = "showText">ShowText</option>
        <option value = "Sweep">Sweep</option>
        <option value = "test">test</option>
      </select>
  </form><br />

  <form action = "/get" target="hidden-form">
  Random animation and interval. This will use Fixed Background when no animation is shown. <br />
  The animation will play for 1 minute before reverting back to default :<br />
  <select name="randomAnimation" id="randomAnimation" onchange="this.form.submit()">
    <option value="%randomAnimation%" selected> %randomAnimation% </option>
    <option value="No random animation">No random animation</option>
    <option value="Random every 60 minutes">Random every 60 minutes</option>
    <option value="Random every 30 minutes">Random every 30 minutes</option>
    <option value="Random every 15 minutes">Random every 15 minutes</option>
    <option value="Random every 1 minute">Random every 1 minute</option>
    </select>
  </form><br />

  <form action = "/get" target="hidden-form">
    Random animation includes, digits, background or both?:
    <select name="randomWhat" id="randomWhat" onchange="this.form.submit()">
      <option value="%randomWhat%" selected> %randomWhat% </option>
      <option value="Digits">Digits</option>
      <option value="Background">Background</option>
      <option value="Both">Both</option>
     </select>
  </form><br />
  

  <form action="/get" target="hidden-form">
    Clock digit animation:  <br />
    <select name="digitOnOff" id="%digitOnOff%" onchange="this.form.submit()">
    <option value="%digitOnOff%" selected> %digitOnOff% </option>
    <option value="No animation">No Animation</option>
    <option value="Dark digits">Dark Digits </option>
    <option value="Rainbow">Rainbow</option>
    <option value="Sparkle">Sparkle</option>
    <option value="Plasma">Plasma</option>
    <option value="Sweep">Sweep</option>
    </select>
  </form><br />
  
  <form action="/get" target="hidden-form">
    <label for="colorpicker1">Digit color: </label> <br />
    <input type="color" id="colorpicker1" name="inputDigitCol" value ="%digitHTMLcol%">
    <input type="submit" value="Submit" >
   </form><br />
   
    <form action="/get" target="hidden-form">
    <label for="colorpicker2">Background color: </label> <br />
    <input type="color" id="colorpicker2" name="inputBGCol" value ="%BGHTMLcol%">
    <input type="submit" value="Submit">
   </form><br />


     <form action="/get" target="hidden-form">
     <p> Overall Brightness of the Matrix <br />
     <div class="slidecontainer">
        <input name="overAllBrightness" type="range" min="1" max="255" value="%overAllBrightness%" class="slider" id="overAllBrightness"  onchange="this.form.submit()">
        Value: %overAllBrightness% 
      </div>
     </form>
     </p>

      <form action="/get" target="hidden-form">
     <p> Background Brightness (does not apply to all animations) <br />
     <div class="slidecontainer">
        <input name="backgroundBrightness" type="range" min="1" max="255" value="%backgroundBrightness%" class="slider" id="backgroundBrightness"  onchange="this.form.submit()">
        Value: %backgroundBrightness%
      </div>
     </form>
     </p>

      <form action="/get" target="hidden-form">
     <p> Digits Brightness (does not apply to all animations) <br />
     <div class="slidecontainer">
        <input name="digitBrightness" type="range" min="1" max="255" value="%digitBrightness%" class="slider" id="digitBrightness"  onchange="this.form.submit()">
        Value: %digitBrightness% 
      </div>
     </form>
     </p>
   
   <form action="/get" target="hidden-form">
   Manual nightmode:<br />
    <select name="nightMode" id="nachtstand" onchange="this.form.submit()">
      <option value="%nightMode%" selected> %nightMode% </option>
        <option value="On">On </option>
        <option value="Off">Off </option>
    </select>
    </form>
<br />
The nightmode can also be set to start and end between certain times.
<form action="/get" target="hidden-form">
   Start nightmode at: <br />
    <select name="nightTimeHour" id="nightTimeHour" onchange="this.form.submit()">
      <option value="%nightTimeHour%" selected> %nightTimeHour% </option>
        <option value="19">19</option>
        <option value="20">20</option>
        <option value="21">21</option>
        <option value="22">22</option>
        <option value="23">23</option>  
    </select> hours and 
     </form>
    <form action="/get" target="hidden-form">
    Minute: 
    <select name="nightTimeMinute" id="nightTimeMinute" onchange="this.form.submit()">
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
    </form>
   <form action="/get" target="hidden-form">
   Stop nighmode at: 
    <select name="morningTimeHour" id="morningTimeHour" onchange="this.form.submit()">
      <option value="%morningTimeHour%" selected> %morningTimeHour% </option>
        <option value="5">5</option>
        <option value="6">6</option>
        <option value="7">7</option>
        <option value="8">8</option>
        <option value="9">9</option>
        <option value="10">10</option>
        <option value="11">11</option>
     </select> hours and
     </form>
    <form action="/get" target="hidden-form">
   Minute: 
    <select name="morningTimeMinute" id="morningTimeMinute" onchange="this.form.submit()">
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
     </form>
     <br />Between %nightTimeHour%:%nightTimeMinute% in the evening and %morningTimeHour%:%morningTimeMinute%  in the morning<br />
     the clock will be in nightmode. <br />
      <br />
      <form action="/get" target="hidden-form">
      Put a message here, this can be displayed: <br />
        <input type="text" id="scrolltext" name="scrolltext" value ="%scrolltext%" maxlength="20">
        <input type="submit" value="Submit" >
      </form>
    <iframe style="display:none" name="hidden-form"></iframe>
    <br /> Made by Bas van Breukelen (c) 2021 
  <br /> This matrix has rebooted: %iRebooted% times since first installing Led-Clock <br />
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
  </form>
  
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";
