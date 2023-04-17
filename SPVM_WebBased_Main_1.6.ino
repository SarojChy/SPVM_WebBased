#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>

WiFiClient client;
HTTPClient http;
const char* ssid = "nepatronix_2.4";
const char* password = "CLB269DA03";
String device_uid = "702250c6-b051-40a5-b2ae-85f14a899737";



const char* fetchedUID_Path = "/FetchedUID.txt";
const char* timeStamp_Path = "/timeStamp.txt";
const char* logUID_Path = "/logUID.txt";

const char* PostUIDServer = "http://spvm.nepatronix.org/api/v1/spvmdevice/sync-logs/";
String GetUIDServer = "http://spvm.nepatronix.org/api/v1/spvmdevice/get-tags?device_uid=" + device_uid + "&page=";
String CheckTagServer = "http://spvm.nepatronix.org/api/v1/spvmdevice/check-tags?device_uid=" + device_uid + "&last_tag_sync=";

Preferences preferences;
#define SS_PIN 16
#define RST_PIN 17

#define BUZZ 16 //define pin buzzer pin
#define MOTDEBUG
#define motorEN1 4  //enbalbe pin for driver 
#define motorEN2 4 //enbalbe pin for driver 

#define stepPin  25
#define dirPin  26
#define stepPin2 32
#define dirPin2 33

#define Resetpin 13 // previously used 34 in DOIT version

unsigned long currentMillis;
const unsigned long readTagInterval = 1000;
unsigned long prevTagIntereval = 0;
const unsigned long WiFiInterval = 5000;
unsigned long prevWiFiIntereval = 0;

//Variables for timeStamp
String chekTag_payload;

bool need_To_Post;


const int Nema17Motor_Steps = 200;
const int PadNo = 60;   //Total number of PAD
String data;
int NpadLim = PadNo / 2;

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);
int NPadNo;
String  DataTemp;
String  UID;
bool Valid_UID = false;

void setup()
{
  Serial.begin(115200);
  Buzzer();
  SPI.begin();
  lcd.init();
  lcd.backlight();
  mfrc522.PCD_Init();
  initSPIFFS();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(1000);


  if (!SD.begin(5)) {
    Serial.println("Error initializing SD card");
    while (1);
  }
  Serial.println("SD card initialized");

  Serial.println("Connected to WiFi");

  preferences.begin("my-app", false);

  pinMode(BUZZ, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(motorEN1, OUTPUT);

  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(motorEN2, OUTPUT);

  digitalWrite(motorEN1, HIGH);
  digitalWrite(motorEN2, HIGH);

  pinMode(Resetpin, INPUT_PULLUP);
  NPadNo = preferences.getInt("NPadNo", 0);

  if (WiFi.status() == WL_CONNECTED) {

    if (!SPIFFS.exists(timeStamp_Path)) {
      Serial.println("NO Exists");
      getUID();

    }
    else {
      Serial.println("File exits");
    }
    String previous_Time = readFile(SPIFFS, timeStamp_Path);
    String CheckTagServer_1 = CheckTagServer + previous_Time;
    Serial.println(CheckTagServer_1);
    if (check_Tag(CheckTagServer_1) != "\"latest\"") {
      getUID();
      checkTagUnixTimeStamp(chekTag_payload);
    }


    if ((preferences.getInt("postState", 0)) == 1) {
      Serial.println("Posting log Data");
      delay(1000);
      Serial.println(need_To_Post);
      postUID();
    }

  }


  //  lcd.clear();
  //  displayMsg("    Namaste");
  //  delay(2000);
  //  displayMsg("** Nepatronix **");
  //  for (int a = 0; a <= 15; a++) {
  //    lcd.setCursor(a, 1);
  //    lcd.print(".");
  //    delay(150);
  //  }
  //  delay(1000);
  //  displayMsg("Tap Your Card");
  //  lcd.setCursor(1, 1);
  //  lcd.print("Available: ");
  //  lcd.setCursor(12, 1);
  //  lcd.print(NPadNo);


}

void loop() {
  currentMillis = millis();
  NPadNo = preferences.getInt("NPadNo", 0);
  Serial.println(NPadNo);
  delay(500);

  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - prevWiFiIntereval >=  WiFiInterval)) {
    Serial.println("Wifi is not connected");
    prevWiFiIntereval = currentMillis;
  }

  if (digitalRead(Resetpin) == 0) {
    Serial.println(digitalRead(Resetpin));
    delay(20);
    Reset();
  }

  if ((NPadNo > 0) && (currentMillis - prevTagIntereval >=  readTagInterval) && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Reading");

    //Serial.println("Card found");
    String data = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      data += String(mfrc522.uid.uidByte[i], HEX);
    }
    data.toUpperCase();
    data.replace(" ", ""); // Remove spaces from tag data
    Serial.print("Data : ");
    Serial.println(data);

    File myFile = SD.open(fetchedUID_Path, FILE_READ);
    if (myFile) {
      Serial.println("File is Opned");
      while (myFile.available()) {
        String line = myFile.readStringUntil('\n');
        if (line.startsWith(data)) {

          Valid_UID =  true;
          myFile.close();
          verify();   //checking either UIDs presein SD or not
          break;

        }
        line = "";
      }

    }
    else {
      Serial.println("Error opening FILE");
    }
    prevTagIntereval = currentMillis;
  }
  else if (NPadNo == 0) {
    nopad();
  }
}


void verify() {

  if (Valid_UID) {

    Serial.println("Valid");
    Buzzer();
    checkForStack();
    //    logDataOnSD();
    preferences.putInt("postState", 1);
    Serial.println("Post state change to: " + String(preferences.getInt("postState", 0)));
    
    count();
  }
  else {
    Serial.println("NOT Valid");
    tone(BUZZ, 300);
    delay(1500);
    noTone(BUZZ);
    invalidCard();
  }
  Valid_UID = false;
}
