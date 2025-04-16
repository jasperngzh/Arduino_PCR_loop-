#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x20, 16, 2);

// Thermistor setup
#define thermistorpin_vdA A0
#define thermistorpin_vd5 2
#define nominal_resistance 10000
#define nominal_temperature 25
#define samplingrate 5
#define beta 3950
#define Rref 10000

// Hardware
#define fanpin 3
#define peltierpin 5

// PCR times (ms)
#define firstdenaturationtime 10000
#define denaturationtime 10000
#define annealingtime 10000
#define extensiontime 10000
#define lastextensiontime 10000

// Target temps (°C)
#define denaturationtemp 94
#define annealingtemp 55
#define extensiontemp 72

// Globals
float temperature;
int peltier_level;
unsigned long startTime = 0;
int currentCycle = 0;
int totalCycles = 3;
int state = 0;  // This acts like switchcasevalue

void tempread() {
  int samples = 0;
  float avg;

  digitalWrite(thermistorpin_vd5, HIGH);
  for (int i = 0; i < samplingrate; i++) {
    samples += analogRead(thermistorpin_vdA);
    delay(10);
  }
  digitalWrite(thermistorpin_vd5, LOW);

  avg = samples / float(samplingrate);
  avg = 1023.0 / avg - 1.0;
  avg = Rref / avg;

  temperature = avg / nominal_resistance;
  temperature = log(temperature);
  temperature /= beta;
  temperature += 1.0 / (nominal_temperature + 273.15);
  temperature = 1.0 / temperature;
  temperature -= 273.15;

  Serial.print("Temp: ");
  Serial.println(temperature);

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C ");
}

void peltier_state(int power) {
  peltier_level = map(power, 0, 99, 0, 255);
  analogWrite(peltierpin, peltier_level);
}

void holdTemp(float targetTemp) {
  tempread();
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

void stop() {
  lcd.setCursor(0, 0);
  lcd.print("Cooling down    ");
  analogWrite(peltierpin, 0);
  digitalWrite(fanpin, HIGH);
  while (true);
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

  state = 0;
  startTime = millis();
}

void loop() {
  switch (state) {
    case 0:  // First Denaturation
      lcd.setCursor(0, 1);
      lcd.print("1st Denaturation");
      if (millis() - startTime < firstdenaturationtime) {
        holdTemp(denaturationtemp);
      } else {
        state = 1;
        startTime = millis();
      }
      break;

    case 1:  // Denaturation
      lcd.setCursor(0, 1);
      lcd.print("Denaturation     ");
      if (millis() - startTime < denaturationtime) {
        holdTemp(denaturationtemp);
      } else {
        state = 2;
        startTime = millis();
      }
      break;

    case 2:  // Annealing
      lcd.setCursor(0, 1);
      lcd.print("Annealing        ");
      if (millis() - startTime < annealingtime) {
        holdTemp(annealingtemp);
      } else {
        state = 3;
        startTime = millis();
      }
      break;

    case 3:  // Extension
      lcd.setCursor(0, 1);
      lcd.print("Extension        ");
      if (currentCycle == totalCycles - 1) {
        if (millis() - startTime < lastextensiontime) {
          holdTemp(extensiontemp);
        } else {
          state = 4;
        }
      } else {
        if (millis() - startTime < extensiontime) {
          holdTemp(extensiontemp);
        } else {
          currentCycle++;
          state = 1;
          startTime = millis();
        }
      }
      break;

    case 4:  // Finish
      lcd.setCursor(0, 1);
      lcd.print("PCR Complete!    ");
      Serial.println("PCR Complete!");
      delay(3000);
      stop();
      break;
  }

  delay(200);  // Helps with LCD flicker and sensor timing
}
