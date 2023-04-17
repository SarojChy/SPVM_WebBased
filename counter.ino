void count() {
  lcd.setCursor(1, 1);
  lcd.print("Available: ");
  lcd.setCursor(12, 1);
  NPadNo--;
  preferences.putInt("NPadNo", NPadNo);
  NPadNo = preferences.getInt("NPadNo", 0);
  Serial.println(NPadNo);
  delay(200);
  lcd.print(NPadNo);
}
