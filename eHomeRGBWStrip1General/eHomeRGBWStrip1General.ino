#include <NTPClient.h>  https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>  //used for NTP 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Ticker.h>
#include "ArduinoJson.h"
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <eHome.h>


#define Strip1RedPin D1
#define Strip1GreenPin D2
#define Strip1BluePin D3
#define Strip1WhitePin D4

#define Strip2RedPin D5
#define Strip2GreenPin D6
#define Strip2BluePin D7
#define Strip2WhitePin D8



int strip1RedLedVal;
int strip1GreenLedVal;
int strip1BlueLedVal;
int strip1WhiteLedVal;
int strip2RedLedVal;
int strip2GreenLedVal;
int strip2BlueLedVal;
int strip2WhiteLedVal;


const char* host = "RGBWStripKitchen";

const char* binFile="eHomeRGBWStrip1.ino.d1_mini.bin";
const char* url      = "/api";
const char* deviceId = "RGBWStripKitchen";
const char* eHomeFwVer = "1.0";
const char* deviceType = "LEDStrip";
const char* deviceLocation = "Ground Floor";
const char* deviceScope = "Kitchen LED Strip";




//////////////// MQTT callback ///////////////////////////////////


void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if(strcmp(topic,"RGBWStrip1Kitchen/colorValues")==0){
    // check for messages on subscribed topics
  payload[length] = '\0';
  Serial.print("Topic: ");
  Serial.println(String(topic));

  // convert payload to String
  String value = String((char*)payload);
  //value.trim();
  Serial.print (value);

// find first ; in the string
  int firstClosingBracket = value.indexOf(';')+1;

  // find second ; in the string
  int secondOpeningBracket = firstClosingBracket + 1;
  int secondClosingBracket = value.indexOf(';', secondOpeningBracket);

  // find the third ; in the string
  int thirdOpeningBracket = secondClosingBracket + 1;
  int thirdClosingBracket = value.indexOf(';', thirdOpeningBracket);

//  // find the fourth ; in the string
//  int fourthOpeningBracket = thirdClosingBracket + 1;
//  int fourthClosingBracket = value.indexOf(';', fourthOpeningBracket);


  // using the locations of ; find values 
  strip1RedLedVal = value.substring(0 , (firstClosingBracket - 1)).toInt();
  strip1GreenLedVal = value.substring(firstClosingBracket , secondClosingBracket).toInt();
  strip1BlueLedVal = value.substring((secondClosingBracket +1) , thirdClosingBracket).toInt();




  analogWrite(Strip1GreenPin, strip1GreenLedVal*10.23);
  analogWrite(Strip1RedPin, strip1RedLedVal*10.23);
  analogWrite(Strip1BluePin, strip1BlueLedVal*10.23);

     
  }


  if(strcmp(topic,"RGBWStrip2Kitchen/colorValues")==0){
  // check for messages on subscribed topics
  payload[length] = '\0';
  Serial.print("Topic: ");
  Serial.println(String(topic));

  // convert payload to String
  String value = String((char*)payload);
  //value.trim();
  Serial.print (value);

// find first ; in the string
  int firstClosingBracket = value.indexOf(';')+1;

  // find second ; in the string
  int secondOpeningBracket = firstClosingBracket + 1;
  int secondClosingBracket = value.indexOf(';', secondOpeningBracket);

  // find the third ; in the string
  int thirdOpeningBracket = secondClosingBracket + 1;
  int thirdClosingBracket = value.indexOf(';', thirdOpeningBracket);

//  // find the fourth ; in the string
//  int fourthOpeningBracket = thirdClosingBracket + 1;
//  int fourthClosingBracket = value.indexOf(';', fourthOpeningBracket);


  // using the locations of ; find values 
  strip2RedLedVal = value.substring(0 , (firstClosingBracket - 1)).toInt();
  strip2GreenLedVal = value.substring(firstClosingBracket , secondClosingBracket).toInt();
  strip2BlueLedVal = value.substring((secondClosingBracket +1) , thirdClosingBracket).toInt();
  //strip1WhiteLedVal = value.substring((thirdClosingBracket +1) , fourthClosingBracket).toInt();



  analogWrite(Strip2GreenPin, strip2GreenLedVal*10.23);
  analogWrite(Strip2RedPin, strip2RedLedVal*10.23);
  analogWrite(Strip2BluePin, strip2BlueLedVal*10.23);
  
     
  }

  if (strcmp(topic,"Strip1Kitchen/white")==0){
   // check for messages on subscribed topics
  payload[length] = '\0';
  Serial.print("Topic: ");
  Serial.println(String(topic));

   // convert payload to String
  String value = String((char*)payload);
  //value.trim();
  Serial.print (value);
  analogWrite(Strip1GreenPin, 0);
  analogWrite(Strip1RedPin, 0);
  analogWrite(Strip1BluePin, 0);
  analogWrite(Strip1WhitePin, value.toInt()*10.23);

}

  if (strcmp(topic,"Strip2Kitchen/white")==0){
   // check for messages on subscribed topics
  payload[length] = '\0';
  Serial.print("Topic: ");
  Serial.println(String(topic));

   // convert payload to String
  String value = String((char*)payload);
  //value.trim();
  Serial.print (value);
  analogWrite(Strip2GreenPin, 0);
  analogWrite(Strip2RedPin, 0);
  analogWrite(Strip2BluePin, 0);
  analogWrite(Strip2WhitePin, value.toInt()*10.23);

}

 }

