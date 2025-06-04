#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <web_template.h>
#include <minikame.h>
#include <config.h>


WebServer server(80);
int calibration[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
MiniKame robot;


void handleRoot() {
  String html = "<html><head><title>Calibration</title></head><body>";
  html += "<h1>Calibration</h1><table border='1'><tr><th>Servo</th><th>-</th><th>Valor</th><th>+</th></tr>";

  for (int i = 0; i < 8; i++) {
    html += "<tr><td>Servo " + String(i) + "</td>";
    html += "<td><a href='/decrease?i=" + String(i) + "'>-</a></td>";
    html += "<td>" + String(calibration[i]) + "</td>";
    html += "<td><a href='/increase?i=" + String(i) + "'>+</a></td></tr>";
  }

  html += "</table><br><h2>Array actual:</h2><pre>const int calibration[8] = { ";
  for (int i = 0; i < 8; i++) {
    html += String(calibration[i]);
    if (i < 7) html += ", ";
  }
  html += " };</pre></body></html>";

  server.send(200, "text/html", html);
}

void updateServo(int i) {
  if (i >= 0 && i < 8) {
    robot.setCalibration(calibration); 
    robot.setServo(i, 90);
  }
}

void handleIncrease() {
  int i = server.arg("i").toInt();
  if (i >= 0 && i < 8) {
    calibration[i]++;
    updateServo(i);
  }
  handleRoot();
}

void handleDecrease() {
  int i = server.arg("i").toInt();
  if (i >= 0 && i < 8) {
    calibration[i]--;
    updateServo(i);
  }
  handleRoot();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.println(WiFi.softAPIP());
    MDNS.begin("kame32");

robot.init();
robot.setCalibration(calibration);

  server.on("/", handleRoot);
  server.on("/increase", handleIncrease);
  server.on("/decrease", handleDecrease);
  server.begin();
}

void loop() {
  server.handleClient();
}
