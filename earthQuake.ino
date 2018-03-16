#include <EEPROM.h>
#include "EEPROMAnything.h"
#include <LiquidCrystal.h>


const int alarmPin = 5;
const int relayPin = 10;
int Xacc, Yacc, Zacc, threshold = 0, thresholdSET = 25;
long debouncing_time = 15; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;
LiquidCrystal lcd(12, 11, 9, 8, 7, 6);
struct sensorValue
{
  int X;
  int Y;
  int Z;
};

sensorValue acceleration;

void debounceInterrupt_Increment()
{
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    IncrementThreshold();
    last_micros = micros();
  }
}

void debounceInterrupt_Decrement()
{
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    DecrementThreshold();
    last_micros = micros();
  }
}
void IncrementThreshold() {
  thresholdSET = EEPROM.read(500);
  thresholdSET++;
  EEPROM.write(500, thresholdSET);
}

void DecrementThreshold() {
  thresholdSET = EEPROM.read(500);
  thresholdSET--;
  EEPROM.write(500, thresholdSET);
}

void setup() {
  lcd.begin(16, 2);
  attachInterrupt(0, debounceInterrupt_Increment, RISING);
  attachInterrupt(1, debounceInterrupt_Decrement, RISING);
  pinMode(alarmPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  EEPROM.write(500, thresholdSET);
  digitalWrite(alarmPin, LOW);
  lcd.setCursor(0, 0);
  lcd.print("Initializing....");
  delay(5000);
  sensorValue acceleration = { analogRead(A0) , analogRead(A1) , analogRead(A2) };
  EEPROM_writeAnything(0, acceleration);
  EEPROM_readAnything(0, acceleration);
  lcd.clear();
}

void loop() {
  EEPROM_readAnything(0, acceleration);
  threshold = EEPROM.read(500);
  lcd.setCursor(0, 0);
  lcd.print("Monitoring Mode");
  lcd.setCursor(0,1);
  lcd.print("Threshold = ");
  lcd.print(threshold);
  Xacc = analogRead(A0);
  Yacc = analogRead(A1);
  Zacc = analogRead(A2);

  if ((Xacc >= (acceleration.X + threshold)) || (Xacc <= (acceleration.X - threshold))||(Yacc >= (acceleration.Y + threshold)) || (Yacc <= (acceleration.Y - threshold))||(Zacc >= (acceleration.Z + threshold)) || (Zacc <= (acceleration.Z - threshold))) {
    digitalWrite(relayPin, LOW);
    digitalWrite(alarmPin, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALARM !!!!!");
    lcd.setCursor(0,1);
    lcd.print("PLEASE EVACUATE");
    delay(5000);
    digitalWrite(relayPin, HIGH);
    digitalWrite(alarmPin, LOW);
    lcd.clear();
  }
 

}
