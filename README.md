## Project Overview
This project demonstrates an ESP32-based IoT system that connects to WiFi, retrieves real-time weather data from an online API, and displays the information on a 20x4 LCD screen.

The system allows users to:
- Switch between cities using a button
- Refresh weather data instantly
- View temperature, humidity, and weather conditions on LCD

This project fulfills the assignment requirements including WiFi communication, web API interaction, JSON parsing, and integration of input/output devices.


## System Block Diagram

ESP32  
│  
├── WiFi Connection → OpenWeatherMap API  
│  
├── Input Devices  
│   ├── Button 1: Change City  
│   └── Button 2: Refresh Weather  
│  
└── Output Devices  
    ├── LCD Display (20x4)
    └── Serial Monitor


## WiFi and API Interaction

The ESP32 connects to the Wokwi WiFi network using:
- SSID: Wokwi-GUEST
- Password: (none)

Once connected, the ESP32 sends an HTTP GET request to the OpenWeatherMap API including the city name, country code, units, and API key.
http://api.openweathermap.org/data/2.5/weather?q=San%20Antonio,US&appid=API_KEY&units=imperial
The API key is required for authentication and allows the ESP32 to access real-time weather data.

The response is received in JSON format and parsed using the Arduino_JSON library. 
The extracted data (temperature, humidity, and weather description) is displayed on the LCD.


## Wokwi Simulation
https://wokwi.com/projects/456604091746078721

## Source Code
The ESP32 Arduino source code used in this project is included in this repository.

Main file:
- esp32_open_weather.ino

This file contains:
- WiFi connection setup
- HTTP request to OpenWeatherMap API
- JSON parsing of weather data
- LCD display control
- Button input handling for city change and refresh

