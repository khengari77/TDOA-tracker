


#ifndef config_PMU_h
#define config_PMU_h

// Defined using AXP2102
#define XPOWERS_CHIP_AXP2101

#include <Wire.h>
#include <Arduino.h>
#include "XPowersLib.h"

extern void setupPMU();
extern void PMUtoMQTT();
extern void MeasurePMU();
/*----------------------------USER PARAMETERS-----------------------------*/
/*-------------DEFINE YOUR MQTT PARAMETERS BELOW----------------*/
#define PMUTOPIC "/PMUtoMQTT"

#if !defined(TimeBetweenReadingPMU) || (TimeBetweenReadingPMU < 200)
#  define TimeBetweenReadingPMU_Charging 500 // time between 2 PMU readings, minimum 200 to let the time of the ESP to keep the connection
#  define TimeBetweenReadingPMU 60000 // time between 2 PMU readings, minimum 200 to let the time of the ESP to keep the connection
#endif

#ifndef CONFIG_PMU_SDA
#define CONFIG_PMU_SDA 21
#endif

#ifndef CONFIG_PMU_SCL
#define CONFIG_PMU_SCL 22
#endif

#ifndef CONFIG_PMU_IRQ
#define CONFIG_PMU_IRQ 35
#endif

#endif

