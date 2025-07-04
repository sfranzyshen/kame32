#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <Arduino.h>

String gamepad_html = R"rawliteral(
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

        #top-area {
            display: flex;
            align-items: center;
            gap: 250px;
        }

        #bottom-area {
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 80px;
        }

        #joystick-area {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        #stick {
            width: 200px;
            height: 200px;
            background: #242424;
            border-radius: 50%;
            position: relative;
        }

        #dot {
            width: 100px;
            height: 100px;
            background: linear-gradient(to bottom, #ff4d4d, #b30000);
            box-shadow:
                inset -2px -2px 6px rgba(0, 0, 0, 0.3),
                inset 2px 2px 6px rgba(0, 0, 0, 0.6),
                0 4px 12px rgba(0, 0, 0, 0.4);
            border-radius: 50%;
            position: absolute;
            top: 50px;
            left: 50px;
        }

        #buttons-area {
            position: relative;
            width: 200px;
            height: 200px;
        }

        #bottom-buttons {
            display: flex;
            gap: 20px;
            margin-bottom: 80px;
        }

        button.bottom {
            background: linear-gradient(to bottom, #444, #222);
            color: white;
            border: 2px solid #555;
            border-radius: 12px;
            padding: 6px 16px;
            font-size: 18px;
            font-weight: bold;
            text-shadow: 0 1px 1px black;
            box-shadow:
                inset 0 1px 2px rgba(255,255,255,0.1),
                0 4px 10px rgba(0,0,0,0.5);
            cursor: pointer;
            transition: transform 0.1s ease, box-shadow 0.2s ease;
        }

        button.bottom:active {
            box-shadow:
                inset 0 1px 20px rgba(0, 0, 0, 0.52),
                0 2px 0px rgba(0,0,0,0.6);
        }

        button.arcade {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            border: 0px solid rgba(0, 0, 0, 0.4);
            box-shadow:
                inset -2px -2px 6px rgba(0, 0, 0, 0.3),
                inset 2px 2px 6px rgba(0, 0, 0, 0.6),
                0 4px 12px rgba(0, 0, 0, 0.4);
            transition: transform 0.02s ease, box-shadow 0.05s ease;
            position: absolute;
        }

        button.arcade:active {
            box-shadow:
                inset -1px -1px 10px rgba(0, 0, 0, 0.7),
                inset 1px 1px 10px rgba(0, 0, 0, 0.9);
        }

        .red    { background: linear-gradient(to bottom, #ff4d4d, #b30000); }
        .yellow { background: linear-gradient(to bottom, #fff176, #fdd835); }
        .blue   { background: linear-gradient(to bottom, #64b5f6, #1e88e5); }
        .orange { background: linear-gradient(to bottom, #ffb74d, #fb8c00); }
        .purple { background: linear-gradient(to bottom, #ba68c8, #8e24aa); }
        .green  { background: linear-gradient(to bottom, #81c784, #43a047); }

        .btn1 {top: 50px;left: 0px;}
        .btn2 {top: 20px;left: 75px;}
        .btn3 {top: 10px;left: 150px;}
        .btn4 {top: 130px;left: 0px;}
        .btn5 {top: 100px;left: 75px;}
        .btn6 {top: 90px;left: 150px;}
    </style>
    <meta name="viewport" content="initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
</head>
<body>
    <div id="top-area">
        <div id="joystick-area">
            <div id="stick">
                <div id="dot"></div>
            </div>
        </div>
        <div id="buttons-area">
            <button class="arcade red    btn1" onclick="handleButton('A')"></button>
            <button class="arcade yellow btn2" onclick="handleButton('B')"></button>
            <button class="arcade blue   btn3" onclick="handleButton('C')"></button>
            <button class="arcade orange btn4" onclick="handleButton('X')"></button>
            <button class="arcade purple btn5" onclick="handleButton('Y')"></button>
            <button class="arcade green  btn6" onclick="handleButton('Z')"></button>
        </div>
    </div>
    <div id="bottom-area">
        <div id="bottom-buttons">
            <button class="bottom" onclick="handleButton('Start')">Start</button>
            <button class="bottom" onclick="handleButton('Stop')">Stop</button>
        </div>
    </div>

    <script>
        const stick = document.getElementById("stick");
        const dot = document.getElementById("dot");

        let lastSent = 0;
        let throttleTime = 100; // milliseconds
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

        function handleButton(label) {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", `/button?label=${label}`);
            xhr.send();
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


        let isMouseDown = false;

        stick.addEventListener("mousedown", function (e) {
            isMouseDown = true;
            touchActive = true;
            moveDotWithMouse(e);
        });

        document.addEventListener("mousemove", function (e) {
            if (!isMouseDown) return;
            moveDotWithMouse(e);
        });

        document.addEventListener("mouseup", function () {
            if (isMouseDown) {
                isMouseDown = false;
                resetJoystick();
            }
        });

        function moveDotWithMouse(e) {
            const rect = stick.getBoundingClientRect();
            let x = e.clientX - rect.left;
            let y = e.clientY - rect.top;

            x = Math.max(20, Math.min(180, x));
            y = Math.max(20, Math.min(180, y));

            dot.style.left = (x - 50) + "px";
            dot.style.top = (y - 50) + "px";

            let xVal = Math.round((x - 100) / 100 * 100);
            let yVal = Math.round((y - 100) / 100 * -100);

            sendJoystick(xVal, yVal);
        }

       function resetJoystick() {
            dot.style.left = "50px";
            dot.style.top = "50px";
            sendJoystick(0, 0);
            touchActive = false;
        }

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

#endif // GAMEPAD_H