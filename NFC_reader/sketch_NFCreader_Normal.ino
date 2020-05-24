#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 4  //D2
#define RST_PIN 5 //D1

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 16, 2);
ESP8266WiFiMulti wifiMulti;
boolean connectioWasAlive = true;
int readerID = 1; //Normal Stamp readerID(1~9)

void setup() 
{
  Serial.begin(9600);
  
  SPI.begin(); 
  mfrc522.PCD_Init();
  Wire.begin(D4, D3);
 
  lcd.init();
  lcd.backlight();

  wifiMulti.addAP("ssid1", "password1");
  wifiMulti.addAP("ssid2", "password2");
  wifiMulti.addAP("ssid3", "password3");


}

void findWiFi()
{
  if (wifiMulti.run() != WL_CONNECTED)
  {
    if (connectioWasAlive == true)
    {
      connectioWasAlive = false;
      Serial.print("Looking for WiFi ");
    }
    Serial.print(".");
    delay(500);
  }
  else if (connectioWasAlive == false)
  {
    connectioWasAlive = true;
    Serial.printf(" connected to %s\n", WiFi.SSID().c_str());
    lcd.setCursor(0,0);                       // 0번째 줄 0번째 셀부터 입력하게 합니다.
    lcd.print("Wifi Connect");               // 아래의 문장을 출력합니다.
    delay(5000);
    lcd.clear();
  }
}

void loop() 
{
  while(WiFi.status() != WL_CONNECTED){
    findWiFi();
  }    
  if (WiFi.status() == WL_CONNECTED) 
  {
    String UID = "";
    if ( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial() ) {   
                                               // 태그 접촉이 되지 않았을때 또는 ID가 읽혀지지 않았을때
    delay(500);                                // 0.5초 딜레이 
    return;                                    // return
    } 
    Serial.print("Card UID:");                  // 태그의 ID출력
  
    for (byte i = 0; i < 4; i++) {               // 태그의 ID출력하는 반복문.태그의 ID사이즈(4)까지
      Serial.print(mfrc522.uid.uidByte[i]);        // mfrc.uid.uidByte[0] ~ mfrc.uid.uidByte[3]까지 출력
      UID +=mfrc522.uid.uidByte[i]; 
      Serial.print(" ");               
    }
    Serial.print("UID:");
    Serial.print(UID); 
    Serial.println();

    HTTPClient http;
 
    http.begin("http://www.tobimoa.ml/api/stamp/add");
    http.addHeader("Content-Type", "application/json");
   
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["ReaderID"] = readerID;
    root["UID"] = UID;
   
    char senddata[100];
    root.prettyPrintTo(senddata,sizeof(senddata));
 
    int httpCode = http.POST(senddata);
    String payload = http.getString();

    Serial.print("httpCode: ");
    Serial.println(httpCode);   //Print HTTP return code
    Serial.print("patload: ");
    Serial.println(payload);    //Print request response payload

    StaticJsonBuffer<500> stampBuffer;
    JsonObject& info = stampBuffer.parseObject(http.getString());
 
    int stamp = info["StampCnt"];
  
   if (httpCode > 0) 
   {
      switch(stamp){
      case 1: 
            lcd.setCursor(0,0);                       // 0번째 줄 0번째 셀부터 입력하게 합니다.
            lcd.print("1st Stamp!");              
            break;  
      case 2:  
            lcd.setCursor(0,0);                       
            lcd.print("2nd Stamp!");             
            break;
      case 3:  
            lcd.setCursor(0,0);                       
            lcd.print("3rt Stamp!");              
            break;  
      case 4:  
            lcd.setCursor(0,0);                       
            lcd.print("4th Stamp!");             
            break;  
      case 5:  
            lcd.setCursor(0,0);                       
            lcd.print("5th Stamp!");              
            break;  
      case 6:  
            lcd.setCursor(0,0);                       
            lcd.print("6th Stamp!");              
            break;  
      case 7:  
            lcd.setCursor(0,0);                       
            lcd.print("7th Stamp!");              
            break;  
      case 8:  
            lcd.setCursor(0,0);                       
            lcd.print("8th Stamp!");              
            break;  
      case 9:  
            lcd.setCursor(0,0);                       
            lcd.print("9th Stamp!");              
            break;  
      case 10:  
            lcd.setCursor(0,0);                       
            lcd.print("10th Stamp!");              
            break;  
              
      default:  
                Serial.println("error");                        
     }  
     delay(5000);                              // 5초간 대기합니다.
     lcd.clear(); 
   }
 
    http.end();  //Close connection

    
  }
  delay(1000);
}
