const char INDEXAP_HTML[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML>
  <html>
  <head>
  <meta content="text/html; charset=ISO-8859-1" http-equiv="content-type">
  <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
  <title>ESP32 Led Matrix Clock, update your, WIFI SETTINGS</title>
  <style>
  "body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; text-align:center;}"
  </style>
  </head>
  <body>
  <h3>Enter your WiFi credentials</h3>
  <form action="/get" target="hidden-form">
  <p>
  <label>SSID:&nbsp;</label>
  <input maxlength="30" name="SSIDname"><br>
  <input type="submit" value="Save">
  </form>
  <form action="/get" target="hidden-form">
  <label>Key: </label><input maxlength="30" name="SSIDpwd"><br>
  <input type="submit" value="Save">
  </p>
  </form>
  <form action="/get" target="hidden-form">
    Press this button to reboot with the new Password and SSID name<br />
    <input type="submit" value="Reboot">
    <br />
    </form>
  <p></p>
  <iframe style="display:none" name="hidden-form"></iframe>
  </body>
  </html>
  )rawliteral";

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


/* Style the tab */
.tab {
  overflow: hidden;
  border: 1px solid #ccc;
  background-color: #f1f1f1;
}

/* Style the buttons that are used to open the tab content */
.tab button {
  background-color: inherit;
  float: left;
  border: none;
  outline: none;
  cursor: pointer;
  padding: 14px 16px;
  transition: 0.3s;
}

/* Change background color of buttons on hover */
.tab button:hover {
  background-color: #ddd;
}

/* Create an active/current tablink class */
.tab button.active {
  background-color: #ccc;
}

/* Style the tab content */
.tabcontent {
  display: none;
  padding: 6px 12px;
  border: 1px solid #ccc;
  border-top: none;
}
</style>

  <title>Led Matrix Clock</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Restarting ESP");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
    function openCity(evt, cityName) {
  // Declare all variables
  var i, tabcontent, tablinks;

  // Get all elements with class="tabcontent" and hide them
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }

  // Get all elements with class="tablinks" and remove the class "active"
  tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(" active", "");
  }

  // Show the current tab, and add an "active" class to the button that opened the tab
  document.getElementById(cityName).style.display = "block";
  evt.currentTarget.className += " active";
}
  </script></head><body>

  <div class="tab">
    <button class="tablinks" onclick="openCity(event, 'Animation')" id="defaultOpen">>Animation</button>
    <button class="tablinks" onclick="openCity(event, 'Brightness')">Brightness</button>
    <button class="tablinks" onclick="openCity(event, 'Sleepmode')">Sleepmode</button>
     <button class="tablinks" onclick="openCity(event, 'Other')">Other</button>
  </div>  
  <!-- Tab content -->
  
  <div id="Animation" class="tabcontent">
    <h3>London</h3>
     <form action="/get" target="hidden-form">
    Choose your background animation. The current effect: - %effect% <br />
      <select name="effect" id="effecten" onchange="this.form.submit()">
        <option value = "%effect%" selected> %effect% </option>
        <option value = "oneColorBackground">Fixed Background </option>
        <option value = "sparkle">Sparkle</option>
        <option value = "confetti">Confetti</option>
        <option value = "plasma">Plasma</option>
        <option value = "ripples">Ripples</option>
        <option value = "rainbowWaves">Rainbow Waves</option>
        <option value = "dotsBeat">Dots</option>
        <option value = "showText">Show your Text</option>
        <option value = "showWeather">Show the Weather</option>
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
  </div>

  <div id="Brightness" class="tabcontent">
    <h3>Brightness</h3>
     <form action="/get" target="hidden-form">
     <p> Overall Brightness of the Matrix <br />
     <div class="slidecontainer">
        <input name="overAllBrightness" type="range" min="1" max="255" value="%overAllBrightness%" class="slider" id="overAllBrightness"  onchange="this.form.submit()">
        Value: %overAllBrightness% 
      </div>
     </form>
     </p>

     
     <p> If your matrix is equiped with a Light Dependent resistor, it can automatically dim or brigthen the led's based on the ambient light<br />
     The current ambient strength of the light is (measured between 0 min to 4095 max): <br />
     Current ambient light level: %LDR% <br/>
    

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
   
  </div>

<div id="Sleepmode" class="tabcontent">
  <h3>Sleepmode</h3>
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
      <form action="/get" target="hidden-form">
     <p> Text Scroll speed (needs a reboot after you set a new speed): <br />
     <div class="slidecontainer">
        <input name="scrollspeed" type="range" min="1" max="255" value="%scrollspeed%" class="slider" id="digitBrightness"  onchange="this.form.submit()">
        Value: %scrollspeed% 
      </div>
     </form>
     </p>
     <form action="/get" target="hidden-form">
 You can click this button to restart the Clock. <br />
  <input type="submit" name="restart" value="Restart clock" onclick="submitMessage()">
  <br />
  </form>
  <br />
</div>
  
  <div id="Other" class="tabcontent">
  <h3> Other </h3>
   <h3> Weather data </h3>
  The clock can also show weather data. But it needs some settings for this. <br/>
  You need a openweathermap account (https://openweathermap.org). From your account you need to go to My API keys (https://home.openweathermap.org/api_keys) and create a new key. <br/>
  This key you need to enter here:
   <form action="/get" target="hidden-form">
      API key: <br />
        <input type="text" id="apikey" name="apikey" value ="%apikey%">
        <input type="submit" value="Submit" >
      </form> <br />
      We also need your location. Remember only use large city names in your local area. Small towns are likely not in the API. <br />
      <form action="/get" target="hidden-form">
      Town: <br />
        <input type="text" id="city" name="city" value ="%city%">
        <input type="submit" value="Submit" >
      </form> <br />
   </div>   
  
    <iframe style="display:none" name="hidden-form"></iframe>
    <br /> Made by Bas van Breukelen (c) 2021 
  <br /> This matrix has rebooted: %iRebooted% times since first installing Led-Clock <br />
</body>
<script>
// Get the element with id="defaultOpen" and click on it
document.getElementById("defaultOpen").click();
</script>

</html>)rawliteral";

// Webpage that is served when ESP is in case accessmode:
const char index2_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Matrix Clock, Accesspoint... Setup mode</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Restarting device");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
  <form action="/get" target="hidden-form">
    What is the name of your WIFI station (SSID): ( %inputWifi% ) <br /> 
    Press submit after entering the SSID. After this, enter your WIFI credentials (password or key).<br />
    <input type="text " name="inputWifi" value="%inputWifi%">
    <input type="submit" value="Submit" >
  </form><br>
  <form action="/get" target="hidden-form">
   What is your WIFI networks password (or key) <br />
    <input type="password " name="inputPWD" value="%inputPWD%">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get" target="hidden-form">
  After provinding a SSID name and Password, you can click this button to restart the clock. <br />
  <input type="submit" value="Restart clock" onclick="submitMessage()">
  <br />
  </form>
  </div>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";
