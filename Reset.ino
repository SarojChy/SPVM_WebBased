void Reset() {
    tone(BUZZ, 500);
    delay(2000);
    noTone(BUZZ);
    preferences.putInt("NPadNo", 60);
    NPadNo = preferences.getInt("NPadNo", 0);
    displayMsg(" -System Reset-");
    for (int a = 0; a <= 15; a++) {
      lcd.setCursor(a, 1);
      lcd.print(".");
      delay(200);
    }
    delay(1000);
    displayMsg(" Tap Your Card");
    lcd.setCursor(1, 1);
    lcd.print("Available: ");
    lcd.setCursor(12, 1);
    lcd.print(NPadNo);
    // Serial.println("Resetttt...");
    delay(500);
}
