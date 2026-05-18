#include <LoRa.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include <stdio.h>

#include "SSD1306.h"

#define SCK  18 // GPIO18  -- SX1278's SCK
#define MISO 19 // GPIO19 -- SX1278's MISnO
#define MOSI 23 // GPIO23 -- SX1278's MOSI
#define SS   5 // GPIO5 -- SX1278's CS
#define RST  4 // GPIO4 -- SX1278's RESET
#define DI0  26 // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND 433E6
#define NODE_ID 1

unsigned long counter = 0;

SSD1306 display(0x3c, 21, 22);


void setup() {
  pinMode(16, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high

  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println();
  Serial.println("LoRa Sender Test");

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);
  LoRa.setPreambleLength(8);
  LoRa.enableCrc();
  LoRa.setTxPower(14);
  

  Serial.println("init ok");
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  delay(1500);
}

void loop() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Sending packet: ");
  display.drawString(90, 0, String(counter));

  // Build json string to send
  String msg = "{\"node_id\":" + String(NODE_ID) + ",\"msg_id\":" + String(counter) + "}";
  
  for (int i = 0; i < 5; i++) {
    // send packet
    LoRa.beginPacket();
    // Send json string
    LoRa.print(msg);
    LoRa.endPacket();
    delay(50);
  }

  Serial.println(String(msg));
  
  display.drawString(0, 15, String(NODE_ID));
  display.display();

  counter++;
  delay(3000);
}
