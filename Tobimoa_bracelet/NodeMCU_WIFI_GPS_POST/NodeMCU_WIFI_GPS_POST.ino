#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFiMulti.h>

TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(4,5); // The serial connection to the GPS device
ESP8266WiFiMulti wifiMulti;

boolean connectioWasAlive = true;
float latitude , longitude; //위도, 경도

void monitorWiFi(); //wifimulti 함수

void setup() {
  
  Serial.begin(115200); //시리얼통신
  ss.begin(9600); //gps통신

  //사용할 wifi 등록
  wifiMulti.addAP("", ""); //your wifi ssid, passwd
  wifiMulti.addAP("", ""); //your wifi ssid, passwd
  wifiMulti.addAP("", ""); //your wifi ssid, passwd
}

void loop() {
  while (WiFi.status() != WL_CONNECTED){
    
    monitorWiFi();
   
  }
  if (WiFi.status() == WL_CONNECTED){

    //GPS 값이 수신되면 서버로 데이터 전송 
    while (ss.available() > 0){
      if (gps.encode(ss.read()))
      {
        if (gps.location.isValid())
        {      
          latitude = gps.location.lat(); 
          longitude = gps.location.lng();
        
          StaticJsonBuffer<200> JSONbuffer;
          JsonObject& Info = JSONbuffer.createObject();
          JsonObject& Location = JSONbuffer.createObject();
      
          Info["UID"] = "1234567890"; //팔찌 UID
          Location["latitude"] = latitude; 
          Location["longitude"] = longitude;
          Info["Location"] = Location; 
      
          char senddata[300];
      
          Info.prettyPrintTo(senddata, sizeof(senddata)); 
          Serial.println(senddata);
          
          HTTPClient http;
      
          http.begin("http://www.tobimoa.ml/api/location/update"); //서버주소
          http.addHeader("Content-Type", "application/json");  //content-type header
      
          int httpCode = http.POST(senddata); //Send the request 200 -> 성공
          String payload = http.getString(); //Get the response payload
       
          Serial.println(httpCode);   //Print HTTP return code
          Serial.println(payload);    //Print request response payload
       
          http.end();  //Close connection

        }
      }
    }
    
    delay(300); //0.3 seconds
    
  }else{
    
    Serial.println("Error in WiFi connection");
    
  }
}

void monitorWiFi()
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
  }
}
