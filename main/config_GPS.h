/*
  Theengs OpenMQTTGateway - We Unite Sensors in One Open-Source Interface

   Act as a gateway between your 433mhz, infrared IR, BLE, LoRa signal and one interface like an MQTT broker
   Send and receiving command by MQTT

   This files enables to set your parameter for the GPS sensor

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
extern void MeasureGPS();

/*-------------------GPS topics & parameters----------------------*/

#define subjectGPStoMQTT "/GPStoMQTT"

#define TimeBetweenReadingGPS 120000 // ms

// Setup for GPS
#ifndef GPSBaud
#  define GPSBaud 9600
#endif

/*-------------------PIN DEFINITIONS----------------------*/
#ifndef GPS_UART
//
// VALUES:
//  -  undefined: use software emulation (pins set by GPS_RX_GPIO & GPS_TX_GPIO)
//  -  0: use HW UART0
//  -  1: use HW UART1
//  -  2: use HW UART2
//  -  3: use HW UART3
//
#  ifdef ESP32
#    define GPS_UART 1
#  else
#    undef GPS_UART
#  endif
#endif

#ifndef GPS_UART0_SWAP
#  define GPS_UART0_SWAP
#endif

#ifndef GPS_RX_GPIO
#  if defined(ESP8266) && !defined(GPS_UART)
#    define GPS_RX_GPIO 4
#  elif defined(ESP32)
#    define GPS_RX_GPIO 34
#  elif defined(__AVR_ATmega2560__) && !defined(GPS_UART)
#    define GPS_RX_GPIO 2
#  else
#    define GPS_RX_GPIO 0
#  endif
#endif

#ifndef GPS_TX_GPIO
#  if defined(ESP8266) && !defined(GPS_UART)
#    define GPS_TX_GPIO 2
#  elif ESP32
#    define GPS_TX_GPIO 12
#  elif defined(__AVR_ATmega2560__) && !defined(GPS_UART)
#    define GPS_TX_GPIO 9
#  else
#    define GPS_TX_GPIO 9
#  endif
#endif

#endif
