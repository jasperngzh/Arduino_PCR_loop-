#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x20, 16, 2);

// Thermistor settings
#define thermistorpin_vdA A0
#define thermistorpin_vd5 2
#define nominal_resistance 10000
#define nominal_temperature 25
#define samplingrate 5
#define beta 3950
#define Rref 10000

// Peltier/Fan
#define fanpin 3
#define peltierpin 5

// PCR timing in milliseconds
#define firstdenaturationtime 10000
#define denaturationtime 10000
#define annealingtime 10000
#define extensiontime 10000
#define lastextensiontime 10000

// PCR temperatures in °C
#define denaturationtemp 94
#define annealingtemp 55
#define extensiontemp 72

// Globals
float temperature;
int peltier_level;

// Read temperature from thermistor
void tempread() {
  int samples = 0;
  float average;
  
  digitalWrite(thermistorpin_vd5, HIGH);
  for (int i = 0; i < samplingrate; i++) {
    samples += analogRead(thermistorpin_vdA);
    delay(10);
  }
  digitalWrite(thermistorpin_vd5, LOW);

  average = samples / float(samplingrate);
  Serial.print("ADC reading: ");
  Serial.println(average);

  average = 1023.0 / average - 1.0;
  average = Rref / average;

  temperature = average / nominal_resistance;
  temperature = log(temperature);
  temperature /= beta;
  temperature += 1.0 / (nominal_temperature + 273.15);
  temperature = 1.0 / temperature;
  temperature -= 273.15;

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C ");
}

// Set Peltier power (0–99%)
void peltier_state(int power) {
  peltier_level = map(power, 0, 99, 0, 255);
  analogWrite(peltierpin, peltier_level);
}

// Hold temperature near target
void holdTemp(float targetTemp) {
  tempread(); // Read temperature before acting
  if (temperature < targetTemp - 0.5) {
    peltier_state(30);
    digitalWrite(fanpin, LOW);
  } else if (temperature > targetTemp + 1) {
    digitalWrite(fanpin, HIGH);
    analogWrite(peltierpin, 0);
  } else {
    digitalWrite(fanpin, LOW);
    analogWrite(peltierpin, 0);
  }
}

// Stop everything at end
void stop() {
  lcd.setCursor(0, 0);
  lcd.print("Cooling down    ");
  analogWrite(peltierpin, 0);
  digitalWrite(fanpin, HIGH);
  while (true); // Infinite loop
}

void setup() {
  pinMode(fanpin, OUTPUT);
  pinMode(thermistorpin_vd5, OUTPUT);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("PCR Starting...");
  delay(2000);
  lcd.clear();
}

void loop() {
  int cyclenumber = 3;
  int currentcycle = 0;

  // First denaturation step
  lcd.setCursor(0, 1);
  lcd.print("1st Denaturation");
  unsigned long startTime = millis();
  while (millis() - startTime < firstdenaturationtime) {
    holdTemp(denaturationtemp);
    delay(500);
  }

  // Main PCR cycles
  while (currentcycle < cyclenumber) {
    Serial.print("Starting cycle ");
    Serial.println(currentcycle + 1);

    // Denaturation
    lcd.setCursor(0, 1);
    lcd.print("Denaturation     ");
    startTime = millis();
    while (millis() - startTime < denaturationtime) {
      holdTemp(denaturationtemp);
      delay(500);
    }

    // Annealing
    lcd.setCursor(0, 1);
    lcd.print("Annealing        ");
    startTime = millis();
    while (millis() - startTime < annealingtime) {
      holdTemp(annealingtemp);
      delay(500);
    }

    // Extension
    lcd.setCursor(0, 1);
    lcd.print("Extension        ");
    startTime = millis();
    if (currentcycle == cyclenumber - 1) {
      while (millis() - startTime < lastextensiontime) {
        holdTemp(extensiontemp);
        delay(500);
      }
    } else {
      while (millis() - startTime < extensiontime) {
        holdTemp(extensiontemp);
        delay(500);
      }
    }

    currentcycle++;
  }

  lcd.setCursor(0, 1);
  lcd.print("Cycle complete  ");
  Serial.println("PCR Cycle Complete!");
  delay(3000);
  stop();
}

void stop() {
  while (1) { // infinite loop
lcd.setCursor(0, 0);
lcd.print("cooling down");
 
  }
}