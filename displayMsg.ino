void displayMsg(char *msg) {
  String message = msg;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}
void nopad() {
  lcd.setCursor(0, 0);
  lcd.print("NoPadsAvailable");
  //Serial.println("No PAD available");
  Buzzer();
}
void invalidCard() {
  //Serial.println("Invalid Card");
  displayMsg("invalid Card");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print(each_uid);
  delay(1000);
  displayMsg(" Tap Your Card");
  lcd.setCursor(1, 1);
  lcd.print("Available: ");
  lcd.setCursor(12, 1);
  lcd.print(NPadNo);
}
void notEnoughPad() {
  lcd.setCursor(0, 0);
  lcd.print("Not Enough Pad   ");
  delay(1000);
  displayMsg(" Tap Your Card");
  lcd.setCursor(1, 1);
  lcd.print("Available: ");
  lcd.setCursor(12, 1);
  lcd.print(NPadNo);
  //Serial.println("No PAD available");
  Buzzer();
}
