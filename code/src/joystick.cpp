#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <config.h>
#include <minikame.h>


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


MiniKame robot;
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

int output = 0;
float progress = 0;

int T = 2000; // Period in milliseconds
int x_amp = 15;
int z_amp = 20;
int ap = 20;
int hi = 10;
float front_x = 0; //12;
int period[] = {T, T, T/2, T/2, T, T, T/2, T/2};
int amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
int offset[] = {    90+ap-front_x,
                    90-ap+front_x,
                    90-hi,
                    90+hi,
                    90-ap-front_x,
                    90+ap+front_x,
                    90+hi,
                    90-hi
                };
int  phase[] = {90, 90, 270, 90, 270, 270, 90, 270};

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

    for (int i=0; i<8; i++){
        robot.oscillator[i].reset();
        robot.oscillator[i].setPeriod(period[i]);
        robot.oscillator[i].setAmplitude(amplitude[i]);
        robot.oscillator[i].setPhase(phase[i]);
        robot.oscillator[i].setOffset(offset[i]);
    }

    server.on("/", handleRoot);
    server.on("/joystick", handleJoystick);
    server.on("/start", handleStart);
    server.on("/stop", handleStop);

    server.begin();

}


float m_period = 400;
int m_amp = 0;


void updateParameters() {
    if (Serial.available() > 0) {
        char key = Serial.read();
        if (key == 'a') {
            m_amp++;
            if (m_amp > 20) m_amp = 20;
        } else if (key == 'z') {
            m_amp--;
            if (m_amp < -20) m_amp = -20;
        }
        else if (key == 's') {
            m_period += 100;
            if (m_period > 5000) m_period = 5000;
        } else if (key == 'x') {
            m_period -= 100;
            if (m_period < 300) m_period = 300;
        }
    }
}

float p[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void loop() {
    server.handleClient();
    //updateParameters();
    //robot.setServo(0, 120 + joy_x * -0.2);
    //robot.setServo(2, 90 + joy_y * 0.2);
    
    progress += robot.oscillator[0].getPhaseProgress();
    while(progress > 360) progress -= 360;
    
    m_amp = joy_y * 0.25;
    Serial.print("Amplitude: ");
    Serial.println(m_amp);
    front_x = m_amp * 0.8;

    robot.oscillator[0].setAmplitude(m_amp);
    robot.oscillator[1].setAmplitude(m_amp);
    robot.oscillator[4].setAmplitude(m_amp);
    robot.oscillator[5].setAmplitude(m_amp);

    robot.oscillator[0].setOffset(90 + ap - front_x);
    robot.oscillator[1].setOffset(90 - ap + front_x);
    robot.oscillator[4].setOffset(90 - ap - front_x);
    robot.oscillator[5].setOffset(90 + ap + front_x);

    robot.oscillator[0].setPeriod(m_period);
    robot.oscillator[1].setPeriod(m_period);
    robot.oscillator[4].setPeriod(m_period);
    robot.oscillator[5].setPeriod(m_period);
    
    robot.oscillator[2].setPeriod(m_period / 2);
    robot.oscillator[3].setPeriod(m_period / 2);
    robot.oscillator[6].setPeriod(m_period / 2);
    robot.oscillator[7].setPeriod(m_period / 2);

    p[0] = phase[0] + progress;
    p[1] = phase[1] + progress;
    p[2] = phase[2] + 2*progress;
    p[3] = phase[3] + 2*progress;
    p[4] = phase[4] + progress;
    p[5] = phase[5] + progress;
    p[6] = phase[6] + 2*progress;
    p[7] = phase[7] + 2*progress;

    for (int i = 0; i < 8; i++) {
        robot.oscillator[i].setPhase(p[i]);
        robot.oscillator[i].reset();
    }

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
