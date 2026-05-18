/*
  Theengs OpenMQTTGateway - We Unite Sensors in One Open-Source Interface

   Act as a gateway between your 433mhz, infrared IR, BLE, LoRa signal and one interface like an MQTT broker
   Send and receiving command by MQTT

   PMU (Power Management Unit) sensor module for AXP2101

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

#ifdef ZsensorPMU
#  include "TheengsCommon.h"
#  include "config_PMU.h"
#  include <Wire.h>
#  include "XPowersLib.h"

bool pmuIrqReceived = false;
XPowersPMU PMU;
unsigned long pmuTime = 0;

void setPmuFlag() {
  pmuIrqReceived = true;
}

void setupPMU() {
  THEENGS_LOG_NOTICE(F("Setting up PMU" CR));
  if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, CONFIG_PMU_SDA, CONFIG_PMU_SCL)) {
    THEENGS_LOG_ERROR(F("PMU not found" CR));
    return;
  }

  THEENGS_LOG_NOTICE(F("PMU chip ID: 0x%x" CR), PMU.getChipID());

  PMU.setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_4V36);
  PMU.setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_1500MA);
  PMU.setSysPowerDownVoltage(2600);
  PMU.disableTSPinMeasure();

  PMU.enableBattDetection();
  PMU.enableVbusVoltageMeasure();
  PMU.enableBattVoltageMeasure();
  PMU.enableSystemVoltageMeasure();
  PMU.enableTemperatureMeasure();

  PMU.setDC1Voltage(3300);
  PMU.setButtonBatteryChargeVoltage(3300);
  PMU.setALDO2Voltage(3300);
  PMU.setALDO3Voltage(3300);
  PMU.enableButtonBatteryCharge();
  PMU.enableALDO2();
  PMU.enableALDO3();

  PMU.setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);

  pinMode(CONFIG_PMU_IRQ, INPUT_PULLUP);
  attachInterrupt(CONFIG_PMU_IRQ, setPmuFlag, FALLING);

  PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
  PMU.clearIrqStatus();
  PMU.enableIRQ(
    XPOWERS_AXP2101_BAT_INSERT_IRQ | XPOWERS_AXP2101_BAT_REMOVE_IRQ |
    XPOWERS_AXP2101_VBUS_INSERT_IRQ | XPOWERS_AXP2101_VBUS_REMOVE_IRQ |
    XPOWERS_AXP2101_PKEY_SHORT_IRQ | XPOWERS_AXP2101_PKEY_LONG_IRQ |
    XPOWERS_AXP2101_BAT_CHG_DONE_IRQ | XPOWERS_AXP2101_BAT_CHG_START_IRQ |
    XPOWERS_AXP2101_WARNING_LEVEL1_IRQ | XPOWERS_AXP2101_WARNING_LEVEL2_IRQ
  );

  PMU.setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_200MA);
  PMU.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);
  if (!PMU.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_1000MA)) {
    THEENGS_LOG_ERROR(F("Setting charger constant current failed" CR));
  }
  PMU.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V1);
  PMU.setLowBatWarnThreshold(10);
  PMU.setLowBatShutdownThreshold(5);
  PMU.enableOverTemperatureLevel2PowerOff();
  PMU.setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
  PMU.fuelGaugeControl(true, true);

  THEENGS_LOG_NOTICE(F("PMU setup done" CR));
}

void MeasurePMU() {
  if (pmuIrqReceived) {
    pmuIrqReceived = false;
  }

  if (millis() > pmuTime) {
    if (PMU.isCharging()) {
      pmuTime = millis() + TimeBetweenReadingPMU_Charging;
    } else {
      pmuTime = millis() + TimeBetweenReadingPMU;
    }

    if (PMU.isDropWarningLevel1Irq() || PMU.isDropWarningLevel2Irq()) {
      THEENGS_LOG_WARNING(F("Low battery warning, shutting down" CR));
      PMU.shutdown();
    }

    StaticJsonDocument<JSON_MSG_BUFFER> PMUdataBuffer;
    JsonObject PMUdata = PMUdataBuffer.to<JsonObject>();

    PMUdata["status"] = PMU.status();
    PMUdata["is_charging"] = PMU.isCharging();
    PMUdata["is_discharge"] = PMU.isDischarge();
    PMUdata["is_standby"] = PMU.isStandby();
    PMUdata["is_vbus_in"] = PMU.isVbusIn();
    PMUdata["is_vbus_good"] = PMU.isVbusGood();
    PMUdata["battery_voltage"] = PMU.getBattVoltage();
    PMUdata["vbus_voltage"] = PMU.getVbusVoltage();
    PMUdata["system_voltage"] = PMU.getSystemVoltage();
    PMUdata["battery_percentage"] = PMU.getBatteryPercent();
    PMUdata["charger_status"] = PMU.getChargerStatus();
    PMUdata["temperature"] = PMU.getTemperature();
    PMUdata["irq_status"] = PMU.getIrqStatus();
    PMUdata["is_drop_warning_level2"] = PMU.isDropWarningLevel2Irq();
    PMUdata["is_drop_warning_level1"] = PMU.isDropWarningLevel1Irq();
    PMUdata["is_gauge_wdt_timeout"] = PMU.isGaugeWdtTimeoutIrq();
    PMUdata["is_bat_charger_over_temperature"] = PMU.isBatChargerOverTemperatureIrq();
    PMUdata["is_bat_work_over_temperature"] = PMU.isBatWorkOverTemperatureIrq();
    PMUdata["current_limit"] = PMU.getVbusCurrentLimit();
    PMUdata["origin"] = PMUTOPIC;
    enqueueJsonObject(PMUdata);
  }
}

#endif
