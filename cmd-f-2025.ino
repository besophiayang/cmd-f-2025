#include <ESPmDNS.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>

const int trigPin = 5;
const int echoPin = 18;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

}
