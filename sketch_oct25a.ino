// Код для пристрою ORIENTA
// Автор проєкту: Дмитро Мартишин
// Помічник, науковий керівник: Андрій Мишопита

#include <Arduino.h>
#define HC_TRIG 12
#define HC_ECHO 13

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
float value_pwm;

float sonarfunc() {
  digitalWrite(HC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(HC_TRIG, LOW);
  uint32_t us = pulseIn(HC_ECHO, HIGH);
  return (us / 5.82);
}

bool threshhold_arr(float arr[], int len, float average, float diff){ // повертаємо одиничку якщо різниця між середнім та будь якою ячейкою масиву більша за поріг
  for (int i = 0; i < len; i++)
  {
    if( ( (arr[i]-average) > diff )||( (average-arr[i]) > diff ) ) {
      return 1;
    }
  }
  return 0;
}

void getDist() {
  static float arr[50] = {0}; static int pointer = 0; static float average_sum = 0;  static float average_res = 0;
  sensor.read();

  average_sum -= arr[pointer];
  arr[pointer] = sensor.ranging_data.range_mm;
  average_sum += arr[pointer];
  
  pointer=(pointer+1)%50; 
  average_res = average_sum/50;

  float rg = min(sensor.ranging_data.range_mm, sonarfunc());

  if (threshhold_arr(arr, 50, average_res, 30)){
    digitalWrite(pin_buz, 0);
    Serial.println("threshhold_arr");
  }
  else {
    digitalWrite(pin_buz, 1);

    analogWrite(pwm, 0);
    digitalWrite(ain1, 0);
    digitalWrite(ain2, 0);
    Serial.println("not threshhold_arr");
    return;

  }
  if (rg >= 1600) {
    analogWrite(pwm, 0);
    digitalWrite(ain1, 0);
    digitalWrite(ain2, 0);
  }
  else {
    value_pwm = map(rg, 1600, 0, 0, 255);
    analogWrite(pwm, value_pwm);
    digitalWrite(ain2, LOW);
    digitalWrite(ain1, HIGH);  
    Serial.print("value_pwm: ");
    Serial.println(value_pwm);
  }
}

// Функція для PIR датчика
void pirfunc(){
  state = digitalRead(pin); // Використовуємо глобальну змінну
  if (state == HIGH) { 
    digitalWrite(led, HIGH);
    digitalWrite(pin_buz, 1);
  } else {
    digitalWrite(led, LOW);
    digitalWrite(pin_buz, 0);
  }
  analogWrite(pwm, LOW);
  digitalWrite(ain2, LOW);
  digitalWrite(ain1, LOW);    
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(pin, INPUT);
  pinMode(pin_buz, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(pwm, OUTPUT); // pwm 
  pinMode(ain2, OUTPUT); // AIN 2 
  pinMode(ain1, OUTPUT); // AIN 1
  pinMode(HC_TRIG, OUTPUT); // HC_TRIG 
  pinMode(HC_ECHO, INPUT); // HC_ECHO
  
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
    getDist();
  }

  if (status == 2){
    Serial.println("Mode PIR");
    pirfunc();
  }
}
