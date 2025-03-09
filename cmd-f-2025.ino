#include <ESPmDNS.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>

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

const char* ssid = "WiFi"
const char* password = "WiFi-Password"

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

}

void loop() {
  Serial.println(readDistance());
  delay(1000);
}
