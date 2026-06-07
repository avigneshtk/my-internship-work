#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "OPPO A16";
const char* password = "e5tm4gnc";

WebServer server(80);

const int ledPin = 2;
const int pwmChannel = 0;
const int pwmFreq = 5000;
const int pwmResolution = 8;

String webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Brightness Control</title>
<style>
body{
    font-family: Arial;
    text-align:center;
    margin-top:50px;
    background:#111;
    color:white;
}
.slider{
    width:80%;
}
</style>
</head>
<body>

<h1>ESP32 LED Brightness</h1>

<input type="range" min="0" max="255" value="0"
class="slider" id="brightness">

<p>Brightness: <span id="value">0</span></p>

<script>
var slider = document.getElementById("brightness");
var output = document.getElementById("value");

slider.oninput = function() {
  output.innerHTML = this.value;

  fetch("/set?value=" + this.value);
}
</script>

</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleBrightness() {
  if(server.hasArg("value")) {
    int brightness = server.arg("value").toInt();
    ledcWrite(pwmChannel, brightness);
    server.send(200, "text/plain", "OK");
  }
}

void setup() {
  Serial.begin(115200);

  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(ledPin, pwmChannel);

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleBrightness);

  server.begin();
}

void loop() {
  server.handleClient();
}