void rotateMotor1() {

  digitalWrite(dirPin, HIGH);
  digitalWrite(motorEN1, LOW); //turn on the motor by active low
  for (int x = 0; x < Nema17Motor_Steps; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
  }
  digitalWrite(motorEN1 , HIGH); //tuon off the motor by active high

}
void rotateMotor2() {

  digitalWrite(dirPin2, HIGH);
  digitalWrite(motorEN2, LOW); //turn on the motor2 by active low
  for (int x = 0; x < Nema17Motor_Steps; x++) {
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(2000);
  }
  digitalWrite(motorEN2, HIGH); //turn off the motor2 by active high

}
