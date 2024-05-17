#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Servo.h>

const char* ssid = "Brat Robotic";
const char* password = "haraMBE2019";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

Servo servoMainRotation , servoSecondRotation, servoMainVertical, servoSecondVertical, claw;

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<script>
      function updateSlider(sliderId, value) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/slider?id=" + sliderId + "&value=" + value, true);
        xhr.send();
      }

      function sendRequest(buttonId) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/button?id=" + buttonId, true);
        xhr.send();
      }
</script>

<style>

h1{
  font-weight: 400;
  font-size: 800%;
  font-style: normal;
  height: 150px;
  background-color: orange;
  margin: 0;
  padding-top: 40px;
}

body{
  margin: 0;
  background-color: gray;
}

.slider {
  -webkit-appearance: none;
  width: 80%;
  height: 80px;
  background: orange;
  outline: none;
  opacity: 0.7;
  -webkit-transition: .2s;
  transition: opacity .2s;
}

.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 80px;
  height: 80px;
  background: black;
  cursor: pointer;
}

.button{
  width: 40%;
  height: 250px;
  font-weight: 400;
  font-size: 500%;
  background: orange;
  border: 0;
  margin-top: 100px;
  margin-left: 5px;
  margin-right: 5px;
  margin-bottom: 50px;
}

p{
  margin-topȘ 50px;
  font-weight: 400;
  font-size: 600%;
}



</style>

<body>
  <center>
      <h1>Brat Robotic</h1><br>
      <p>Rotire Baza</p>
      <input type="range" min="0" max="100" value="50" onchange="updateSlider(1, this.value)" class="slider">
      <p>Ridicare Brat 1</p>
      <input type="range" min="0" max="100" value="50" onchange="updateSlider(2, this.value)" class="slider">
      <p>Ridicare Brat 2</p>
      <input type="range" min="0" max="100" value="50" onchange="updateSlider(3, this.value)" class="slider">
      <p>Rotire Ghiara</p>
      <input type="range" min="0" max="100" value="50" onchange="updateSlider(4, this.value)" class="slider">
      <br>

      <button onclick="sendRequest(1)" class="button">Deschide</button>
      <button onclick="sendRequest(2)" class="button">Inchide</button>

  </center>

</body>
</html>
)=====";

int MainRotation , MainVertical, SecondVertical, SecondRotation;

void handleButton() {
  if (server.hasArg("id")) {
    int buttonId = server.arg("id").toInt();
    Serial.print("Button ID: ");
    Serial.println(buttonId);
    
    // Perform action based on buttonId here
    switch (buttonId) {
      case 1:
        claw.write(0); // CLOSE
        break;
      case 2:
        claw.write(180); // OPEN
        break;
      default:
        // Handle invalid button ID
        break;
    }
  }
}

void handleSlider() {
  if (server.hasArg("id") && server.hasArg("value")) {
    int sliderId = server.arg("id").toInt();
    int sliderValue = server.arg("value").toInt();
    
    Serial.print("Slider ID: ");
    Serial.print(sliderId);
    Serial.print(" Value: ");
    Serial.println(sliderValue);

    switch(sliderId){
      case 1: servoMainRotation.write(sliderValue); break;
      case 2: servoMainVertical.write(sliderValue); break;
      case 3: servoSecondVertical.write(sliderValue); break;
      case 4: servoSecondRotation.write(sliderValue); break;
      default: Serial.print("No such slider.");break;
    }
  }
}

void handleRoot() {
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void setup(){
  Serial.begin(9600);
  
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet); 
 
  server.on("/", handleRoot);  
  server.on("/button", handleButton);
  server.on("/slider", handleSlider);
  server.begin();                 
  Serial.println("HTTP server started");
  servoMainRotation.attach(D0);
  servoSecondRotation.attach(D1);
  servoMainVertical.attach(D2);
  servoSecondVertical.attach(D3);
  claw.attach(D4);
}
int val;

void loop() {
  server.handleClient(); 
  
}
