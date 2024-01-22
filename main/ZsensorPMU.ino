/*  
  OpenMQTTGateway Addon  - ESP8266 or Arduino program for home automation 

   Act as a wifi or ethernet gateway between your 433mhz/infrared IR signal  and a MQTT broker 
   Send and receiving command by MQTT
 
    Analog GPIO reading Addon
  
    Copyright: (c)Florian ROBERT
    
    Contributors:
    - 1technophile
  
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

#ifdef ZsensorPMU

XPowersLibInterface *PMU = NULL;

#ifndef PMU_WIRE_PORT
#define PMU_WIRE_PORT   Wire
#endif

bool pmuInterrupt;

void setPmuFlag()
{
    pmuInterrupt = true;
}


bool setupPMU()
{
    if (!PMU) {
        PMU = new XPowersAXP2101(PMU_WIRE_PORT);
        if (!PMU->init()) {
            Log.error("Warning: Failed to find AXP2101 power management");
            delete PMU;
            PMU = NULL;
        } else {
            Log.trace("AXP2101 PMU init succeeded, using AXP2101 PMU");
        }
    }

    PMU->setChargingLedMode(XPOWERS_CHG_LED_BLINK_1HZ);

    pinMode(PMU_IRQ, INPUT_PULLUP);
    attachInterrupt(PMU_IRQ, setPmuFlag, FALLING);

    if (PMU->getChipModel() == XPOWERS_AXP2101) {

#if defined(CONFIG_IDF_TARGET_ESP32)
        //Unuse power channel
        PMU->disablePowerOutput(XPOWERS_DCDC2);
        PMU->disablePowerOutput(XPOWERS_DCDC3);
        PMU->disablePowerOutput(XPOWERS_DCDC4);
        PMU->disablePowerOutput(XPOWERS_DCDC5);
        PMU->disablePowerOutput(XPOWERS_ALDO1);
        PMU->disablePowerOutput(XPOWERS_ALDO4);
        PMU->disablePowerOutput(XPOWERS_BLDO1);
        PMU->disablePowerOutput(XPOWERS_BLDO2);
        PMU->disablePowerOutput(XPOWERS_DLDO1);
        PMU->disablePowerOutput(XPOWERS_DLDO2);

        // GNSS RTC PowerVDD 3300mV
        PMU->setPowerChannelVoltage(XPOWERS_VBACKUP, 3300);
        PMU->enablePowerOutput(XPOWERS_VBACKUP);

        //ESP32 VDD 3300mV
        // ! No need to set, automatically open , Don't close it
        // PMU->setPowerChannelVoltage(XPOWERS_DCDC1, 3300);
        // PMU->setProtectedChannel(XPOWERS_DCDC1);
        PMU->setProtectedChannel(XPOWERS_DCDC1);

        // LoRa VDD 3300mV
        PMU->setPowerChannelVoltage(XPOWERS_ALDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO2);

        //GNSS VDD 3300mV
        PMU->setPowerChannelVoltage(XPOWERS_ALDO3, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO3);

#endif /*CONFIG_IDF_TARGET_ESP32*/


