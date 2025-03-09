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
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Otto.</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;600&display=swap');
        body { background-color: #000; color: white; font-family: 'Inter', sans-serif; margin: 0; padding: 0; }
        nav { display: flex; justify-content: space-between; align-items: center; padding: 20px; }
        nav a { color: white; text-decoration: none; margin-left: 70px; padding: 10px 30px; border-radius: 20px; transition: all 0.3s; font-size: 24px; }
        nav a.active { border: 2px solid; border-image: linear-gradient(90deg, #a855f7, white); border-image-slice: 1; }
        .logo img { height: 200px; }
    </style>
</head>
<body>
    <div class="container">
        <nav>
            <div class="logo"><img src="otto_logo.jpg" alt="Otto Logo"></div>
            <div>
                <a class="active" href="#">Home</a>
                <a href="#">About</a>
                <a href="#">Check on Otto</a>
            </div>
        </nav>
    </div>
</body>
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
