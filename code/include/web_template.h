#ifndef WEB_TEMPLATE_H
#define WEB_TEMPLATE_H

const char* htmlTemplate = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Kame Gamepad</title>
    <style>
        body {
            font-family: 'Segoe UI', sans-serif;
            background: #000000;
            color: #fff;
            margin: 0;
            padding: 20px;
            text-align: center;
        }

        h1 {
            margin-bottom: 10px;
            font-size: 28px;
        }

        .ip {
            font-size: 16px;
            color: #aaa;
            margin-bottom: 10px;
        }

        .battery {
            font-size: 16px;
            color: #aaa;
            margin-bottom: 30px;
        }

        .gamepad {
            display: flex;
            justify-content: space-between;
            align-items: center;
            flex-wrap: wrap;
            max-width: 800px;
            margin: 0 auto;
        }

        .dpad {
            display: grid;
            grid-template-columns: 60px 60px 60px;
            grid-template-rows: 60px 60px 60px;
            gap: 10px;
            margin-right: 30px;
        }

        .actions {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 20px;
        }

        button {
            width: 70px;
            height: 70px;
            font-size: 30px;
            background: #444;
            color: white;
            border: none;
            border-radius: 50%;
            box-shadow: 0 4px 10px rgba(0,0,0,0.3);
            transition: background 0.2s, transform 0.1s;
        }

        button:hover {
            background: #666;
        }

        button:active {
            transform: scale(0.95);
        }

        .empty {
            visibility: hidden;
        }

        .label {
            font-size: 14px;
            margin-top: 8px;
            color: #ddd;
        }
    </style>
</head>
<body>
    <h1>Kame32 Gamepad</h1>
    <div class="ip">IP address: <strong>{{IP}}</strong></div>
    <div class="battery">Battery level: <strong>{{BATT_PERCENT}}%</strong></div>


    <div class="gamepad">
        <!-- D-pad -->
        <div class="dpad">
            <div class="empty"></div>
            <button onmousedown="sendCmd('forward')" onmouseup="sendCmd('stop')">⬆️</button>
            <div class="empty"></div>

            <button onmousedown="sendCmd('left')" onmouseup="sendCmd('stop')">⬅️</button>
            <div class="empty"></div>
            <button onmousedown="sendCmd('right')" onmouseup="sendCmd('stop')">➡️</button>

            <div class="empty"></div>
            <button onmousedown="sendCmd('backward')" onmouseup="sendCmd('stop')">⬇️</button>
            <div class="empty"></div>
        </div>

        <!-- Action buttons -->
        <div class="actions">
            <button onmousedown="sendCmd('hello')" onmouseup="sendCmd('stop')">🔴</button>
            <button onmousedown="sendCmd('jump')" onmouseup="sendCmd('stop')">🟡</button>
            <button onmousedown="sendCmd('pushup')" onmouseup="sendCmd('stop')">🔵</button>

            <button onmousedown="sendCmd('dance')" onmouseup="sendCmd('stop')">🟠</button>
            <button onmousedown="sendCmd('moonwalk')" onmouseup="sendCmd('stop')">🟣</button>
            <button onmousedown="sendCmd('frontback')" onmouseup="sendCmd('stop')">🟢</button>
        </div>
    </div>

    <script>
        function sendCmd(cmd) {
            fetch('/cmd?val=' + cmd);
        }
    </script>
</body>
</html>
)rawliteral";

#endif // WEB_TEMPLATE_H