#include <ESPmDNS.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>

const int trigPin = 5;
const int echoPin = 18;
#define SOUND_SPEED 0.034

long duration;
float distanceCm;

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
 
    WiFi.begin(ssid, password);

}

void loop() {
  Serial.println(readDistance());
  delay(1000);
}
