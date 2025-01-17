

// * ---------------------    --------------------
// * | HC-SC04 | Arduino |    | 3 pins | Arduino |
// * ---------------------    --------------------
// * |   Vcc   |   5V    |    |   Vcc  |   5V    |
// * |   Trig  |   12    | OR |   SIG  |   13    |
// * |   Echo  |   13    |    |   Gnd  |   GND   |
// * |   Gnd   |   GND   |    --------------------
// * ---------------------


// driver - TB6612


/// ultrasonic
#include <Ultrasonic.h>
Ultrasonic ultrasonic(12, 13);
int distance;

/// for driver
int pwm = 3;
int ain2 = 5;
int ain1 = 6;
float i;

void setup() {
  pinMode(pwm, OUTPUT); // pwm 
  pinMode(ain2, OUTPUT); // AIN 2 
  pinMode(ain1, OUTPUT); // AIN 1
  Serial.begin(9600);
}

void loop() {
  distance = ultrasonic.read();
  
  // Цикл, який виводить повідомлення "No!", коли відстань більше 30 см
  while (distance > 100) {
      Serial.println("Too far!");
      delay(300);
      digitalWrite(ain1, 0);
      digitalWrite(ain2, 0);
      distance = ultrasonic.read(); 
  }

  Serial.print("Distance in CM: ");
  Serial.println(distance);
//  delay(100);

  // Обчислюємо i, щоб вона зменшувалася з відстанню
  i = 255 - (distance * 2.1); // коригуємо коефіцієнт 8.5 за необхідності

  // Обмежуємо значення i в межах 0-255
  i = constrain(i, 0, 255);

  analogWrite(pwm, i);
  digitalWrite(ain2, LOW);
  digitalWrite(ain1, HIGH);
}
