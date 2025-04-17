🛰️ Radar Scanning System using CVI & Arduino
============================================

📖 Description
--------------
The **Radar Scanning System** is a hardware-software integration project that simulates a radar sweep using a servo motor and ultrasonic sensor. Data is captured by an Arduino UNO and visualized in real-time using a desktop application built with **LabWindows/CVI**.

This project is ideal for demonstrating embedded systems, real-time visualization, multithreaded data processing, and basic sensor integration.

✨ Features
-----------
🎯 **Real-time Radar Sweep** – Servo motor scans from 0° to 180°, collecting distance data.  
📈 **Live Graph Panel** – CVI displays real-time distance over time.  
📊 **Statistical Analysis** – Parallel threads calculate average, minimum, and maximum distances dynamically.  
🧵 **Multithreaded Architecture** – UI updates and stat calculations run in separate threads.  
🧩 **Modular Design** – Core logic for radar drawing and statistics is exported via a custom DLL.  
📡 **Serial Communication** – Efficient data transfer between Arduino and PC via COM port.

🧰 Components
-------------
- Arduino UNO
- HC-SR04 Ultrasonic Distance Sensor
- Servo Motor (SG90 or similar)
- USB cable for serial connection
- PC running CVI application

⚙️ System Architecture
----------------------
1. Arduino controls the servo and measures distance at each angle.
2. Distance and angle are sent via serial to the PC.
3. CVI reads the serial data and:
   - Draws radar arcs on a custom canvas.
   - Plots distance in a real-time graph.
   - Launches threads to compute min/max/average in parallel.
4. DLL functions handle radar logic and statistical calculations.
5. UI updates dynamically with every new frame.

🖼️ Screenshots
---------------
![Radar UI](Images&video/Radar_UI.png)

🔌 Setup Instructions
---------------------

### 🛠️ Hardware Setup
- Connect **HC-SR04** sensor to pins 10 (Trig) and 11 (Echo) on the Arduino.
- Connect **Servo motor** to pin 12.
- Power the components using Arduino + USB.

### 🖥️ Software Setup
1. Upload `radar_scanner.ino` to the Arduino using Arduino IDE.
2. Open CVI project (`Final_Project.c` and `.uir`) in NI LabWindows/CVI.
3. Compile and run the application. Make sure COM port is correctly set (default: COM11).
4. Place your radar system on a flat surface and observe the UI updates.

📽️ Demo Video
--------------
*(Upload a demo and place the link here)*  
👉 [Watch the radar demo](Images&video/Radar_Video.mp4)

🧠 How It Works
----------------
- Servo rotates between 0°–180°.
- At each angle, Arduino measures the distance using ultrasonic sensor.
- The angle and distance are sent via Serial.
- CVI visualizes this as radar arcs and logs the distance values.
- Threads calculate and display live average, min, and max values in the UI.
- Radar redraws continuously to simulate real-world scanning.

🚀 Future Enhancements
----------------------
- Add obstacle tracking using color/position memory.
- Include logging and CSV export of data.
- Replace Arduino UNO with ESP32 and enable Wi-Fi transmission.
- Enhance GUI with target detection alerts.
