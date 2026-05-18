/*
  Theengs OpenMQTTGateway - We Unite Sensors in One Open-Source Interface

   Act as a gateway between your 433mhz, infrared IR, BLE, LoRa signal and one interface like an MQTT broker
   Send and receiving command by MQTT

   GPS sensor module

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
#include "User_config.h"

#ifdef ZsensorGPS
#  include <TinyGPS++.h>
#  include "TheengsCommon.h"
#  include "config_GPS.h"

TinyGPSPlus gps;

unsigned long timeGPS = 0;

#  ifndef GPS_UART
#    include <SoftwareSerial.h>
SoftwareSerial GPSSoftSerial(GPS_RX_GPIO, GPS_TX_GPIO);
#  endif

Stream* GPSStream = NULL;

void setupGPS() {
#  ifdef GPS_UART
#    if GPS_UART == 0
  Serial.end();
#      ifdef ESP32
  Serial.begin(GPSBaud, SERIAL_8N1, GPS_RX_GPIO, GPS_TX_GPIO);
#      else
  Serial.begin(GPSBaud, SERIAL_8N1);
#      endif
#      if defined(ESP8266) && defined(GPS_UART0_SWAP)
  Serial.swap();
#      endif
  GPSStream = &Serial;
  THEENGS_LOG_NOTICE(F("GPS HW UART0" CR));

#    elif GPS_UART == 1
  Serial1.end();
#      ifdef ESP32
  Serial1.begin(GPSBaud, SERIAL_8N1, GPS_RX_GPIO, GPS_TX_GPIO);
#      else
  Serial1.begin(GPSBaud, SERIAL_8N1);
#      endif
  GPSStream = &Serial1;
  THEENGS_LOG_NOTICE(F("GPS HW UART1" CR));

#    elif GPS_UART == 2
  Serial2.end();
#      ifdef ESP32
  Serial2.begin(GPSBaud, SERIAL_8N1, GPS_RX_GPIO, GPS_TX_GPIO);
#      else
  Serial2.begin(GPSBaud, SERIAL_8N1);
#      endif
  GPSStream = &Serial2;
  THEENGS_LOG_NOTICE(F("GPS HW UART2" CR));

#    elif GPS_UART == 3
  Serial3.end();
  Serial3.begin(GPSBaud, SERIAL_8N1);
  GPSStream = &Serial3;
  THEENGS_LOG_NOTICE(F("GPS HW UART3" CR));
#    endif

#  else
  pinMode(GPS_RX_GPIO, INPUT);
  pinMode(GPS_TX_GPIO, OUTPUT);
  GPSSoftSerial.begin(GPSBaud);
  GPSStream = &GPSSoftSerial;

  THEENGS_LOG_NOTICE(F("GPS_RX_GPIO: %d" CR), GPS_RX_GPIO);
  THEENGS_LOG_NOTICE(F("GPS_TX_GPIO: %d" CR), GPS_TX_GPIO);
#  endif

  while (GPSStream->available() > 0) {
    GPSStream->read();
  }

  THEENGS_LOG_NOTICE(F("GPSBaud: %d" CR), GPSBaud);
  THEENGS_LOG_TRACE(F("sensorGPS setup done" CR));
}

void MeasureGPS() {
  if (GPSStream->available() > 0 && gps.encode(GPSStream->read())) {
    if (millis() > (timeGPS + TimeBetweenReadingGPS) && gps.time.isValid()) {
      timeGPS = millis();
      THEENGS_LOG_TRACE(F("Creating GPS buffer" CR));
      StaticJsonDocument<JSON_MSG_BUFFER> GPSdataBuffer;
      JsonObject GPSdata = GPSdataBuffer.to<JsonObject>();

      GPSdata["satellites"] = gps.satellites.value();
      GPSdata["latitude"] = gps.location.lat();
      GPSdata["longitude"] = gps.location.lng();
      GPSdata["altitude"] = gps.altitude.meters();
      GPSdata["year"] = gps.date.year();
      GPSdata["month"] = gps.date.month();
      GPSdata["day"] = gps.date.day();
      GPSdata["hour"] = gps.time.hour();
      GPSdata["minute"] = gps.time.minute();
      GPSdata["second"] = gps.time.second();
      GPSdata["centisecond"] = gps.time.centisecond();
      GPSdata["origin"] = subjectGPStoMQTT;
      enqueueJsonObject(GPSdata);
    }
  }
}

#endif
