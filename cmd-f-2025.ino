#include <ESPmDNS.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <Stepper.h>

const int trigPin = 5;
const int echoPin = 18;
#define SOUND_SPEED 0.034
#define DISTANCE_THRESHOLD 7.0

long duration;
float distanceCm;

const int stepsPerRevolution = 2048;
#define IN1 19
#define IN2 21
#define IN3 15
#define IN4 2
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

const char* ssid = "ORBI44"; 
const char* password = "roundcarrot897"; 
AsyncWebServer server(80);

String readDistance() {
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distanceCm = duration * SOUND_SPEED / 2;
    Serial.println("Distance:");
    Serial.println(distanceCm);
    return String(distanceCm);
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css">
    <style>
      html {
          font-family: Arial;
          display: inline-block;
          margin: 0px auto;
          text-align: center;
      }
      h2 { font-size: 3.0rem; }
      p { font-size: 3.0rem; }
      .units { font-size: 1.2rem; }
      .dht-labels {
          font-size: 1.5rem;
          vertical-align: middle;
          padding-bottom: 15px;
      }
    </style>
</head>
<body>
    <h2>Ultrasonic sensor reading on server</h2>
    <p>
        <i class="fa fa-road" style="color:#00add6;"></i>
        <span class="dht-labels">Distance</span>
        <span id="Distance">%DISTANCE%</span>
        <sup class="units">Cm</sup>
    </p>
</body>
<script>
setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("Distance").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/distanceCm", true);
    xhttp.send();
}, 1000 );
</script>
</html>
)rawliteral";

String processor(const String& var) {
    if (var == "DISTANCE") {
        return readDistance();
    }
    return String();
}

void setup() {
    Serial.begin(115200);
    pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT);

    myStepper.setSpeed(5);

    WiFi.disconnect(true); 
    WiFi.begin(ssid, password);
    Serial.println("Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    if (!MDNS.begin("esp32")) {
        Serial.println("Error starting mDNS");
    } else {
        Serial.println("mDNS started");
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html, processor);
    });

    server.on("/distanceCm", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", readDistance().c_str());
    });

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    readDistance();
    if (distanceCm < DISTANCE_THRESHOLD) {
      Serial.println("Object detected and gripper closing");
      myStepper.step(-stepsPerRevolution);
    }
    delay(2000);
}
