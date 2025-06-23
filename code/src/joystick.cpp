#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <config.h>
#include <kame.h>


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
    let touchActive = false;

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

    function resetJoystick() {
      dot.style.left = "50px";
      dot.style.top = "50px";
      sendJoystick(0, 0);
      touchActive = false;
    }

    stick.addEventListener("touchstart", function () {
      touchActive = true;
    });

    stick.addEventListener("touchmove", function (e) {
      e.preventDefault();
      touchActive = true;
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

    document.addEventListener("touchend", resetJoystick);
    document.addEventListener("touchcancel", resetJoystick);
    document.addEventListener("touchleave", resetJoystick);

    setInterval(() => {
      if (!touchActive) {
        sendJoystick(0, 0);
      }
    }, 200); // every 200ms
  </script>
</body>
</html>
)rawliteral";



Kame robot;
WebServer server(80);

int joy_x = 0;
int joy_y = 0;
bool start = false;
unsigned long start_time = 0;


void handleRoot() {
    server.send(200, "text/html", html);
}

void handleJoystick() {
    if (server.hasArg("x") && server.hasArg("y")) {
        joy_x = server.arg("x").toInt();
        joy_y = server.arg("y").toInt();
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Missing x or y");
    }
}

void handleStart() {
    start = true;
    server.send(200, "text/plain", "Started");
}

int a = 10;

void handleStop() {
    start = false;
    server.send(200, "text/plain", "Stopped");
}


float progress = 0;
float period = 450;
float leg_spread = 20;
float body_height = 10;
float body_shift = 0;
float step_amplitude = 0;
float step_height = 20;
float phase_linear[] =  {90,  90,  270, 90,  270, 270, 90,  270};
float phase_angular[] = {90,  270, 270, 90,  270, 90,  90,  270};
float phase[] =         {0,   0,   0,   0,   0,   0,   0,   0};

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASSWORD);
    //WiFi.mode(WIFI_STA);
    //WiFi.begin(SSID, PASSWORD);
    MDNS.begin(HOSTNAME);
    
    robot.init();
    robot.loadCalibration();

    robot.home();

    server.on("/", handleRoot);
    server.on("/joystick", handleJoystick);
    server.on("/start", handleStart);
    server.on("/stop", handleStop);

    server.begin();
}

void loop() {
    server.handleClient();

    progress += robot.oscillator[0].getPhaseProgress();
    while(progress > 360) progress -= 360;

    if (abs(joy_y) >= abs(joy_x)) {
        // Linear movement
        step_amplitude = joy_y * 0.25;
        body_shift = step_amplitude * 0.8;

        phase[0] = phase_linear[0] + progress;
        phase[1] = phase_linear[1] + progress;
        phase[2] = phase_linear[2] + 2 * progress;
        phase[3] = phase_linear[3] + 2 * progress;
        phase[4] = phase_linear[4] + progress;
        phase[5] = phase_linear[5] + progress;
        phase[6] = phase_linear[6] + 2 * progress;
        phase[7] = phase_linear[7] + 2 * progress;
    } else {
        // Angular movement
        step_amplitude = joy_x * 0.25;
        body_shift = 0.0;

        phase[0] = phase_angular[0] + progress;
        phase[1] = phase_angular[1] + progress;
        phase[2] = phase_angular[2] + 2 * progress;
        phase[3] = phase_angular[3] + 2 * progress;
        phase[4] = phase_angular[4] + progress;
        phase[5] = phase_angular[5] + progress;
        phase[6] = phase_angular[6] + 2 * progress;
        phase[7] = phase_angular[7] + 2 * progress;
    }

    for (int i = 0; i < 8; i++) {
        robot.oscillator[i].setPhase(phase[i]);
        robot.oscillator[i].reset();
    }

    robot.oscillator[0].setAmplitude(step_amplitude);
    robot.oscillator[1].setAmplitude(step_amplitude);
    robot.oscillator[4].setAmplitude(step_amplitude);
    robot.oscillator[5].setAmplitude(step_amplitude);

    robot.oscillator[2].setAmplitude(step_height);
    robot.oscillator[3].setAmplitude(step_height);
    robot.oscillator[6].setAmplitude(step_height);
    robot.oscillator[7].setAmplitude(step_height);

    robot.oscillator[0].setOffset(90 + leg_spread - body_shift);
    robot.oscillator[1].setOffset(90 - leg_spread + body_shift);
    robot.oscillator[4].setOffset(90 - leg_spread - body_shift);
    robot.oscillator[5].setOffset(90 + leg_spread + body_shift);

    robot.oscillator[2].setOffset(90 - body_height);
    robot.oscillator[3].setOffset(90 + body_height);
    robot.oscillator[6].setOffset(90 + body_height);
    robot.oscillator[7].setOffset(90 - body_height);

    robot.oscillator[0].setPeriod(period);
    robot.oscillator[1].setPeriod(period);
    robot.oscillator[4].setPeriod(period);
    robot.oscillator[5].setPeriod(period);

    robot.oscillator[2].setPeriod(period / 2);
    robot.oscillator[3].setPeriod(period / 2);
    robot.oscillator[6].setPeriod(period / 2);
    robot.oscillator[7].setPeriod(period / 2);

    bool side;
    side = progress > 180 ? true : false;

    if (start) {
        robot.setServo(0, robot.oscillator[0].refresh());
        robot.setServo(1, robot.oscillator[1].refresh());
        robot.setServo(4, robot.oscillator[4].refresh());
        robot.setServo(5, robot.oscillator[5].refresh());

        if (!side) {
            robot.setServo(3, robot.oscillator[3].refresh());
            robot.setServo(6, robot.oscillator[6].refresh());
        } else {
            robot.setServo(2, robot.oscillator[2].refresh());
            robot.setServo(7, robot.oscillator[7].refresh());
        }
    }
    else{
        robot.home();
    }
}