/////////////////////////////////////////////////////////








/////////////////// SETUP /////////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(Strip1RedPin, OUTPUT);
  pinMode(Strip1GreenPin, OUTPUT);
  pinMode(Strip1BluePin, OUTPUT);
  pinMode(Strip1WhitePin, OUTPUT);
  
  pinMode(Strip2RedPin, OUTPUT);
  pinMode(Strip2GreenPin, OUTPUT);
  pinMode(Strip2BluePin, OUTPUT);
  pinMode(Strip2WhitePin, OUTPUT);
  

 // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

  
    WiFi.setAutoReconnect(false);
    WiFi.begin(ssid, password);
    WiFi.waitForConnectResult();


    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    client.setServer(serverMqtt, 1883);
    client.setCallback(callback);
  
//web server
  
  server.on("/", HTTP_GET, [](){
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex);
    });

      server.on("/httpUpdate", HTTP_POST, [](){
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain","ok");
    //delay(2000);
    updateFW();
  });
  
   server.on("/restart", HTTP_POST, [](){
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain","ok");
    //delay(2000);
    ESP.restart();
  });

  
   server.on("/status", HTTP_OPTIONS, []() {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Access-Control-Max-Age", "10000");
      server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
      server.sendHeader("Access-Control-Allow-Headers", "Access-Control-Allow-Origin, *, X-Requested-With, Content-Type, Accept");
      server.send(200, "text/plain", "" );
    });



   server.on("/status", HTTP_GET, [](){

        long currentTime = millis();
        
        String response;
        timeClient.update();
        String actual_time=timeClient.getFormattedTime();
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
        
        DynamicJsonBuffer jsonBuffer;
        JsonObject &root = jsonBuffer.createObject();

        root["heap"] = ESP.getFreeHeap();
        root["ssid"] = WiFi.SSID();
        root["wifistr"]=WiFi.RSSI();
        root["IP"] = WiFi.localIP().toString();
        root["deviceId"] = deviceId;
        root["deviceType"] = deviceType;
        root["deviceLocation"] = deviceLocation;
        root["deviceScope"] = deviceScope;
        
        root["uptime"] = millis();
        root["lastReason"] = ESP.getResetReason();
        root["chipId"]=ESP.getChipId();
//        root["chipRevision"]=ESP.getChipRevision();
        root["FlashSize"]=ESP.getFlashChipSize();
        root["SDKVersion"]=ESP.getSdkVersion();
        root["eHomeFwVer"]=eHomeFwVer;
        root["time"] = actual_time;
   
        
        root.printTo(response);
        server.send(200,"text/json",response);
});


  /*handling uploading firmware file */
   server.on("/update", HTTP_POST, [](){
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
    },[](){
      HTTPUpload& upload = server.upload();
      if(upload.status == UPLOAD_FILE_START){
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });

  MDNS.addService("http", "tcp", 80);

    Serial.printf("Ready! Open http://%s.local in your browser\n", host);  
  server.begin();

  timeClient.begin();
 
 

 


  sendLog("system rebooted");

  secondTick.attach(1,ISRwatchdog);
}

void loop() {
 server.handleClient();
  
 
 if (WiFi.isConnected()) 
  {
    
   if (!client.connected()) {
    

    
    long now = millis();

    ///work it locally

  
      
    //try to reconnect from time to time
    
    if (now - lastReconnectAttempt > 30000) {
      sendLog("MQTT not connected. Working in local mode");
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {


    client.loop();
    watchdogCount = 0;




}  //end client connected else

}//end wifi connected

   if (! WiFi.isConnected()) {
   
    WiFi.begin(ssid, password);
    WiFi.waitForConnectResult();


    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    ESP.wdtFeed();
   
  }





}
