
// Код для пристрою ORIENTA
// Автор проєкту: Дмитро Мартишин
// Помічник, науковий керівник: Андрій Мишопита

// * ---------------------    --------------------
// * | HC-SR04 | Arduino |    | 3 pins | Arduino |
// * ---------------------    --------------------
// * |   Vcc   |   5V    |    |   Vcc  |   5V    |
// * |   Trig  |   12    | OR |   SIG  |   13    |
// * |   Echo  |   13    |    |   Gnd  |   GND   |
// * |   Gnd   |   GND   |    --------------------
// * ---------------------


// Ультразвуковий датчик HC-SR04 
#include <Ultrasonic.h>
Ultrasonic ultrasonic(12, 13);
int distance;

// Драйвер моторів TB6612FNG
int pwm = 3;
int ain2 = 5;
int ain1 = 6;

// значення поданої напруги на вібромоторчик (за принципом ШИМ) 
float i;

void setup() {
  pinMode(pwm, OUTPUT); // pwm 
  pinMode(ain2, OUTPUT); // AIN 2 
  pinMode(ain1, OUTPUT); // AIN 1
  Serial.begin(9600);
}

void loop() {

  // зчитуємо значення з датчика відстані
  distance = ultrasonic.read();
  
  // Цикл, який вимикає вібромотор, коли відстань більше 120 см
  while (distance > 120) {
      Serial.println("Too far!");
      delay(300);
      digitalWrite(ain1, 0);
      digitalWrite(ain2, 0);
      distance = ultrasonic.read(); 
  }

  // Обчислюємо i - , щоб воно зменшувалася зі збільшенням відстані, та збільшувалось зі зменшенням відстані
  i = 255 - (distance * 2.1); // коригуємо коефіцієнт 2.1 за необхідності (це потрібно для релюгування відстані)

  // Обмежуємо значення i в межах 0-255
  i = constrain(i, 0, 255);

  // подаємо струм на моторчик із певною напругою, маючи вже розраховане значення i (ШИМ)
  analogWrite(pwm, i);
  digitalWrite(ain2, LOW);
  digitalWrite(ain1, HIGH);
}