#if defined(LILYGO_TBeamS3_SUPREME_V3_0)

        //t-beam m.2 inface
        //gps
        PMU->setPowerChannelVoltage(XPOWERS_ALDO4, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO4);

        // lora
        PMU->setPowerChannelVoltage(XPOWERS_ALDO3, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO3);

        // In order to avoid bus occupation, during initialization, the SD card and QMC sensor are powered off and restarted
        if (ESP_SLEEP_WAKEUP_UNDEFINED == esp_sleep_get_wakeup_cause()) {
            Log.trace("Power off and restart ALDO BLDO..");
            PMU->disablePowerOutput(XPOWERS_ALDO1);
            PMU->disablePowerOutput(XPOWERS_ALDO2);
            PMU->disablePowerOutput(XPOWERS_BLDO1);
            delay(250);
        }

        // Sensor
        PMU->setPowerChannelVoltage(XPOWERS_ALDO1, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO1);

        PMU->setPowerChannelVoltage(XPOWERS_ALDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_ALDO2);

        //Sdcard

        PMU->setPowerChannelVoltage(XPOWERS_BLDO1, 3300);
        PMU->enablePowerOutput(XPOWERS_BLDO1);

        PMU->setPowerChannelVoltage(XPOWERS_BLDO2, 3300);
        PMU->enablePowerOutput(XPOWERS_BLDO2);

        //face m.2
        PMU->setPowerChannelVoltage(XPOWERS_DCDC3, 3300);
        PMU->enablePowerOutput(XPOWERS_DCDC3);

        PMU->setPowerChannelVoltage(XPOWERS_DCDC4, XPOWERS_AXP2101_DCDC4_VOL2_MAX);
        PMU->enablePowerOutput(XPOWERS_DCDC4);

        PMU->setPowerChannelVoltage(XPOWERS_DCDC5, 3300);
        PMU->enablePowerOutput(XPOWERS_DCDC5);


        //not use channel
        PMU->disablePowerOutput(XPOWERS_DCDC2);
        // PMU->disablePowerOutput(XPOWERS_DCDC4);
        // PMU->disablePowerOutput(XPOWERS_DCDC5);
        PMU->disablePowerOutput(XPOWERS_DLDO1);
        PMU->disablePowerOutput(XPOWERS_DLDO2);
        PMU->disablePowerOutput(XPOWERS_VBACKUP);

        // Set constant current charge current limit
        PMU->setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_500MA);

        // Set charge cut-off voltage
        PMU->setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V2);

        // Disable all interrupts
        PMU->disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
        // Clear all interrupt flags
        PMU->clearIrqStatus();
        // Enable the required interrupt function
        PMU->enableIRQ(
            XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
            XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
            XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
            XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
            // XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | XPOWERS_AXP2101_PKEY_POSITIVE_IRQ   |   //POWER KEY
        );

