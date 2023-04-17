
void JSON_Conversion_uid(String payload) {
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc,  payload);
  if (error) {
    Serial.println("deserializeJson() failed");
    return;
  }
  String tagsVar = doc["results"];
  //  Serial.println(tagsVar);
  deserializeJson(doc,  tagsVar);
  String tags = doc["tags"];
  //  Serial.println(tags);
  deserializeJson(doc, tags);
  if (doc.is<JsonArray>()) {

    File output = SD.open(fetchedUID_Path, FILE_APPEND);
    if (output) {
      JsonArray arr = doc.as<JsonArray>();
      for (JsonVariant v : arr) {
        if (v.is<String>()) {
          String str = v.as<String>();
          Serial.println(str);
          // Open the file in append mode and write the string to it
          output.println(str);
        }
        else {
          Serial.println("Invalid String Format");
        }
      }
      output.close();
      Serial.println("Successfully Written");
    }
    else {
      Serial.println("Failed to open file");
    }
  }
  else {
    Serial.println("Invalid JSON array");
  }
}


void checkTagUnixTimeStamp(String chekTag_payload) {
  Serial.println(chekTag_payload);
  StaticJsonDocument<200> doc;
  deserializeJson(doc,  chekTag_payload);
  String latest_Stamp = doc["latest_timestamp"];
  Serial.println(latest_Stamp);
  writeFile(SPIFFS, timeStamp_Path, latest_Stamp.c_str());

}
