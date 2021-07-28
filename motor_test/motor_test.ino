#include <L298N.h>

//pin definition
#define ENA 9
#define IN1 2
#define IN2 3

#define ENB 6
#define IN3 4
#define IN4 5

L298N motorR(ENA, IN1, IN2);
L298N motorL(ENB, IN3, IN4);

void setup() {
  Serial.begin(9600);

  motorR.setSpeed(255); // an integer between 0 and 255
  motorL.setSpeed(255); // an integer between 0 and 255
}

void loop() {
  // put your main code here, to run repeatedly:
  motorR.forward();
  motorL.forward();
  delay(3000);
  motorR.stop();
  motorL.stop();
  delay(3000);

  for (int i = 100; i < 255; i++) {
    motorR.setSpeed(i); // an integer between 0 and 255
    motorL.setSpeed(i); // an integer between 0 and 255
    motorR.forward();
    motorL.forward();
    Serial.print("speed: ");
    Serial.println(i);
    delay(100);
    motorR.stop();
    motorL.stop();
  }
}
