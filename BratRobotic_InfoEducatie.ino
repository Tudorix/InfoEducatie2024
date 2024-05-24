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
int mainRotPos = 90, secondRotPos = 90, turnPos = 90;
int aut = 1;

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

.buttonLast{
  width: 80%;
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
      <input type="range" min="0" max="180" value="90" onchange="updateSlider(1, this.value)" class="slider">
      <p>Ridicare Brat 1</p>
      <input type="range" min="0" max="140" value="90" onchange="updateSlider(2, this.value)" class="slider">
      <p>Ridicare Brat 2</p>
      <input type="range" min="43" max="160" value="90" onchange="updateSlider(3, this.value)" class="slider">
      <p>Rotire Gheara</p>
      <input type="range" min="0" max="180" value="90" onchange="updateSlider(4, this.value)" class="slider">
      <br>

      <button onclick="sendRequest(1)" class="button">Deschide</button>
      <button onclick="sendRequest(2)" class="button">Inchide</button>
      <button onclick="sendRequest(3)" class="buttonLast">Autonom</button>

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
        if(aut == 1) claw.write(0); // CLOSE
        break;
      case 2:
        if(aut == 1) claw.write(180); // OPEN
        break;
      case 3:
        autonomous();
        break;
      default:
        // Handle invalid button ID
        break;
    }
  }
}

void smoothMove(Servo &servo, int start, int end, int steps) {
  float increment = (end - start) / float(steps);
  for (int i = 0; i <= steps; i++) {
    servo.write(start + i * increment);
    delay(15);
  }
}

void handleSlider() {
  if (server.hasArg("id") && server.hasArg("value") && aut == 1) {
    int sliderId = server.arg("id").toInt();
    int sliderValue = server.arg("value").toInt();
    
    Serial.print("Slider ID: ");
    Serial.print(sliderId);
    Serial.print(" Value: ");
    Serial.println(sliderValue);

    switch(sliderId){
      case 1: smoothMove(servoMainRotation, turnPos, sliderValue, 40); turnPos = sliderValue; break;
      case 4: servoMainVertical.write(sliderValue); break;
      case 3: smoothMove(servoSecondVertical, secondRotPos, sliderValue, 50); secondRotPos = sliderValue; break;
      case 2: smoothMove(servoSecondRotation, mainRotPos, sliderValue, 50); mainRotPos = sliderValue; break;
      default: Serial.print("No such slider.");break;
    }
  }
}

void moveToPos(int id, int target){
    switch(id){
      case 1: smoothMove(servoMainRotation, turnPos, target, 40); turnPos = target; break;
      case 4: servoMainVertical.write(target); break;
      case 3: smoothMove(servoSecondVertical, secondRotPos, target, 50); secondRotPos = target; break;
      case 2: smoothMove(servoSecondRotation, mainRotPos, target, 50); mainRotPos = target; break;
      case 5: claw.write(target);
      default: Serial.print("No.");break;
    }
}

void handleRoot() {
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void moveServosToPosition(int base , int rot1, int rot2, int rot3){
  servoMainRotation.write(base);
  servoMainVertical.write(rot1);
  servoSecondVertical.write(rot2);
  servoSecondRotation.write(rot3);
}


void autonomous(){// 0 == open
  moveToPos(5, 0);
  moveToPos(1,90);
  moveToPos(4 , 90);
  moveToPos(2 , 90);
  moveToPos(3 , 90);
  delay(50);

  moveToPos(1,100);
  delay(15);
  moveToPos(4 , 180);
  delay(15);
  moveToPos(2 , 0);
  delay(15);
  moveToPos(3 ,75);
  delay(15);
  moveToPos(5 , 180);
  delay(50);
  moveToPos(2, 70);
  delay(15);
  moveToPos(3,125);
  delay(15);
  moveToPos(2, 90);
  delay(100);
  moveToPos(4,0);
  delay(500);
  smoothMove(servoSecondRotation, mainRotPos, 130, 80);
  delay(100);
  moveToPos(5,0);
  delay(500);
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
  servoMainRotation.attach(D0, 500, 2500);
  servoSecondRotation.attach(D1, 500, 2500);
  servoMainVertical.attach(D3, 500, 2500);
  servoSecondVertical.attach(D4, 500, 2500);
  claw.attach(D2, 500, 2500);
}
int val;

void loop() {
  server.handleClient();
}
