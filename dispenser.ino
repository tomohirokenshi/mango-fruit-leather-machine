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
#include "LiquidCrystal_I2C.h"
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
  // 1st Step: Set Up Temperature (Minimum = Ambient, Maximum = 70degC)
  while (1) {
    lcd.setCursor(0, 0); lcd.print("A:- B:+ C:DONE");
    lcd.setCursor(0, 1); lcd.print("Temp: "); lcd.print(tempTH);
    if (!digitalRead(btnA)) {
      delay(200);
      tempTH -= 5;
      if (tempTH < 0) {
        tempTH = 0;
      }
    }
    if (!digitalRead(btnB)) {
      delay(200);
      tempTH += 5;
      if (tempTH > 70) {
        tempTH = 70;
      }
    }
    if (!digitalRead(btnC)) {
      delay(200);
      break;
    }
  }
  lcd.clear();

  // 2nd Step: Set Up Duration
  while (1) {
    lcd.setCursor(0, 0); lcd.print("A:- B:+ C:DONE");
    if (mins == 5) {
      lcd.setCursor(0, 1); lcd.print("Time: 05");
    } else {
      lcd.setCursor(0, 1); lcd.print("Time: "); lcd.print(mins);
    }
    if (!digitalRead(btnA)) {
      mins -= 5;
      delay(200);
      if (mins < 0) {
        mins = 0;
      }
    }
    if (!digitalRead(btnB)) {
      mins += 5;
      delay(200);
    }
    if (!digitalRead(btnC)) {
      break;
    }
  }
  delay(500);
  lcd.clear();

  // Turn ON Dehydrator Elements and Buzzer
  digitalWrite(heater, HIGH);
  digitalWrite(fans, HIGH);
  preHeat();
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  lcd.clear();
  showTimer(mins);
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  digitalWrite(heater, LOW);
  digitalWrite(fans, LOW);
}

// Preheat Function
void preHeat() {
  timeNow = millis();
  lcd.clear();
  lcd.setCursor(0, 1); lcd.print("T: "); lcd.print(temp, 2); lcd.print("C H: "); lcd.print(hum, 0); lcd.print("%");
  lcd.setCursor(0, 0); lcd.print("PREHEATING");
  while (1) {
    if (millis() > timeNow + 1000) {
      temp = sht20.readTemperature();
      hum = sht20.readHumidity();
      lcd.setCursor(0, 1); lcd.print("T: "); lcd.print(temp, 2); lcd.print("C H: "); lcd.print(hum, 0); lcd.print("%");
      lcd.setCursor(0, 0); lcd.print("PREHEATING");
      timeNow = millis();
    }
    if (temp >= tempTH) {
      lcd.clear();
      break;
    }
    if (!digitalRead(btnC)) {
      delay(200);
      break;
    }
  } 
}

// Initialize Timer after the Desired Temperature is reached
void showTimer(int Minutes) {
  int Hours = floor(Minutes / 60);
  Minutes = Minutes - (Hours * 60);
  unsigned long lastTimerUpdate = millis();
  int Seconds = 0;
  lcd.clear();
  timeNow = millis();
  while (1) {
    if (millis() > timeNow + 1000) {
      int secondsPassed = (millis() - lastTimerUpdate) / 1000;
      Seconds = Seconds - secondsPassed;
      lastTimerUpdate = lastTimerUpdate + (secondsPassed * 1000);
      temp = sht20.readTemperature();
      hum = sht20.readHumidity();
      timeNow = millis();
    }
    String sh = String(Hours);
    String sm = String(Minutes);
    String ss = String(Seconds);
    if (Hours < 10) {
      sh = "0" + sh;
    }
    if (Minutes < 10) {
      sm = "0" + sm;
    }
    if (Seconds < 10) {
      ss = "0" + ss;
    }
    String line1 = sh + ":" + sm + ":" + ss;

    // Temperature Balance Function
    if (temp > tempTH) {
      digitalWrite(heater, LOW);
    }
    if (temp < tempTH - 5) {
      digitalWrite(heater, HIGH);
    }

    // Display Values to LCD
    lcd.setCursor(0, 0); lcd.print("T: "); lcd.print(temp, 2); lcd.print("C H: "); lcd.print(hum, 0); lcd.print("%");
    lcd.setCursor(0, 1); lcd.print(line1); lcd.print(" ");
    if (Seconds < 0) {
      Seconds = 59;
      Minutes --;
      if (Minutes < 0) {
        Minutes = 59;
        Hours --;
      }
    }
    if (Hours < 0) {
      timeNow = millis();
      while (1) {
        lcd.setCursor(0, 0); lcd.print("DRYING DONE.  ");
        lcd.setCursor(0, 1); lcd.print("DRYING DONE.  ");
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        digitalWrite(heater, LOW);
        digitalWrite(fans, LOW);
        break;
        if (!digitalRead(btnA) || !digitalRead(btnB) || !digitalRead(btnC)){
          lcd.setCursor(0, 0); lcd.print("THANK YOU FOR   ");
          lcd.setCursor(0, 1); lcd.print("USING THE DEVICE");
          delay(2000);
          break;
        }
      }
      lcd.clear();
      break;
    }
  }
}
