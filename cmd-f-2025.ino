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
        @import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600&display=swap');

        body {
            background-color: #000;
            color: white;
            font-family: 'Inter', sans-serif;
            margin: 0;
            padding: 0;
            height: 3000px;
            overflow-y: scroll;
        }

        .container {
            width: 1900px;
            margin: 0 auto;
        }

        nav {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 20px;
        }

        nav a {
            color: white;
            text-decoration: none;
            margin-left: 70px;
            padding: 10px 30px;
            border-radius: 20px;
            transition: all 0.3s;
            font-size: 24px;
            font-weight: 300;
        }

        nav a.active {
            border: 2px solid;
            border-image: linear-gradient(90deg, #680092, white);
            border-image-slice: 1;
        }

        .logo {
            font-size: 150px;
            font-weight: 300;
            background: linear-gradient(90deg, #680092, white);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        .meet-otto {
            text-align: left;
            padding: 450px 0 40px 120px; 
        }

        .meet-otto span {
            font-size: 120px;
            line-height: 1;
        }

        .meet-otto .meet {
            color: #b96eff;
            font-weight: 300;
            margin-right: 30px;
        }

        .meet-otto .otto {
            color: white;
            font-weight: 600;
        }

        .meet-description {
            font-size: 28px;
            color: #b3b3b3;
            font-weight: 300;
            margin-top: 15px;
            padding-left: 205px;
        }

          .functions {
              text-align: center;
              padding: 50px 0;
              padding-top: 300px; 
          }


        .functions-header {
            font-family: 'Inter', sans-serif;
            font-weight: 300;
            font-size: 80px;
            background: linear-gradient(90deg, #680092, white);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin-bottom: 40px;
        }

        .function-boxes {
            display: flex;
            justify-content: space-around;
            gap: 20px;
        }

        .function-boxes {
            display: flex;
            justify-content: space-between;
            gap: 40px;
            margin-top: 20px;
        }

        .function-box {
            background-color: #111;
            padding: 30px;
            border-radius: 15px;
            flex-basis: 30%;
        }

        .function-box p {
            font-size: 18px;
            color: #b3b3b3;
            line-height: 1.6;
        }

        .function-box strong {
            color: white;
        }

    </style>
</head>
<body>
    <div class="container">
        <nav>
            <div class="logo">Otto.</div>
            <div>
                <a class="active" href="#">Home</a>
                <a href="#">About</a>
                <a href="#">Check on Otto</a>
            </div>
        </nav>

                <!-- Meet Otto Section -->
        <div class="meet-otto">
            <span class="meet">Meet</span> <span class="otto">Otto</span>
        </div>
        <div class="meet-description">
            A nature inspired "otto"-matic fruit harvester
        </div>

        <!-- Functions Section -->
        <div class="functions">
            <h2 class="functions-header">Functions</h2>
            <div class="function-boxes">
                <div class="function-box">
                    <p><strong>Harvesting:</strong> Otto uses an ultrasonic distance sensor to detect the proximity of ripe fruit. Once detected, Otto extends its gripper to carefully harvest the fruit without causing damage.</p>
                </div>
                <div class="function-box">
                    <p><strong>Distance Measurement:</strong> The ultrasonic sensor measures the distance between Otto and the fruit. If the fruit is within a pre-set range, Otto activates its stepper motor to perform the harvest.</p>
                </div>
                <div class="function-box">
                    <p><strong>Wireless Monitoring:</strong> You can monitor Otto’s performance remotely via the ESP32 web server. The real-time distance from fruit and harvesting status are displayed on the dashboard.</p>
                </div>
            </div>
        </div>

    <script>
        setInterval(function() {
            fetch("/distanceCm").then(response => response.text()).then(data => {
                document.getElementById("distance").innerText = data;
            });
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
      myStepper.step(stepsPerRevolution);
      delay(3000);
      myStepper.step(-(stepsPerRevolution*2));
    }
    delay(2000);
}
