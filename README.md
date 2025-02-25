Here’s a well-structured and visually appealing README file for your Smart Lock project:  

---

# 🔒 Smart Lock – Secure Your Valuables with Ease  

## 📌 Overview  
Smart Lock is a compact, secure, and intelligent locking system designed to protect valuable assets with minimal hassle. Implemented on the **ESP32-C6 Dev Kit**, this prototype combines **RFID authentication, motion detection, and real-time notifications** to ensure **enhanced security** for personal belongings.  

Developed as a **bonus project** for the **Microprocessors Laboratory** at the **School of Electrical and Computer Engineering – NTUA**, this device is tailored for applications such as:  
✔ **Smart Bicycle Locks** – Get notified if someone tampers with your bike.  
✔ **Luggage Protection** – Secure your luggage and receive alerts when it's moved.  

---

## 🎯 Features  
✅ **RFID Authentication** – Lock/unlock the device using an RFID card.  
✅ **Motion Detection** – Detects movement using an **MPU6050 accelerometer**.  
✅ **Alarm Activation** – Triggers a **buzzer** when unauthorized movement is detected.  
✅ **Visual Indication** – LED indicators show lock status (Red = Locked, Green = Unlocked).  
✅ **Wireless Connectivity** – Built-in **Wi-Fi 6 & Bluetooth LE** via ESP32-C6.  
✅ **Real-Time Alerts** – Sends notifications upon tampering detection.  

---

## 🛠️ Components & Cost  
| Component | Description | Price (€) |  
|-----------|------------|-----------|  
| ESP32-C6 | SoC with Wi-Fi 6 & Bluetooth LE | 15.00 |  
| MPU6050 | Triple Axis Gyroscope & Accelerometer | 3.60 |  
| RC522 | RFID Card Reader | 4.80 |  
| LEDs | Indicator lights (Red & Green) | 0.70 each |  
| Buzzer | Alarm system | 1.20 |  
| **Total Cost:** | **~25€** |  

---

## ⚙️ How It Works  
1. **Lock Activation** – The user locks the device using an **RFID card**.  
2. **Motion Detection** – If movement surpasses a preset threshold, the **alarm is triggered**.  
3. **Real-Time Notification** – The system sends an **alert** to the user.  
4. **Unlocking** – The lock remains active **until** a valid **RFID card** is scanned again.  

---

## 🖥️ Software Implementation  
### **ESP32 Firmware (C/C++)**  
- Uses **FreeRTOS tasks** for efficient multitasking.  
- **NVS Flash Initialization** for persistent data storage.  
- **Wi-Fi Connection Handling** for future cloud integration.  
- **MPU6050 & RC522 Drivers** to interface with sensors and RFID.  
- **Task Scheduling** for motion detection and LED control.  

### **Key Code Highlights**  
- **Motion Detection Task** (MPU6050-based)  
- **RFID Scan Task** (RC522-based)  
- **LED Control Task**  
- **Lock/Unlock Control with State Management**  

---

## 🔧 Hardware Schematic  
*(Refer to the project documentation for detailed circuit schematics.)*  

---

## 🎬 Live Demonstration  
🚀 A real-time demo of the Smart Lock was showcased, demonstrating its functionality in **bicycle security & luggage protection** scenarios.  

---

## 🏆 Contributors  
👨‍💻 Developed by **[Nikolaos Angelitsis , Nestoras Koumis]**  
📍 Microprocessors Laboratory, **ECE NTUA**  
