void Buzzer() {
  tone(BUZZ, 500);
  delay(100);
  noTone(BUZZ);
  delay(250);
  tone(BUZZ, 500);
  delay(100);
  noTone(BUZZ);
}
