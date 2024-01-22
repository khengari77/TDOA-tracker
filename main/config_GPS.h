/*  
  OpenMQTTGateway  - ESP8266 or Arduino program for home automation 

   Send data from GPS device to a MQTT broker 
   Send and receiving command by MQTT
 
   This files enables to set your parameter for the GPS gateway 
  
    Copyright: (c)Abdulrahman Khengari
  
    This file is part of OpenMQTTGateway.
    
    OpenMQTTGateway is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenMQTTGateway is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef config_GPS_h
#define config_GPS_h

#include <TinyGPS++.h>

extern void setupGPS();
extern void GPStoMQTT();
extern void MQTTtoGPS(char* topicOri, JsonObject& GPSdata);

/*-------------------GPS topics & parameters----------------------*/

// Settings GPS to MQTT topic
#define subjectGPStoMQTT "/GPStoMQTT"

// Time between each GPS measurement
#define TimeBetweenReadingGPS 1000 // ms

// setting to specify mode used for sending MQTT messages:
//   0: RAW: all recieved input at GPS interface is collected in single MQTT message
//      defined by GPStoMQTTsubject
//
//   1: JSON: Assumes input at GPS interface to be valid JSON. JSON keys are used to
//      split the JSON data in separate MQTT sub-topics. This will be repeated for
//      sub-keys up to the specified nesting level (GPSmaxJSONlevel).
//
//      EXAMPLE: "{temperature: {sens1: 22, sens2: 23}, humidity: {sens1: 80, sens2: 60}}"
//        - with GPSmaxJSONlevel=1:
//            ./GPStoMQTT/temperature  ==> "{sens1: 22, sens2: 23}"
//            ./GPStoMQTT/humidity     ==> "{sens1: 80, sens2: 60}"
//
//        - with GPSmaxJSONlevel=2 (or higher):
//            ./GPStoMQTT/temperature/sens1 ==> 22
//            ./GPStoMQTT/temperature/sens2 ==> 23
//            ./GPStoMQTT/humidity/sens1 ==> 80
//            ./GPStoMQTT/humidity/sens2 ==> 60
//
//#ifndef GPStoMQTTmode
//#  define GPStoMQTTmode 0
//#endif
//
//// settings for GPSTopicMode 0 (RAW)
//#define GPSInPost '\n' // Hacky way to get last character of postfix for incoming
//#define MAX_INPUT   200 // how much serial data we expect
//
//// settings for GPSTopicMode 1 (JSON)
//#define GPSmaxJSONlevel 2 // Max nested level in which JSON keys are converted to seperate sub-topics
//#define GPSJSONDocSize  1024 // bytes to reserve for the JSON doc
//
//// settings for MQTT to GPS
//#define subjectMQTTtoGPS "/commands/MQTTtoGPS"
//#define GPSPre           "00" // The prefix for the GPS message
//#define GPSPost          "\r" // The postfix for the GPS message

//Setup for GPS
#ifndef GPSBaud
#  define GPSBaud 9600 // The serial connection Baud
#endif

/*-------------------PIN DEFINITIONS----------------------*/
#ifndef GPS_UART
// set hardware serial UART to be used for device communication or
// use software emulaton if not defined
//
// VALUES:
//  -  not defined: use software emulation (pins set by GPS_RX_GPIO & GPS_TX_GPIO)
//
//  -  0: use HW UART0 (serial), warning: default used for logging & usb
//        ESP8266:     (TX0 GPIO1,  RX0 GPIO3)
//                     (TX0 GPIO15, RX0 GPIO13) with UART0 swap enabled
//        ESP32:       (TX0 by GPS_TX_GPIO, RX0 by GPS_RX_GPIO)
//        ATmega2560:  (TX0 pin1, RX0 pin0)
//        Arduino Uno: (TX0 pin1, RX0 pin0)
//
//  -  1: use HW UART1 (serial1)
//        ESP8266:     (TX1 GPIO2,  RX none) only transmit available
//        ESP32:       (TX1 by GPS_TX_GPIO,  RX1 by GPS_RX_GPIO)
//        ATmega2560:  (TX1 pin18, RX1 pin19)
//        Arduino Uno: N/A
//
//  -  2: use HW UART2 (serial2)
//        ESP8266:     N/A
//        ESP32:       (TX2 by GPS_TX_GPIO,  RX2 by GPS_RX_GPIO)
//        ATmega2560:  (TX2 pin16, RX1 pin17)
//        Arduino Uno: N/A
//
//  -  3: use HW UART3 (serial3)
//        ESP8266:     N/A
//        ESP32:       N/A
//        ATmega2560:  (TX2 pin14, RX1 pin15)
//        Arduino Uno: N/A
//
// defaults
#  ifdef ESP32
#    define GPS_UART 1 // use HW UART ESP32
#  else
#    undef GPS_UART // default use software serial
//#  define GPS_UART 1 // define to use HW UART
#  endif
#endif

#ifndef GPS_UART0_SWAP
// option for ESP8266 only to swap UART0 ports from (GPIO1,GPIO3) to (GPIO15,GPIO13)
#  define GPS_UART0_SWAP
#endif

#ifndef GPS_RX_GPIO
// define receive pin (for software serial or ESP32 with configurable HW UART)
#  if defined(ESP8266) && !defined(GPS_UART)
#    define GPS_RX_GPIO 4 //D2
#  elif defined(ESP32)
#    define GPS_RX_GPIO 34
#  elif defined(__AVR_ATmega2560__) && !defined(GPS_UART)
#    define GPS_RX_GPIO 2 // 2 = D2 on arduino mega
#  else
#    define GPS_RX_GPIO 0 // 0 = D2 on arduino UNO
#  endif
#endif

#ifndef GPS_TX_GPIO
// define transmit pin (for software serial and/or ESP32)
#  if defined(ESP8266) && !defined(GPS_UART)
#    define GPS_TX_GPIO 2 //D4
#  elif ESP32
#    define GPS_TX_GPIO 12
#  elif defined(__AVR_ATmega2560__) && !defined(GPS_UART)
#    define GPS_TX_GPIO 9
#  else
#    define GPS_TX_GPIO 9
#  endif
#endif

#endif
