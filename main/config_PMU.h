/*  
  OpenMQTTGateway  - ESP8266 or Arduino program for home automation 

   Act as a wifi or ethernet gateway between your 433mhz/infrared IR signal  and a MQTT broker 
   Send and receiving command by MQTT
 
   This files enables to set your parameter for the PMU value
  
    Copyright: (c)Florian ROBERT
  
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
#ifndef config_PMU_h
#define config_PMU_h

#include <Wire.h>
#include "XPowersLib.h"

extern bool setupPMU();
extern void PMUtoMQTT();
extern void MeasurePMU();
/*----------------------------USER PARAMETERS-----------------------------*/
/*-------------DEFINE YOUR MQTT PARAMETERS BELOW----------------*/
#define PMUTOPIC "/PMUtoMQTT"

//#if !defined(TimeBetweenReadingPMU) || (TimeBetweenReadingPMU < 200)
//#  define TimeBetweenReadingPMU 500 // time between 2 PMU readings, minimum 200 to let the time of the ESP to keep the connection
//#endif
//
//#ifndef ThresholdReadingPMU
//#  define ThresholdReadingPMU 50 // following the comparison between the previous value and the current one +- the threshold the value will be published or not
//#endif
//
//#if !defined(NumberOfReadingsPMU) || (NumberOfReadingsPMU < 1)
//#  define NumberOfReadingsPMU 1 // number of readings for better accuracy: avg adc = sum of adc / num readings
//#endif
//
//#ifndef MinTimeInSecBetweenPublishingPMU
//#  define MinTimeInSecBetweenPublishingPMU 0 // pub at least at defined interval - useful to publish values in case they do not change so much ; 0 = disabled
//#endif

/*-------------------PIN DEFINITIONS----------------------*/
#define PMU_IRQ                     35
#endif
