// Push Buttons
#define btnA A0
#define btnB A1
#define btnC A2

// Mixing and Dispensing Elements
#define wp1 2
#define wp2 3
#define wp3 4
#define sv 5
#define mixer 6

// Load Cell and HX711 Amplifier
#include "HX711.h"
#define DOUT1 8
#define CLK 1 7
#define DOUT2 10
#define CLK 2 9
HX711 scale1;
HX711 scale2;
float calibration_factor = 108320.00;
float weight;
float targetWeight = 0.335;

// LCD Display
#include <Wire.h>
#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
String title = "MANGO FRUIT LEATHER"
String BLANK = "                   "

// Values
int mix = 0;
int qtytray;

void setup() {
  Serial.begin(9600);

  // Initialize Scale
  scale1.begin(DOUT1, CLK1);
  scale1.set_scale();
  scale1.tare();
  long zero_factor1 = scale1.read_average();
  Serial.print("Zero factor1: ");
  Serial.println(zero_factor1);
  scale2.begin(DOUT2, CLK2);
  scale2.set_scale();
  scale2.tare();
  long zero_factor2 = scale2.read_average();
  Serial.print("Zero factor2: ");
  Serial.println(zero_factor2);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0); lcd. print(title);
  lcd.setCursor(0,1); lcd. print("A: Add Ingredients  ");
  lcd.setCursor(0,2); lcd. print("B: Start/Stop Mixing");
  lcd.setCursor(0,3); lcd. print("C: Start Dispensing ");

  // Pins
  pinMode(btnA, INPUT_PULLUP);
  pinMode(btnB, INPUT_PULLUP);
  pinMode(btnC, INPUT_PULLUP);
  pinMode(wp1, OUTPUT);
  pinMode(wp2, OUTPUT);
  pinMode(wp3, OUTPUT);
  pinMode(sv, OUTPUT);
  pinMode(mixer, OUTPUT);
  digitalWrite(wp1, HIGH);
  digitalWrite(wp2, HIGH);
  digitalWrite(wp3, HIGH);
  digitalWrite(sv, LOW);
  digitalWrite(mixer, LOW);
}

void loop() {
  // 1st Step: Add Ingredients
  if (!digitalRead(btnA)) {
    delay(200);
    dispenseFunc();
  }

  // 2nd Step: Start/Stop Mixing
  if (!digitalRead(btnB)) {
    mix = !mix;
    digitalWrite(mixer, mix);
    lcd.setCursor(0, 1); lcd.print(BLANK);
    lcd.setCursor(0, 2); lcd.print(BLANK);
    lcd.setCursor(0, 3); lcd.print(BLANK);
    lcd.setCursor(0, 1); lcd.print("      MIXING...     ");
    lcd.setCursor(0, 2); lcd.print("Tap B to Stop Mixing");
    delay(200);
    unsigned long startTime = millis();
    
    while (true) {
      unsigned long elapsedTime = millis() - startTime;
      unsigned int seconds = (elapsedTime / 1000) % 60;
      unsigned int minutes = (elapsedTime / 60000) % 60;
      lcd.setCursor(0, 3);
      lcd.print(minutes / 10);
      lcd.print(minutes % 10);
      lcd.print(":");
      lcd.print(seconds / 10);
      lcd.print(seconds % 10);
      delay(200);
      
      if (!digitalRead(btnB)) {
        mix = !mix;
        digitalWrite(mixer, mix);
        lcd.setCursor(0, 1); lcd.print("       MIXING       ");
        lcd.setCursor(0, 2); lcd.print("      STOPPED!      ");
        delay(3000);
        displayMainMenu();
        break;
      }
    }
  }

  // 3rd Step: Start Dispensing
  if (!digitalRead(btnC)) {
    delay(200);
    scale1.tare();
    scale2.tare();
    lcd.setCursor(0, 1); lcd.print(BLANK);
    lcd.setCursor(0, 2); lcd.print(BLANK);
    lcd.setCursor(0, 3); lcd.print(BLANK);
    unsigned long timeNow = millis();
    digitalWrite(sv, HIGH);
    while (1) {
      getWeight();
      if (millis() > timeNow + 1000) {
        lcd.setCursor(0, 1); lcd.print("    Dispensing...   ");
        float percentage = (weight / targetWeight) * 100;
        if (percentage < 0) percentage = 0;
        lcd.setCursor(0, 2); lcd.print("  Completed: "); lcd.print(percentage, 1); lcd.print("%");
      }
      if (weight >= targetWeight) {
        lcd.setCursor(0, 2); lcd.print("  Completed: 100%   ");
        lcd.setCursor(0, 3); lcd.print("DISPENSING COMPLETED");
        break;
      }
      if (!digitalRead(btnC)) {
        lcd.setCursor(0, 3); lcd.print(" DISPENSING STOPPED ");
        break;
      }
    }
    digitalWrite(sv, LOW);
    delay(3000);
    displayMainMenu();
  }
}
