#include <SD.h>
#include <SPI.h>

//SPI Pins
#define CS0   10
#define MOSI0 11
#define MISO0 12
#define SCK0  13

//PMod Pins
#define JXADC0 0
#define JXADC1 1
#define JXADC2 2
#define JXADC3 3
#define JXADC4 4
#define JXADC5 5
#define JXADC6 6
#define JXADC7 7

//Sensor Pins
#define pot   A0

//ISR Pins
#define ISRPin 8

File pot_data;

int logging = 0;

//byte pot_val;

int serial_input = 0;

byte JXADC[] = {JXADC0, JXADC1, JXADC2, JXADC3, JXADC4, JXADC5, JXADC6, JXADC7};
const byte numPins = 8;

void setup() {
  
      delay(1000);
    
      //Configure SPI Pins
      SPI.setCS(CS0);
      SPI.setMOSI(MOSI0);
      SPI.setMISO(MISO0);
      SPI.setSCK(SCK0);
    
      pinMode(ISRPin, INPUT);

      pinMode(JXADC0, OUTPUT);
      pinMode(JXADC1, OUTPUT);
      pinMode(JXADC2, OUTPUT);
      pinMode(JXADC3, OUTPUT);
      pinMode(JXADC4, OUTPUT);
      pinMode(JXADC5, OUTPUT);
      pinMode(JXADC6, OUTPUT);
      pinMode(JXADC7, OUTPUT);
    
      //Port Registers
      //DDRD = B11111111;
    
    
      Serial.begin(9600);
      //Serial1.begin(9600);
    
      Serial.print("Initializing SD card...");
    
      if (!SD.begin(CS0)) {
        Serial.println("initialization failed!");
        return;
      }
      Serial.println("initialization done.");
    
      //ISR triggers
      //attachInterrupt(digitalPinToInterrupt(ISRPin), logDataOn, RISING);
      //attachInterrupt(digitalPinToInterrupt(ISRPin), logDataOff, FALLING);
      attachInterrupt(digitalPinToInterrupt(ISRPin), logDataToggle, FALLING);
    
      //Initialize SD card file
      pot_data = SD.open("pot_data.csv", FILE_WRITE);
      if(pot_data) {
        pot_data.println("********************************");
        pot_data.close();
        Serial.println("SD card file initialized!");
      }
      else {
        Serial.println("Error initializing pot_data.csv");
      }
      
}

void loop() {


      //Serial input used as button to test code
      if(Serial.available() > 0) {
        serial_input = Serial.read();
      }
    
      if(serial_input == 49) {
        logDataOn();
        serial_input = 0;
      }
      else if(serial_input == 48) {
        logDataOff();
        serial_input = 0;
      }
    
      //Log 10-bit pot value (in %) to SD card
      if(logging == 1) {
        pot_data.println(analogRead(pot));
      }
    
      //Create 8-bit pot value
      byte num = map(analogRead(pot),0,1023,0,255);
      //Serial.println(num, BIN);
    
      //Send 8-bit pot value to Basys 3 via UART
      //Serial1.write(num);
    
    
      //Send 8-bit pot value to Basys 3 via PMod
      //PORTD = num
      for (byte i=0; i < numPins; i++) {
        byte state = bitRead(num, i);
        digitalWrite(JXADC[i], state);
        //Serial.print(state);
      }
      //Serial.println();
      
      delay(500);
      
}

void logDataToggle() {
  if (logging == 0) {
    pot_data = SD.open("pot_data.csv", FILE_WRITE);
    pot_data.println("*****New Data Entry*****");
    if(pot_data) {
      logging = 1;
      Serial.print("Logging data...");
    }
    else {
      Serial.println("Error opening pot_data.csv");
    }
    return;
  }
  else if (logging == 1) {
    logging = 0;
    pot_data.close();
    Serial.println("logging complete. Safe to remove SD card.");
    return;
  }
}


void logDataOn() {
  pot_data = SD.open("pot_data.csv", FILE_WRITE);
  pot_data.println("*****New Data Entry*****");
  if(pot_data) {
    logging = 1;
    Serial.print("Logging data...");
  }
  else {
    Serial.println("Error opening pot_data.csv");
  }
  return;
}

void logDataOff() {
  logging = 0;
  pot_data.close();
  Serial.println("logging complete. Safe to remove SD card.");
  return;
}
