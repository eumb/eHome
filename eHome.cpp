/*
 * ----------------------------------------------------------------------------
 * eHome.cpp
 * ----------------------------------------------------------------------------
 *
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE.
 * The copyright notice above does not evidence any actual or intended
 * publication of such source code.
 *
 * Not to be disclosed or used except in accordance with applicable agreements.
 *
 * ----------------------------------------------------------------------------
 *  Created on: March 21, 2018
 *      Author: yoyo
 * ----------------------------------------------------------------------------
 */

#ifndef DEBUG
#define DEBUG 1
#endif

#include "eHome.h"

namespace
{

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


const char* ssid     = "eHome";
const char* password = "allI0Td3v1c3s!";
char* serverMqtt = "192.168.2.3";


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
    ESP.reset();
  }
}

void updateFW(){
  sendLog("Updating....");
  ESPhttpUpdate.rebootOnUpdate(true);
  t_httpUpdate_return ret=ESPhttpUpdate.update("192.168.2.3", 80, "/RGBWStripKitchen.bin");
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
  
  
  http.begin("http://192.168.2.3:2000/api/log"); //Specify destination for HTTP request
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
  
  if (client.connect("RGBWStripKitchen")) {
    // Once connected, publish an announcement...
    client.publish("log","reconnected; Hello from RGBWStripKitchen");
    // ... and resubscribe
     client.subscribe("RGBWStrip1Kitchen/colorValues");
     client.subscribe("RGBWStrip2Kitchen/colorValues");
     client.subscribe("Strip1Kitchen/white");
     client.subscribe("Strip2Kitchen/white");
     client.publish("RGBWStripKitchen/log","RGBWStripKitchen client connected");

     Serial.println("MQTT connected");
     sendLog("MQTT connected");
  
  }
  return client.connected();
}

///////////////////////////////////////////////////////////////////////////////


}


