// Based on RFM69 library sample sketch, "Gateway" by Felix Rusu - http://LowPowerLab.com/contact
// Copyright Felix Rusu (2015)
// Using Moteino R4 http://lowpowerlab.com

// THE NODE lives in the control box and sends signals to the Gateway, which lives in the light.

#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <RFM69_ATC.h>//get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <SPIFlash.h> //get it here: https://www.github.com/lowpowerlab/spiflash

#define NODEID        2    //must be unique for each node on same network (range up to 254, 255 is used for broadcast)
#define NETWORKID     100  //the same on all nodes that talk to each other (range up to 255)
#define GATEWAYID     1
#define FREQUENCY   RF69_433MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL

#define LED           9 // Moteinos have LEDs on D9
#define FLASH_SS      8 // and FLASH SS on D8

#define SERIAL_BAUD   115200

int TRANSMITPERIOD = 150; //transmit a packet to gateway every... (in ms)
char buff[20];
byte sendSize = 1;
long lastPeriod = 0;
boolean requestACK = false;
SPIFlash flash(FLASH_SS, 0xEF30); //EF30 for 4mbit  Windbond chip (W25X40CL)

#ifdef ENABLE_ATC
RFM69_ATC radio;
#else
RFM69 radio;
#endif

// ****** GOES TO 11 ********* //

#include <avr/sleep.h>
const int knobPin = A1;
const byte powerLedPin = 4;
const byte readPin = 8;
const byte interruptPin = 1;
char payload[] = "A"; // default = "off"



void setup() {
  Serial.begin(SERIAL_BAUD);
  initRadio();

  // ***** GOES TO 11 *****
  pinMode(knobPin, INPUT);
  pinMode(powerLedPin, OUTPUT);
  pinMode(readPin, INPUT);
}

void loop() {
  if (millis() < 1000) sleepNow();
  
  digitalWrite(powerLedPin, !digitalRead(readPin));
  
  if (digitalRead(readPin) == HIGH) {
//    Serial.println("going to sleep now...");
    sleepNow();
  }

  int currPeriod = millis() / TRANSMITPERIOD;
  if (currPeriod != lastPeriod) {
    radio.sendWithRetry(GATEWAYID, payload, sendSize);
    lastPeriod = currPeriod;
  }
}

void wakeUp() {
  payload[0] = 'B';
}


void sleepNow() {
  payload[0] = 'A';
  radio.sendWithRetry(GATEWAYID, payload, sendSize);
  delay(250);
  detachInterrupt(interruptPin);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
  sleep_enable();          // enables the sleep bit in the mcucr register
  attachInterrupt(interruptPin, wakeUp, LOW);
  sleep_mode();
  // *** shhhh ***
  sleep_disable();
}

void initRadio() {
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.encrypt(ENCRYPTKEY);

#ifdef ENABLE_ATC
  radio.enableAutoPower(-90);
#endif

  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

  if (flash.initialize())
  {
    Serial.print("SPI Flash Init OK ... UniqueID (MAC): ");
    flash.readUniqueId();
    for (byte i = 0; i < 8; i++)
    {
      Serial.print(flash.UNIQUEID[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
  else
    Serial.println("SPI Flash MEM not found (is chip soldered?)...");

#ifdef ENABLE_ATC
  Serial.println("RFM69_ATC Enabled (Auto Transmission Control)\n");
#endif
}