#endif
    }
    PMU->enableSystemVoltageMeasure();
    PMU->enableVbusVoltageMeasure();
    PMU->enableBattVoltageMeasure();
    PMU->disableTSPinMeasure();

    Log.trace("=========================================\n");
    if (PMU->isChannelAvailable(XPOWERS_DCDC1)) {
        Log.trace(F("DC1  : %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_DCDC1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC1));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC2)) {
        Log.trace(F("DC2  : %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_DCDC2)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC2));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC3)) {
        Log.trace(F("DC3  : %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_DCDC3)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC3));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC4)) {
        Log.trace(F("DC4  : %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_DCDC4)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC4));
    }
    if (PMU->isChannelAvailable(XPOWERS_DCDC5)) {
        Log.trace(F("DC5  : %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_DCDC5)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC5));
    }
    if (PMU->isChannelAvailable(XPOWERS_LDO2)) {
        Log.trace(F("LDO2 : %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_LDO2)   ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_LDO2));
    }
    if (PMU->isChannelAvailable(XPOWERS_LDO3)) {
        Log.trace(F("LDO3 : %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_LDO3)   ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_LDO3));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO1)) {
        Log.trace(F("ALDO1: %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_ALDO1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO1));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO2)) {
        Log.trace(F("ALDO2: %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_ALDO2)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO2));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO3)) {
        Log.trace(F("ALDO3: %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_ALDO3)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO3));
    }
    if (PMU->isChannelAvailable(XPOWERS_ALDO4)) {
        Log.trace(F("ALDO4: %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_ALDO4)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_ALDO4));
    }
    if (PMU->isChannelAvailable(XPOWERS_BLDO1)) {
        Log.trace(F("BLDO1: %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_BLDO1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_BLDO1));
    }
    if (PMU->isChannelAvailable(XPOWERS_BLDO2)) {
        Log.trace(F("BLDO2: %s   Voltage: %04u mV \n"),  PMU->isPowerChannelEnable(XPOWERS_BLDO2)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_BLDO2));
    }
    Log.trace("=========================================\n");


    // Set the time of pressing the button to turn off
    PMU->setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
    uint8_t opt = PMU->getPowerKeyPressOffTime();
    Log.trace("PowerKeyPressOffTime:");
    switch (opt) {
    case XPOWERS_POWEROFF_4S: Log.trace("4 Second");
        break;
    case XPOWERS_POWEROFF_6S: Log.trace("6 Second");
        break;
    case XPOWERS_POWEROFF_8S: Log.trace("8 Second");
        break;
    case XPOWERS_POWEROFF_10S: Log.trace("10 Second");
        break;
    default:
        break;
    }

    return true;
}

void disablePeripherals()
{
}


void MeasurePMU() {}

//static int persistedadc = -1024; // so that the first reading will always be published
//
////Time used to wait for an interval before resending adc value
//unsigned long timeadc = 0;
//unsigned long timeadcpub = 0;

//void MeasurePMU() {
//  if (millis() - timeadc > TimeBetweenReadingPMU) { //retrieving value of temperature and humidity of the box from DHT every xUL
//#  if defined(ESP8266)
//    yield();
//    analogRead(PMU_GPIO); //the reliability of the readings can be significantly improved by discarding an initial reading and then averaging the results
//#  endif
//    timeadc = millis();
//    int val = analogRead(PMU_GPIO);
//    int sum_val = val;
//    if (NumberOfReadingsPMU > 1) { // add extra measurement for accuracy
//      for (int i = 1; i < NumberOfReadingsPMU; i++) {
//        sum_val += analogRead(PMU_GPIO);
//      }
//      val = sum_val / NumberOfReadingsPMU;
//    }
//    if (isnan(val)) {
//      Log.error(F("Failed to read from PMU !" CR));
//    } else {
//      if (val >= persistedadc + ThresholdReadingPMU || val <= persistedadc - ThresholdReadingPMU || (MinTimeInSecBetweenPublishingPMU > 0 && millis() - timeadcpub > (MinTimeInSecBetweenPublishingPMU * 1000UL))) {
//        timeadcpub = millis();
//        Log.trace(F("Creating PMU buffer" CR));
//        StaticJsonDocument<JSON_MSG_BUFFER> PMUdataBuffer;
//        JsonObject PMUdata = PMUdataBuffer.to<JsonObject>();
//        PMUdata["adc"] = (int)val;
//        if (NumberOfReadingsPMU > 1) {
//          PMUdata["adc_reads"] = NumberOfReadingsPMU;
//        }
//#  if defined(PMU_DIVIDER)
//        float volt = 0;
//#    if defined(ESP32)
//        // Convert the analog reading (which goes from 0 - 4095) to a voltage (0 - 3.3V):
//        volt = val * (3.3 / 4096.0);
//#    elif defined(ESP8266)
//        // Convert the analog reading (which goes from 0 - 1024) to a voltage (0 - 3.3V):
//        volt = val * (3.3 / 1024.0);
//#    else
//        // Asume 5V and 10bits PMU
//        volt = val * (5.0 / 1024.0);
//#    endif
//        volt *= PMU_DIVIDER;
//        // let's give 2 decimal point
//        int v = (volt * 100);
//        volt = (float)v / 100.0;
//        PMUdata["volt"] = (float)volt;
//#  endif
//#  if defined(PMU_CALIBRATED_SCALE_FACTOR) // calibration: scale factor = ref-Vcal-mV / sum-of-cal-readings @see https://skillbank.co.uk/arduino/calibrate.htm
//        float voltage = sum_val * PMU_CALIBRATED_SCALE_FACTOR; // convert to voltage in millivolts
//        PMUdata["adc_sum"] = (int)sum_val;
//        PMUdata["mvolt_scaled"] = (int)voltage; // real scaled/calibrated value in mV
//#  endif
//        PMUdata["origin"] = PMUTOPIC;
//        handleJsonEnqueue(PMUdata);
//        persistedadc = val;
//      }
//    }
//  }
//}
#endif
