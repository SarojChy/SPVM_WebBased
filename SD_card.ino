void Update_Log(String tagUID) {
  Serial.println("Received tag is : " + tagUID);
  

  dataFile = SD.open(logUID_Path, FILE_READ);
  int c = 0;
  if (!dataFile) {
    Serial.println("Could not open file!");
    return;
  }
  boolean tagExists = false;
  while (dataFile.available()) {
    String line = "";
    line = dataFile.readStringUntil('\n');
    int pos = line.indexOf(":");
    String fileTag = line.substring(0, pos);
    int count = line.substring(pos + 1).toInt();
    if (fileTag.startsWith(tagUID)) {
      tagExists = true;
      Serial.println(fileTag + " ->Exists");
      Serial.println("line :" + String(c) + "," + line);
      update_Count(fileTag, count, c);
      break;
    }
    c++;
  }
  dataFile.close();
  if (!tagExists) {

    dataFile = SD.open(logUID_Path, FILE_APPEND);
    if (!dataFile) {
      Serial.println("Could not open file!");
      return;
    }
    String New_1 = each_uid + ":" + "1";
    Serial.println("New Log: " + New_1);
    dataFile.println(New_1);
    dataFile.close();
  }
}


void update_Count(String fileTag, int count, int c) {
  dataFile.close();
  char* array [500];
  int line_count = 0;
  count++;
  String updated_tag = fileTag + ":" + String(count);
  Serial.println(updated_tag);

  dataFile = SD.open(logUID_Path, FILE_READ);
  if (!dataFile) {
    Serial.println("Could not open file!");
    return;
  }

  // Allocate memory for each pointer in the array
  for (int i = 0; i < sizeof(array) / sizeof(char*); i++) {
    array[i] = (char*)malloc(15); // Allocate 15 bytes for each string
  }

  while (dataFile.available()) {

    String line = "";
    line = dataFile.readStringUntil('\n');

    array[line_count] = (char*)malloc(line.length() + 1);

    if (line_count == c) {
      line = "";
      line = updated_tag;
    }
    strcpy(array[line_count], line.c_str());
    line_count++;
  }
  dataFile.close();

  dataFile = SD.open(logUID_Path, FILE_WRITE);
  if (!dataFile) {
    Serial.println("Could not open file!");
    return;
  }
  for (int i = 0; i < line_count; i++) {
    Serial.println(array[i]);
    //    Serial.println(array[i].length());
    String new_line_data = String(array[i]);
    new_line_data.trim();
    dataFile.print(new_line_data);
    dataFile.println();
  }
  dataFile.close();

  //// Free the memory for each pointer in the array
  for (int i = 0; i < sizeof(array) / sizeof(char*); i++) {
    free(array[i]);
  }
}



String postLog() {
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
