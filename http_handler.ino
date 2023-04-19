void getUID() {
  SD.remove(fetchedUID_Path);
  for (int page = 1; page <= 50; page++) {
    String get_UID_Payload = " ";
    Serial.println("Page :" + String(page));
    String getUIDServer_1 = GetUIDServer + String(page);
    http.begin(client, getUIDServer_1);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      get_UID_Payload = http.getString();
      Serial.println(get_UID_Payload);
      JSON_Conversion_uid(get_UID_Payload);

    } else {
      Serial.println("Fetching Successfull");
      Serial.println(httpResponseCode);

      return;
    }
  }
  http.end();
}

String check_Tag(String CheckTagServer_1) {
  http.begin(client, CheckTagServer_1);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    chekTag_payload = http.getString();
    Serial.println("Chek Tag Payload: " + chekTag_payload);
    return chekTag_payload;
  }
  Serial.println("Fetching Successfull");
  Serial.println(httpResponseCode);

  http.end();
}

void postUID() {
  http.begin(client, PostUIDServer);
  http.addHeader("Content-Type", "application/json");
  int httpPostResponse = http.POST(postLog());
  Serial.println("HTTP POST response code is: " + String(httpPostResponse));
  if ( httpPostResponse == 200) {
    Serial.println("POSS succesed");
    preferences.putInt("postState", 0);
    Serial.println("Post stete change to after post: " + String(preferences.getInt("postState", 0)));
  }
  http.end();
}
