#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <config.h>
#include <minikame.h>


MiniKame robot;
WebServer server(80);

int joystick_x = 0;
int joystick_y = 0;

String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8" />
  <title>Joystick Virtual</title>
  <style>
    body {
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      height: 100vh;
      background: #111;
      margin: 0;
    }

    #buttons {
      display: flex;
      gap: 20px;
      margin-bottom: 80px;
    }

    button {
      background-color: red;
      color: white;
      border: none;
      border-radius: 12px;
      padding: 20px 40px;
      font-size: 22px;
      cursor: pointer;
    }

    #stick {
      width: 200px;
      height: 200px;
      background: #333;
      border-radius: 50%;
      position: relative;
    }

    #dot {
      width: 100px;
      height: 100px;
      background: red;
      border-radius: 50%;
      position: absolute;
      top: 50px;
      left: 50px;
    }
  </style>
  <meta name="viewport" content="initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
</head>
<body>

  <div id="buttons">
    <button onclick="sendStart()">Start</button>
    <button onclick="sendStop()">Stop</button>
  </div>

  <div id="stick">
    <div id="dot"></div>
  </div>

  <script>
    const stick = document.getElementById("stick");
    const dot = document.getElementById("dot");

    let lastSent = 0;
    let throttleTime = 50; // milisegundos

    function sendJoystick(x, y) {
        const now = Date.now();
        if (now - lastSent >= throttleTime) {
        const xhr = new XMLHttpRequest();
        xhr.open("GET", `/joystick?x=${x}&y=${y}`);
        xhr.send();
        lastSent = now;
        }
    }

    function sendStart() {
      const xhr = new XMLHttpRequest();
      xhr.open("GET", "/start");
      xhr.send();
    }

    function sendStop() {
      const xhr = new XMLHttpRequest();
      xhr.open("GET", "/stop");
      xhr.send();
    }

    stick.addEventListener("touchmove", function (e) {
      e.preventDefault();
      const touch = e.touches[0];
      const rect = stick.getBoundingClientRect();
      let x = touch.clientX - rect.left;
      let y = touch.clientY - rect.top;

      x = Math.max(20, Math.min(180, x));
      y = Math.max(20, Math.min(180, y));

      dot.style.left = (x - 50) + "px";
      dot.style.top = (y - 50) + "px";

      let xVal = Math.round((x - 100) / 100 * 100);
      let yVal = Math.round((y - 100) / 100 * -100);

      sendJoystick(xVal, yVal);
    }, { passive: false });

    stick.addEventListener("touchend", function () {
      dot.style.left = "50px";
      dot.style.top = "50px";
      sendJoystick(0, 0);
    });
  </script>

</body>
</html>

)rawliteral";


void handleRoot() {
    server.send(200, "text/html", html);
}

void handleJoystick() {
    if (server.hasArg("x") && server.hasArg("y")) {
        joystick_x = server.arg("x").toInt();
        joystick_y = server.arg("y").toInt();
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Missing x or y");
    }
}

void handleStart() {
    robot.home();
        server.send(200, "text/plain", "Started");
}

void handleStop() {
    robot.zero();
        server.send(200, "text/plain", "Stopped");
}

void setup() {
    Serial.begin(115200);
    //pinMode(LED_PIN, OUTPUT);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASSWORD);
    //WiFi.mode(WIFI_STA);
    //WiFi.begin(SSID, PASSWORD);
    MDNS.begin(HOSTNAME);

    robot.init();
    robot.loadCalibration();
    //robot.setCalibration(servo_calibration);
    robot.home();

    server.on("/", handleRoot);
    server.on("/joystick", handleJoystick);
    server.on("/start", handleStart);
    server.on("/stop", handleStop);
    
    server.begin();
}


void loop() {
    server.handleClient();

    robot.setServo(0, 120 + joystick_x * -0.2);
    robot.setServo(2, 90 + joystick_y * 0.2);
    
}