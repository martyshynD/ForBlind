
// Код для пристрою ORIENTA
// Автор проєкту: Дмитро Мартишин
// Помічник, науковий керівник: Андрій Мишопита

#include <Arduino.h>

// Кнопка
#define PIN_BTN 9
int status = 0;

// PIR датчик HC-SR501
int led = 13; 
int pin = 2;
int pin_buz = 8; 
bool state; 

// Оптичний датчик VL53L1X
#include <Wire.h>
#include <VL53L1X.h>
VL53L1X sensor;

// Драйвер моторів TB6612FNG
int pwm = 3;
int ain2 = 5;
int ain1 = 6;

// значення поданої напруги на вібромоторчик (за принципом ШИМ) 
float i;


// Функція для оптичного датчику
void opticfunc(){
  noTone(pin_buz);
  sensor.read();
  if (sensor.ranging_data.range_mm >= 1600) {
    analogWrite(pwm, 0);
    digitalWrite(ain1, 0);
    digitalWrite(ain2, 0);
  }
  else {
    i = map(sensor.ranging_data.range_mm, 1600, 0, 0, 255);
    analogWrite(pwm, i);
    digitalWrite(ain2, LOW);
    digitalWrite(ain1, HIGH);  
  }
}




// Функція для PIR датчика
void pirfunc(){
  state = digitalRead(pin); // Використовуємо глобальну змінну
  if (state == HIGH) { 
    digitalWrite(led, HIGH);
    tone(pin_buz, 880);
  } else {
    digitalWrite(led, LOW);
    noTone(pin_buz);
  }
  analogWrite(pwm, LOW);
  digitalWrite(ain2, LOW);
  digitalWrite(ain1, LOW);    
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(pin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(pwm, OUTPUT); // pwm 
  pinMode(ain2, OUTPUT); // AIN 2 
  pinMode(ain1, OUTPUT); // AIN 1
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C
  sensor.setTimeout(100);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);
  sensor.startContinuous(50);  
}

void loop() {

  if (digitalRead(PIN_BTN)== 0){
    delay(200);
    status++;
  }
  if (status > 2){
    status = 1; 
  }

  if (status == 1){
    Serial.println("Mode OPTIC");
    opticfunc();
  }

  if (status == 2){
    Serial.println("Mode PIR");
    pirfunc();
  }
}
