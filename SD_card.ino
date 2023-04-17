
String readLog() {
  // Open the file for reading
  File file = SD.open(logUID_Path, FILE_READ);
  if (!file) {
    Serial.println("Error opening file for reading");
  }
  // Read the file contents into a string
  String fileContents = "";
  while (file.available()) {
    fileContents += (char)file.read();
  }
  //  Serial.print(fileContents);
  // Close the file
  file.close();

  // Parse the file contents and construct the JSON string
  String jsonString = "{";
  String key, value;
  bool inKey = true;
  for (int i = 0; i < fileContents.length(); i++) {
    char c = fileContents.charAt(i);
    if (c == ':') {
      inKey = false;
    }
    else if (c == '\n' || c == '\r') {
      if (key != "" && value != "") {
        jsonString += "\"" + key + "\": " + value + ",";
        key = "";
        value = "";
        inKey = true;
      }
    }
    else if (c != ' ') {
      if (inKey) {
        key += c;
      }
      else {
        value += c;
      }
    }
  }
  jsonString.remove(jsonString.length() - 1); // Remove last comma
  jsonString += "}";

  //  Serial.print(String('{') + String('"') + "uid" + String('"') + String(':') + String('"') + UID + String('"') + String(',') + String('"') + "logs" + String('"')+ String(':') + jsonString + String('}'));
  String Log_Data = String('{') + String('"') + "uid" + String('"') + String(':') + String('"') + uid + String('"') + String(',') + String('"') + "logs" + String('"') + String(':') + jsonString + String('}');

  Serial.println(Log_Data);
  return Log_Data;
}
