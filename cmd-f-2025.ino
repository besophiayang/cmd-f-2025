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

const char* ssid = "WiFi";
const char* password = "WiFi-Password";
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
<html>
<head><title>ESP32 Distance Sensor Test Website</title></head>
<body>
    <h1>Ultrasonic Sensor</h1>
    <p>Distance: <span id="Distance">%DISTANCE%</span> cm</p>
    <script>
        setInterval(async () => {
            let res = await fetch('/distanceCm');
            let text = await res.text();
            document.getElementById('Distance').innerText = text;
        }, 1000);
    </script>
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

    WiFi.disconnect(true); 
    WiFi.begin(ssid, password);
    Serial.println("Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to Wi-Fi");
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
  Serial.println(readDistance());
  delay(1000);
}
