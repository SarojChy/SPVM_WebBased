#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <AsyncTCP.h>
#include <LiquidCrystal_I2C.h>
#include <ESPAsyncWebServer.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
IPAddress localIP;
//IPAddress localIP(192, 168, 1, 200); // hardcoded
// Set your Gateway IP address
//IPAddress localGateway;
IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);

WiFiClient client;
HTTPClient http;

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "uid";
//Variables to save values from HTML form
String ssid;
String pass;
String uid;

//const char* ssid = "nepatronix_2.4";
//const char* password = "CLB269DA03";
//String device_uid = "702250c6-b051-40a5-b2ae-85f14a899737";

const char* ssid_Path = "/ssid.txt";
const char* pass_Path = "/pass.txt";
const char* uid_Path = "/uid.txt";

const char* fetchedUID_Path = "/FetchedUID.txt";
const char* timeStamp_Path = "/timeStamp.txt";
const char* logUID_Path = "/logUID.txt";


const char* PostUIDServer = "http://spvm.nepatronix.org/api/v1/spvmdevice/sync-logs/";
String GetUIDServer;
String CheckTagServer;

Preferences preferences;
#define SS_PIN 16
#define RST_PIN 17

#define ResetWiFi 12

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
const unsigned long eventAPmode = 15000;  //Turn into Access point mode for WiFi configuration for 15 Secconds
unsigned long prevAPevent = 0;

//Variables for timeStamp
String chekTag_payload;



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
  WiFi.mode(WIFI_STA);
  Buzzer();
  SPI.begin();
  lcd.init();
  lcd.backlight();
  mfrc522.PCD_Init();
  initSPIFFS();
  WiFi.mode(WIFI_STA);


  // Reading values from Access Point mode
  ssid = readFile(SPIFFS, ssid_Path);
  pass = readFile(SPIFFS, pass_Path);
  uid = readFile (SPIFFS, uid_Path);
  String previous_Time = readFile(SPIFFS, timeStamp_Path);
  uid = uid.c_str();
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(uid);
  Serial.println(previous_Time);
  WiFi.begin(ssid.c_str(), pass.c_str());
  delay(4000);

  if (!SD.begin(5)) {
    Serial.println("Error initializing SD card");
    while (1);
  }
  Serial.println("SD card initialized");

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
  pinMode(ResetWiFi, INPUT_PULLUP);
  NPadNo = preferences.getInt("NPadNo", 0);



  GetUIDServer = "http://spvm.nepatronix.org/api/v1/spvmdevice/get-tags?device_uid=" + uid + "&page=";
  CheckTagServer = "http://spvm.nepatronix.org/api/v1/spvmdevice/check-tags?device_uid=" + uid + "&last_tag_sync=";
  Serial.println(GetUIDServer);
  Serial.println(CheckTagServer);
  delay(2000);
  if (WiFi.status() == WL_CONNECTED) {

    if (!SPIFFS.exists(timeStamp_Path)) {
      Serial.println("NO Exists");
      getUID();
      checkTagUnixTimeStamp(chekTag_payload);

    }
    else {
      Serial.println("File exits");
    }

    String CheckTagServer_1 = CheckTagServer + previous_Time;
    Serial.println(CheckTagServer_1);

    if (check_Tag(CheckTagServer_1) != "\"latest\"") {
      getUID();
      checkTagUnixTimeStamp(chekTag_payload);
    }

    if ((preferences.getInt("postState", 0)) == 1) {
      Serial.println("Posting log Data");
      delay(1000);
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
  delay(200);

  if (digitalRead(Resetpin) == 0) {
    Serial.println(digitalRead(Resetpin));
    delay(20);
    Reset();
  }

  if (digitalRead(ResetWiFi) == LOW) {
    if (currentMillis - prevAPevent >= eventAPmode) {
      AP_Mode();
      prevAPevent = currentMillis;
    }
  }

  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - prevWiFiIntereval >=  WiFiInterval)) {
    Serial.println("WiFi Disconnected");
    WiFi.disconnect();
    delay(50);
    WiFi.reconnect();
    prevWiFiIntereval = currentMillis;
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
