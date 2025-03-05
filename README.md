# battery_monitoring_circuit
Overview

This project is an Arduino-based battery voltage monitoring and logging system that measures, displays, and records voltage levels over time. It helps track battery health and detect abnormal conditions, making it useful for battery-powered systems, renewable energy applications, and backup power monitoring.

##Features

Real-time voltage monitoring using an analog input and a voltage divider.

OLED display to show battery voltage and threshold values.

Data logging to an SD card every 5 minutes with a timestamp.

Buzzer and LED alerts for critical voltage levels.

Misuse detection when voltage drops below a predefined limit, with a counter stored in EEPROM.

Adjustable voltage thresholds stored in EEPROM.

RTC integration for accurate timestamping.

Components Used

Arduino (any compatible board)

OLED Display (SSD1306)

SD Card Module

RTC Module (virtuabotixRTC)

Buzzer and LEDs for alerts

Voltage divider circuit for battery voltage measurement

EEPROM for saving threshold values and misuse count

##How It Works

Voltage Measurement:

Reads battery voltage through an analog pin (A0) using a voltage divider.

Converts the reading to actual voltage using Arduino’s ADC resolution and reference voltage.

Threshold Monitoring:

Upper (vh) and Lower (vl) thresholds are set and stored in EEPROM.

If the voltage crosses these limits, the system activates LED and buzzer alerts.

Data Logging:

Every 5 minutes, the system logs the voltage reading to an SD card (voltage.csv) with a timestamp from the RTC.

Misuse Detection:

If voltage falls below the lower threshold (vl), the system increments a misuse counter stored in EEPROM.

The counter value is logged in missuse.csv.

Display Information:

The OLED screen shows current battery voltage, thresholds, and misuse count.

User Adjustment:

Users can update vh and vl dynamically, and these values are saved in EEPROM.
