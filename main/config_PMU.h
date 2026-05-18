/*
  Theengs OpenMQTTGateway - We Unite Sensors in One Open-Source Interface

   Act as a gateway between your 433mhz, infrared IR, BLE, LoRa signal and one interface like an MQTT broker
   Send and receiving command by MQTT

   This files enables to set your parameter for the PMU sensor (AXP2101)

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
#ifndef config_PMU_h
#define config_PMU_h

#define XPOWERS_CHIP_AXP2101

#include "XPowersLib.h"

extern void setupPMU();
extern void MeasurePMU();

/*----------------------------USER PARAMETERS-----------------------------*/
#define PMUTOPIC "/PMUtoMQTT"

#if !defined(TimeBetweenReadingPMU) || (TimeBetweenReadingPMU < 200)
#  define TimeBetweenReadingPMU_Charging 500
#  define TimeBetweenReadingPMU 60000
#endif

#ifndef CONFIG_PMU_SDA
#  define CONFIG_PMU_SDA 21
#endif

#ifndef CONFIG_PMU_SCL
#  define CONFIG_PMU_SCL 22
#endif

#ifndef CONFIG_PMU_IRQ
#  define CONFIG_PMU_IRQ 35
#endif

#endif
