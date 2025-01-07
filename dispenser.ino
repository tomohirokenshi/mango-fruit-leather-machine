// Push Buttons
#define btnA A0
#define btnB A1
#define btnC A2
#define buzzer A3

// Dehydrator Elements
#define heater 2
#define fans 3

// LCD Display
#include <Wire.h>
#include "LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

// Temperature Sensor
#include "DFRobot_SHT20.h"
DFRobot_SHT20 sht20(&Wire, SHT20_I2C_ADDR);
float temp, hum;

// Values
float tempTH;
int mins;
unsigned long timeNow;

void setup() {
  Serial.begin(9600);

  // Initialize LCD
  lcd.init();
  lcd.setCursor(0, 0); lcd.print("  MANGO FRUIT  ");
  lcd.setCursor(0, 1); lcd.print(" LEATHER DRYER ");

  // Initialize SHT20 Sensor
  sht20.initSHT20();
  sht20.checkSHT20();
  delay(2000);
  lcd.clear();
  
  // Pins
  pinMode(heater, OUTPUT);
  pinMode(fans, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(btnA, INPUT_PULLUP);
  pinMode(btnB, INPUT_PULLUP);
  pinMode(btnC, INPUT_PULLUP);
  digitalWrite(heater, LOW);
  digitalWrite(fans, LOW);
  digitalWrite(buzzer, LOW);
}

void loop() {

}
