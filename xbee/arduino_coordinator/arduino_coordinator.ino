
/* 
 *  Goes To Eleven 
 *  Matthew Epler for Human Condition Global, 2016
 *  Do not use without permission
 *  
 *  This code is for the Controller XBee which lives inside the light housing.
 *  The Router unit at the control box does not require a microcontroller. 
 *  See git repo README for full documentation. 
 *  LINK = [TBD]
*/

#include <SoftwareSerial.h>
SoftwareSerial mySerial (2, 3);

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define LEDPIN         11
#define NUMPIXELS      16
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

byte readValue = 0;
int localLED = 10;


void setup() {
  mySerial.begin(9600);
  pinMode(localLED, OUTPUT); // for debug only. not used in prod. env. 

  pixels.setBrightness(255);
  pixels.begin(); 
}

void loop() {
    if (mySerial.available() > 21) {
      if (mySerial.peek() == 0x7e) {
        for (int i = 0; i < 22; i++) {
  
          mySerial.print(i);
          mySerial.print(":");
          mySerial.print(Serial.read(), HEX);
          mySerial.print(", ");
  
          if (i == 19) {
            readValue = mySerial.read();
          }
        }
        
        mySerial.print("-> "); // end of the line will show you the info you really need - state of switch. 
        mySerial.print(readValue, HEX);
        if (readValue == 0) {
          ledChange(127); // set neopixels to RGB 127, 127, 127
          mySerial.print(" Goes to 11!");
          digitalWrite(localLED, HIGH);
          //setRemoteLED(0x5); for feedback on knob box. unnecessary but keeping so i can see how to do it later if need be
        } else if (readValue == 0x10) {
          ledChange(0); // set neopixels to RGB 0, 0, 0
          mySerial.print(" Turn it up!");
          digitalWrite(localLED, LOW);
          //setRemoteLED(0x4); unnecessary but keeping so i can see how to do it later if need be
        }
        mySerial.println();
      }
    }
}

void ledChange(int val) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(val, val, val));
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}



// Unnecessary but keeping so i can see how to do it later if need be

//void setRemoteLED(char value) {
//  Serial.write(0x7e);       // start byte
//  Serial.write((byte)0x0);  // high part of length (always zero)
//  Serial.write(0x10);       // low part of length, # of bytes to follow not includeing checksum
//  Serial.write(0x17);       // 0x17 is a remote AT command
//  Serial.write((byte)0x0);  // frame id set to zero for no reply
//
//  // ID of receipient, or use 0x000000000000FFF for broadcast
//  Serial.write((byte)00);
//  Serial.write((byte)00);
//  Serial.write((byte)00);
//  Serial.write((byte)00);
//  Serial.write((byte)00);
//  Serial.write((byte)00);
//  Serial.write(0xFF);
//  Serial.write(0xFF);
//
//  // 16 bit Destination Address or 0xFFFE if unknown
//  Serial.write(0xFF);
//  Serial.write(0xFE);
//
//  Serial.write(0x02); // oxo2 to apply changes immediately on remote
//
//  // caommand name in ASCII characters
//  Serial.write('D');
//  Serial.write('3');
//
//  // command data in as many bytes as needed
//  Serial.write(value);
//
//  // checksum is all bytes after length bytes (byte)00 = 0, so not included.
//  long sum = 0x17 + 0xFF + 0xFF + 0xFF + 0xFE + 0x02 + 'D' + '3' + value;
//  Serial.write( 0xFF - ( sum & 0xFF) ); // calculate the proper checksum
//  Serial.flush();
//}

