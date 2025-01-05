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

// Dispensing Function
void dispenseFunc() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("How many tray? "); lcd.print(qtytray);
  lcd.setCursor(0, 1); lcd.print("A: -1 Tray Mix Ratio");
  lcd.setCursor(0, 2); lcd.print("B: +1 Tray Mix Ratio");
  lcd.setCursor(0, 3); lcd.print("C: Confirm Selection");
  while (1) {
    lcd.setCursor(0, 0); lcd.print("How many tray? "); lcd.print(qtytray);
    if (!digitalRead(btnA)) {
      delay(200);
      qtytray -= 1;
      if (qtytray < 0) {
        qtytray = 0;
      }
    }
    if (!digitalRead(btnB)) {
      delay(200);
      qtytray += 1;
      if (qtytray > 5) {
        qtytray = 5;
      }
    }
    if (!digitalRead(btnC)) {
      if (qtytray == 0) {
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print(title);
        primerTubes();
        break;
      }
      else {
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print(title);
        addIngredients();
        break;
      }
    }
  }
  delay(500);
  displayMainMenu();
}

// Primer Tubes
void primerTubes() {
  long totalDelay = 16000L + 6500L + 6800L;
  long elapsedTime = 0;
  long stepDelay = 100;
  long numSteps;
  long stepElapsedTime;

  digitalWrite(wp1, LOW);
  numSteps = 6500L / stepDelay;
  stepElapsedTime = 6500L / numSteps;
  for (long i = 0; i < numSteps; i++) {
    elapsedTime += stepElapsedTime;
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(title);
    lcd.setCursor(0, 1); lcd.print("      Priming...    ");
    lcd.setCursor(0, 2); lcd.print("  Lemon Juice Tube  ");
    lcd.setCursor(0, 3); lcd.print(" ");
    lcd.setCursor(8, 3); lcd.print((elapsedTime * 100) / totalDelay);
    lcd.setCursor(11, 3); lcd.print("%");
    delay(stepDelay);
  }
  digitalWrite(wp1, HIGH);

  digitalWrite(wp2, LOW);
  numSteps = 16000L / stepDelay;
  stepElapsedTime = 16000L / numSteps;
  for (long i = 0; i < numSteps; i++) {
    elapsedTime += stepElapsedTime;
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(title);
    lcd.setCursor(0, 1); lcd.print("      Priming...    ");
    lcd.setCursor(0, 2); lcd.print("     Pectin Tube    ");
    lcd.setCursor(0, 3); lcd.print(" ");
    lcd.setCursor(8, 3); lcd.print((elapsedTime * 100) / totalDelay);
    lcd.setCursor(11, 3); lcd.print("%");
    delay(stepDelay);
  }
  digitalWrite(wp2, HIGH);

  digitalWrite(wp3, LOW);
  numSteps = 6800L / stepDelay;
  stepElapsedTime = 6800L / numSteps;
  for (long i = 0; i < numSteps; i++) {
    elapsedTime += stepElapsedTime;
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(title);
    lcd.setCursor(0, 1); lcd.print("      Priming...    ");
    lcd.setCursor(0, 2); lcd.print("   Sweetener Tube   ");
    lcd.setCursor(0, 3); lcd.print(" ");
    lcd.setCursor(8, 3); lcd.print((elapsedTime * 100) / totalDelay);
    lcd.setCursor(11, 3); lcd.print("%");
    delay(stepDelay);
  }
  digitalWrite(wp3, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(title);
  lcd.setCursor(0, 1); lcd.print("  PRIMING OF TUBES  ");
  lcd.setCursor(0, 2); lcd.print("      COMPLETE      ");
  lcd.setCursor(0, 3); lcd.print(" ");
  lcd.setCursor(8, 3); lcd.print((elapsedTime * 100) / totalDelay);
  lcd.setCursor(11, 3); lcd.print("%");
  delay(3000);
}

// Add Ingredients
void addIngredients() {
  long totalDelay = qtytray * (20000L + 6000L + 39500L);
  long elapsedTime = 0;
  long stepDelay = 100;
  long numSteps;
  long stepElapsedTime;

  digitalWrite(wp1, LOW);
  numSteps = (6000L * qtytray) / stepDelay;
  stepElapsedTime = (6000L * qtytray) / numSteps;
  for (long i = 0; i < numSteps; i++) {
    elapsedTime += stepElapsedTime;
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(title);
    lcd.setCursor(0, 1); lcd.print("    Dispensing...    ");
    lcd.setCursor(0, 2); lcd.print("     Lemon Juice     ");
    lcd.setCursor(0, 3); lcd.print(" ");
    lcd.setCursor(8, 3); lcd.print((elapsedTime * 100) / totalDelay);
    lcd.setCursor(11, 3); lcd.print("%");
    delay(stepDelay);
  }
  digitalWrite(wp1, HIGH);

  digitalWrite(wp2, LOW);
  numSteps = (qtytray * 20000L) / stepDelay;
  stepElapsedTime = (qtytray * 20000L) / numSteps;
  for (long i = 0; i < numSteps; i++) {
    elapsedTime += stepElapsedTime;
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(title);
    lcd.setCursor(0, 1); lcd.print("    Dispensing...    ");
    lcd.setCursor(0, 2); lcd.print("        Pectin       ");
    lcd.setCursor(0, 3); lcd.print(" ");
    lcd.setCursor(8, 3); lcd.print((elapsedTime * 100) / totalDelay);
    lcd.setCursor(11, 3); lcd.print("%");
    delay(stepDelay);
  }
  digitalWrite(wp2, HIGH);

  digitalWrite(wp3, LOW);
  numSteps = (qtytray * 39500L) / stepDelay;
  stepElapsedTime = (qtytray * 39500L) / numSteps;
  for (long i = 0; i < numSteps; i++) {
    elapsedTime += stepElapsedTime;
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(title);
    lcd.setCursor(0, 1); lcd.print("    Dispensing...    ");
    lcd.setCursor(0, 2); lcd.print("      Sweetener      ");
    lcd.setCursor(0, 3); lcd.print(" ");
    lcd.setCursor(8, 3); lcd.print((elapsedTime * 100) / totalDelay);
    lcd.setCursor(11, 3); lcd.print("%");
    delay(stepDelay);
  }
  digitalWrite(wp3, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(title);
  lcd.setCursor(0, 1); lcd.print(" ADDING INGREDIENTS ");
  lcd.setCursor(0, 2); lcd.print("      COMPLETE      ");
  lcd.setCursor(0, 3); lcd.print(" ");
  lcd.setCursor(8, 3); lcd.print((elapsedTime * 100) / totalDelay);
  lcd.setCursor(11, 3); lcd.print("%");
  delay(3000);
}

// Display Main Menu
void displayMainMenu() {
  lcd.setCursor(0, 0); lcd.print(title);
  lcd.setCursor(0, 1); lcd.print("A: Add Ingredients  ");
  lcd.setCursor(0, 2); lcd.print("B: Start/Stop Mixing");
  lcd.setCursor(0, 3); lcd.print("C: Start Dispensing ");
}

// Get Weight
void getWeight() {
  scale1.set_scale(calibration_factor);
  float weight1 = scale1.get_units();
  Serial.println("Reading 1: " + String(weight1, 3) + " kg");
  scale2.set_scale(calibration_factor);
  float weight2 = scale2.get_units();
  Serial.println("Reading 2: " + String(weight2, 3) + " kg");
  weight = (weight1 + weight2) / 2;
  Serial.println("AVE Weight: " + String(weight, 3) + " kg");
}
