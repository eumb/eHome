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

#define Strip3RedPin D5
#define Strip3GreenPin D6
#define Strip3BluePin D7
#define Strip3WhitePin D8



const char* serverIndex = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update'>"
    "<input type='submit' value='Update'>"
"</form>"
"<div id='prg'>progress: 0%</div>"
"<script>"
"$('form').submit(function(e){"
    "e.preventDefault();"
      "var form = $('#upload_form')[0];"
      "var data = new FormData(form);"
      " $.ajax({"
            "url: '/update',"
            "type: 'POST',"               
            "data: data,"
            "contentType: false,"                  
            "processData:false,"  
            "xhr: function() {"
                "var xhr = new window.XMLHttpRequest();"
                "xhr.upload.addEventListener('progress', function(evt) {"
                    "if (evt.lengthComputable) {"
                        "var per = evt.loaded / evt.total;"
                        "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
                    "}"
               "}, false);"
               "return xhr;"
            "},"                                
            "success:function(d, s) {"    
                "console.log('success!')"
           "},"
            "error: function (a, b, c) {"
            "}"
          "});"
"});"
"</script>";

int Strip3RedLedVal;
int Strip3GreenLedVal;
int Strip3BlueLedVal;
int Strip3WhiteLedVal;


const char* host = "eHomeRGBWStrip3";

const char* ssid     = "eHome";
const char* password = "allI0Td3v1c3s!";
IPAddress local_IP(192, 168, 2, 27);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(192, 168, 2, 1); //optional
IPAddress secondaryDNS(8, 8, 8, 8); //optional


char* serverMqtt = "192.168.2.10";
const char* binFile="eHomeRGBWStrip3.ino.d1_mini.bin";
const char* url      = "/api";
const char* deviceId = "eHomeRGBWStrip3";
const char* eHomeFwVer = "1.0";
const char* deviceType = "LEDStrip";
const char* deviceLocation = "Ground Floor";
const char* deviceScope = "Livingroom LED Strip";


ESP8266WebServer server(80);



WiFiUDP ntpUDP;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);



WiFiClient espClient;

PubSubClient client(espClient);

long lastReconnectAttempt = 0;
long lastConnection = 0;

//watcdog control variable
Ticker secondTick;
volatile int watchdogCount = 0;


void ISRwatchdog(){

  watchdogCount++;
  if(watchdogCount == 50){
    Serial.println("watchdog bites");
    sendLog("WatchDog triggered. Going to reset");
    ESP.restart();
  }
}

void updateFW(){
  sendLog("Updating....");
  ESPhttpUpdate.rebootOnUpdate(true);
  t_httpUpdate_return ret=ESPhttpUpdate.update("192.168.2.10", 80, "/eHomeRGBWStrip3.bin");
  Serial.print("ret ");Serial.println(ret);


   switch(ret) {
            case HTTP_UPDATE_FAILED:
                
                Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                Serial.println(binFile);
                sendLog("HTTP_UPDATE_FAILED Error");
                sendLog(ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                sendLog("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                Serial.println("HTTP_UPDATE_OK");
                sendLog("HTTP_UPDATE_OK");
                break;
        }
        
}

////////////////////////  Log ////////////////////////////////////////////


void sendLog(String message){

  HTTPClient http;
  
  timeClient.update();
  String actual_time=timeClient.getFormattedTime();
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  
  root["time"] = actual_time;
  root["deviceId"] = deviceId;
  root["message"] = message;


  
  char JSONmessageBuffer[300];

  root.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  
  
  http.begin("http://192.168.2.10:2000/api/log"); //Specify destination for HTTP request
  http.addHeader("Content-Type", "application/json"); //Specify content-type header
  int httpResponseCode = http.POST(JSONmessageBuffer); //Send the actual POST request
  
  if(httpResponseCode>0){
   
      String response = http.getString();  //Get the response to the request
   
      Serial.println(httpResponseCode);   //Print return code
      Serial.println(response);           //Print request answer
   
  }else{
   
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
   
  }

  
}

//////////////////////////////////////////////////////////////////////////

    
///////////////////////   MQTT connection management  //////////////////////
//reconnect on MQTT connection lost
boolean reconnect() {
  
  if (client.connect("eHomeRGBWStrip3")) {
    // Once connected, publish an announcement...
    client.publish("log","reconnected; Hello from eHomeRGBWStrip3");
    // ... and resubscribe
     client.subscribe("eHomeRGBWStrip3/colorValues");

     client.subscribe("eHomeRGBWStrip3/white");

     client.publish("eHomeRGBWStrip3/log","eHomeRGBWStrip3 client connected");

     Serial.println("MQTT connected");
     sendLog("MQTT connected");
  
  }
  return client.connected();
}

///////////////////////////////////////////////////////////////////////////////





////////////////  Wifi connection management  ///////////////////////

/////////////////////////////////////////////////////////////////


//////////////// MQTT callback ///////////////////////////////////


void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if(strcmp(topic,"eHomeRGBWStrip3/colorValues")==0){
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
  Strip3RedLedVal = value.substring(0 , (firstClosingBracket - 1)).toInt();
  Strip3GreenLedVal = value.substring(firstClosingBracket , secondClosingBracket).toInt();
  Strip3BlueLedVal = value.substring((secondClosingBracket +1) , thirdClosingBracket).toInt();




  analogWrite(Strip3GreenPin, Strip3GreenLedVal*10.23);
  analogWrite(Strip3RedPin, Strip3RedLedVal*10.23);
  analogWrite(Strip3BluePin, Strip3BlueLedVal*10.23);

     
  }



  if (strcmp(topic,"eHomeRGBWStrip3/white")==0){
   // check for messages on subscribed topics
  payload[length] = '\0';
  Serial.print("Topic: ");
  Serial.println(String(topic));

   // convert payload to String
  String value = String((char*)payload);
  //value.trim();
  Serial.print (value);
  analogWrite(Strip3GreenPin, 0);
  analogWrite(Strip3RedPin, 0);
  analogWrite(Strip3BluePin, 0);
  analogWrite(Strip3WhitePin, value.toInt()*10.23);

}

 

 }

/////////////////////////////////////////////////////////








/////////////////// SETUP /////////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(Strip3RedPin, OUTPUT);
  pinMode(Strip3GreenPin, OUTPUT);
  pinMode(Strip3BluePin, OUTPUT);
  pinMode(Strip3WhitePin, OUTPUT);
  
 
  digitalWrite(Strip3WhitePin,LOW);
 
 // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

  
    WiFi.setAutoReconnect(false);
    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
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
      Serial.println("Root web server called");
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

  //MDNS.addService("http", "tcp", 80);

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
    //ESP.wdtFeed();




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
