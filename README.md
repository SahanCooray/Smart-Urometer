## Description
This project focuses on developing an IoT-based smart urometer designed to improve real-time urine level monitoring in hospitals. The device integrates a mobile application for better accessibility and reduces the workload for healthcare professionals.
![Smart Urometer Prototype](Assets/Final Product.png)
## Features
- IoT-enabled urine monitoring system
- Mobile and desktop application integration
- Real-time data processing and transmission
- Compact and lightweight design
- Cost-effective solution for hospitals

## Components Used
- **Microcontroller**: ESP32 WROOM
- **Load Sensor**: 5Kg load cell
- **Capacitive Liquid Level Sensor**: XKC-Y25
- **ADC Module**: HX711
- **Power Supply**: 9V battery & adapter
- **Mobile App**: Real-time monitoring and alert system

## Problem Statement
"There is a need for an affordable and reliable IoT-based urome
ter with mobile app integration in Sri Lankan hospitals to improve
 real-time urine level monitoring, reduce nurse workload, and ad
dress the inefficiencies of manual measurements"

## Device Details and Methodology
![Smart Urometer Prototype](Assets/FunctionalBlockDiagram.png)


## How It Works
1. The urometer collects urine volume data using a 5Kg load cell and HX711 ADC.
2. The ESP32 microcontroller processes and transmits data via Wi-Fi.
3. A mobile application displays real-time volume and triggers alerts when thresholds are breached.
4. The power supply ensures continuous operation with both adapter and battery backup.

## Device Enclosure
![Smart Urometer Prototype](Assets/EnclosurePart1.png)
![Smart Urometer Prototype](Assets/EnclosurePart2.png)

## Mobile APP
![Smart Urometer Prototype](Assets/MobileApp.jpg)
## License
This project is licensed under the [MIT License](LICENSE).
