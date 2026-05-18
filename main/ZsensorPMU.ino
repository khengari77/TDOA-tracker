/*
MIT License

Copyright (c) 2022 lewis he

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
! WARN:
Please do not run the example without knowing the external load voltage of the PMU,
it may burn your external load, please check the voltage setting before running the example,
if there is any loss, please bear it by yourself
*/

#include "User_config.h"

#ifdef ZsensorPMU

bool  pmu_flag = 0;
XPowersPMU PMU;

const uint8_t i2c_sda = CONFIG_PMU_SDA;
const uint8_t i2c_scl = CONFIG_PMU_SCL;
const uint8_t pmu_irq_pin = CONFIG_PMU_IRQ;

void setFlag(void)
{
    pmu_flag = true;
}


void setupPMU()
{
    bool result = PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, i2c_sda, i2c_scl);

    if (result == false) {
        Serial.println("PMU is not online..."); while (1)delay(50);
    }

    Serial.printf("getID:0x%x\n", PMU.getChipID());

    // Set the minimum common working voltage of the PMU VBUS input,
    // below this value will turn off the PMU
    PMU.setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_4V36);

    // Set the maximum current of the PMU VBUS input,
    // higher than this value will turn off the PMU
    PMU.setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_1500MA);


    // Get the VSYS shutdown voltage
    uint16_t vol = PMU.getSysPowerDownVoltage();
    Serial.printf("->  getSysPowerDownVoltage:%u\n", vol);

    // Set VSY off voltage as 2600mV , Adjustment range 2600mV ~ 3300mV
    PMU.setSysPowerDownVoltage(2600);

    // It is necessary to disable the detection function of the TS pin on the board
    // without the battery temperature detection function, otherwise it will cause abnormal charging
    PMU.disableTSPinMeasure();

    // PMU.enableTemperatureMeasure();

    // Enable internal ADC detection
    PMU.enableBattDetection();
    PMU.enableVbusVoltageMeasure();
    PMU.enableBattVoltageMeasure();
    PMU.enableSystemVoltageMeasure();
    PMU.enableTemperatureMeasure();

    // DCDC1 1500~3400mV, IMAX=2A
    PMU.setDC1Voltage(3300); // ESP32,  AXP2101 power-on value: 3300

        // ALDO 500~3500V, 100mV/step, IMAX=300mA
    PMU.setButtonBatteryChargeVoltage(3300); // GNSS battery

    PMU.setALDO2Voltage(3300); // LoRa, AXP2101 power-on value: 2800
    PMU.setALDO3Voltage(3300); // GPS,  AXP2101 power-on value: 3300

        // axp_2xxx.enableDC1();
    PMU.enableButtonBatteryCharge();

    PMU.enableALDO2();
    PMU.enableALDO3();

    /*
      The default setting is CHGLED is automatically controlled by the PMU.
    - XPOWERS_CHG_LED_OFF,
    - XPOWERS_CHG_LED_BLINK_1HZ,
    - XPOWERS_CHG_LED_BLINK_4HZ,
    - XPOWERS_CHG_LED_ON,
    - XPOWERS_CHG_LED_CTRL_CHG,
    * */
    PMU.setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);


    // Force add pull-up
    pinMode(pmu_irq_pin, INPUT_PULLUP);
    attachInterrupt(pmu_irq_pin, setFlag, FALLING);


    // Disable all interrupts
    PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    // Clear all interrupt flags
    PMU.clearIrqStatus();
    // Enable the required interrupt function
    PMU.enableIRQ(
        XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
        XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
        XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
        XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ   |    //CHARGE
        XPOWERS_AXP2101_WARNING_LEVEL1_IRQ | XPOWERS_AXP2101_WARNING_LEVEL2_IRQ     //Low battery warning
        // XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | XPOWERS_AXP2101_PKEY_POSITIVE_IRQ   |   //POWER KEY
    );

    // Set the precharge charging current
    PMU.setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_200MA);


    // Set stop charging termination current
    PMU.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);


    // Set constant current charge current limit
    if (!PMU.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_1000MA)) {
        Serial.println("Setting Charger Constant Current Failed!");
    }

    const uint16_t currTable[] = {
        0, 0, 0, 0, 100, 125, 150, 175, 200, 300, 400, 500, 600, 700, 800, 900, 1000
    };
    uint8_t val = PMU.getChargerConstantCurr();
    Serial.print("Setting Charge Target Current : ");
    Serial.println(currTable[val]);

    // Set charge cut-off voltage
    PMU.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V1);

    const uint16_t tableVoltage[] = {
        0, 4000, 4100, 4200, 4350, 4400, 255
    };
    val = PMU.getChargeTargetVoltage();
    Serial.print("Setting Charge Target Voltage : ");
    Serial.println(tableVoltage[val]);


    // Set the power level to be lower than 15% and send an interrupt to the host`
    PMU.setLowBatWarnThreshold(10);

    // Set the power level to be lower than 5% and turn off the power supply
    PMU.setLowBatShutdownThreshold(5);

    PMU.enableOverTemperatureLevel2PowerOff();

    // Set the time of pressing the button to turn off
    PMU.setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
    /*
    Turn on the learning battery curve,
    And write the learned battery curve into the ROM
    */
    PMU.fuelGaugeControl(true, true);

    Serial.println();

    delay(5000);
}

uint32_t printTime = 0;
void MeasurePMU()
{
    if (millis() > printTime) {
      if (PMU.isCharging()) {
        printTime = millis() + TimeBetweenReadingPMU_Charging;
      } else {
        printTime = millis() + TimeBetweenReadingPMU;
      }
      if (PMU.isDropWarningLevel1Irq() || PMU.isDropWarningLevel2Irq()) {
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
      handleJsonEnqueue(PMUdata);
    }

}
#endif

