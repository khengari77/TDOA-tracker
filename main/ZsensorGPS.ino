/*  
  OpenMQTTGateway  - ESP8266 or Arduino program for home automation 

   Send data from GPS device to a MQTT broker 
   Send and receiving command by MQTT
 
  This gateway enables to:
 - receive MQTT data from a topic and send GPS signal corresponding to the received MQTT data
 - publish MQTT data to a different topic related to received GPS signal

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

TinyGPSPlus gps;

unsigned long timeGPS = 0;

#  ifndef GPS_UART // software serial mode
#    include <SoftwareSerial.h>
SoftwareSerial GPSSoftSerial(GPS_RX_GPIO, GPS_TX_GPIO); // RX, TX
#  endif

// use pointer to stream class for serial communication to make code
// compatible with both softwareSerial as hardwareSerial.
Stream* GPSStream = NULL;

void setupGPS() {
//Initalize serial port
#  ifdef GPS_UART // Hardware serial
#    if GPS_UART == 0 // init UART0
  Serial.end(); // stop if already initialized
#      ifdef ESP32
  Serial.begin(GPSBaud, SERIAL_8N1, GPS_RX_GPIO, GPS_TX_GPIO);
#      else
  Serial.begin(GPSBaud, SERIAL_8N1);
#      endif
#      if defined(ESP8266) && defined(GPS_UART0_SWAP)
  Serial.swap(); // swap UART0 ports from (GPIO1,GPIO3) to (GPIO15,GPIO13)
#      endif
  GPSStream = &Serial;
  Log.notice(F("GPS HW UART0" CR));

#    elif GPS_UART == 1 // init UART1
  Serial1.end(); // stop if already initialized
#      ifdef ESP32
  Serial1.begin(GPSBaud, SERIAL_8N1, GPS_RX_GPIO, GPS_TX_GPIO);
#      else
  Serial1.begin(GPSBaud, SERIAL_8N1);
#      endif
  GPSStream = &Serial1;
  Log.notice(F("GPS HW UART1" CR));

#    elif GPS_UART == 2 // init UART2
  Serial2.end(); // stop if already initialized
#      ifdef ESP32
  Serial2.begin(GPSBaud, SERIAL_8N1, GPS_RX_GPIO, GPS_TX_GPIO);
#      else
  Serial2.begin(GPSBaud, SERIAL_8N1);
#      endif
  GPSStream = &Serial2;
  Log.notice(F("GPS HW UART2" CR));

#    elif GPS_UART == 3 // init UART3
  Serial3.end(); // stop if already initialized
  Serial3.begin(GPSBaud, SERIAL_8N1);
  GPSStream = &Serial3;
  Log.notice(F("GPS HW UART3" CR));
#    endif

#  else // Software serial
  // define pin modes for RX, TX:
  pinMode(GPS_RX_GPIO, INPUT);
  pinMode(GPS_TX_GPIO, OUTPUT);
  GPSSoftSerial.begin(GPSBaud);
  GPSStream = &GPSSoftSerial; // get stream of serial

  Log.notice(F("GPS_RX_GPIO: %d" CR), GPS_RX_GPIO);
  Log.notice(F("GPS_TX_GPIO: %d" CR), GPS_TX_GPIO);
#  endif

  // Flush all bytes in the "link" serial port buffer
  while (GPSStream->available() > 0){
    GPSStream->read();
  }

  Log.notice(F("GPSBaud: %d" CR), GPSBaud);
  Log.trace(F("ZsensorGPS setup done" CR));
}

void MeasureGPS(){
  if (GPSStream->available() > 0 && gps.encode(GPSStream->read())) {
    if (millis() > (timeGPS + TimeBetweenReadingGPS)) {
      timeGPS = millis();
      Log.trace(F("Creating GPS buffer" CR));
      StaticJsonDocument<JSON_MSG_BUFFER> GPSdataBuffer;
      JsonObject GPSdata = GPSdataBuffer.to<JsonObject>();
      //Location
      GPSdata["latitude"] = gps.location.lat();
      GPSdata["longitude"] = gps.location.lng();
     
      //Altitude
      GPSdata["altitude"] = gps.altitude.meters();
     
      //Date
      GPSdata["year"] = gps.date.year();
      GPSdata["month"] = gps.date.month();
      GPSdata["day"] = gps.date.day();
     
      //Time
      GPSdata["hour"] = gps.time.hour();
      GPSdata["minute"] = gps.time.minute();
      GPSdata["second"] = gps.time.second();
      GPSdata["centisecond"] = gps.time.centisecond();
     
      GPSdata["origin"] = subjectGPStoMQTT; 
      handleJsonEnqueue(GPSdata);
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Log.trace("No GPS detected: check wiring.");
  }

}
#endif
