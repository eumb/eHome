#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32WebServer.h> //https://github.com/nhatuan84/esp32-webserver
#include <ESPmDNS.h>
#include <Update.h>
#include "ArduinoJson.h"    //download from library
#include "esp_system.h"
#include "FS.h" //esp file system library
#include "esp_wps.h"
#include <PubSubClient.h> //mqtt /              cx /download from library
#include <rom/rtc.h> //to get the reset reason
#include <NTPClient.h>  ////download from library or https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>  /used for NTP 
#include <RCSwitch.h> //https://github.com/sui77/rc-switch/releases/tag/2.6.2
#include <HTTPClient.h>


const char* hostName = "eHomeShadesUnit1";
const char* ssid = "eHome";
const char* password = "allI0Td3v1c3s!";
const char* loggerHost     = "192.168.2.3";
const char* url      = "/api";
char* serverMqtt = "192.168.2.3";

const char* deviceId = "eHomeShadesUnit1";


// OTA HTTP Config
String otaHost = "192.168.2.3"; // Host => bucket-name.s3.region.amazonaws.com
int otaPort = 80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String bin = "eHomeShadesUnit1.ino.esp32.bin"; // bin file name with a slash in front.
int contentLength = 0;
bool isValidContentType = false;

const char* eHomeFwVer = "1.0";
const char* deviceType = "Light";
const char* deviceLocation = "Ground floor";
const char* deviceScope = "Livingroom shades";

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}



WiFiClient espClient;

PubSubClient client(espClient);


ESP32WebServer server(80);


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





#define SW1 33
#define SW2 32
#define SW3 35



#define MotUp 16
#define MotDown 17



#define REMOTE 25  //switches remote connection

WiFiUDP ntpUDP;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);


//bool sw1,sw2,sw3,remote_set;

int sw1_state,sw2_state,sw3_state,sw4_state,sw1PreviousState=LOW,sw2PreviousState=LOW,sw3PreviousState=LOW,sw4PreviousState=LOW;
char sw1[25],sw2[25],sw3[25],sw4[25];
String sw1_remote,sw2_remote,sw3_remote,sw4_remote;

bool set_sw1 ,set_sw2,set_sw3,set_sw4 ;

long lastReconnectAttempt = 0;
long lastConnection = 0;

//watcdog control variable
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule(){
    ets_printf("watchdog interrupt triggered reboot\n");
    
    esp_restart();
}

String reset_reason;

void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : reset_reason="Vbat power on reset";break;          /**<1,  Vbat power on reset*/
    case 3 : reset_reason="Software reset digital core";break;               /**<3,  Software reset digital core*/
    case 4 : reset_reason="Legacy watch dog reset digital core";break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : reset_reason="Deep Sleep reset digital core";break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : reset_reason="Reset by SLC module, reset digital core";break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : reset_reason="Timer Group0 Watch dog reset digital core";break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : reset_reason="Timer Group1 Watch dog reset digital core";break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : reset_reason="RTC Watch dog Reset digital core";break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : reset_reason="Instrusion tested to reset CPU";break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : reset_reason="Time Group reset CPU";break;       /**<11, Time Group reset CPU*/
    case 12 : reset_reason="Software reset CPU";break;          /**<12, Software reset CPU*/
    case 13 : reset_reason="RTC Watch dog Reset CPU";break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : reset_reason="for APP CPU, reseted by PRO CPU";break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : reset_reason="Reset when the vdd voltage is not stable";break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : reset_reason="RTC Watch dog reset digital core and rtc module";break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : reset_reason=="NO_MEAN";
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
  //int httpResponseCode = http.POST("POSTING from ESP32"); //Send the actual POST request
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
  
  if (client.connect("eHomeShadeUnit1")) {
    // Once connected, publish an announcement...
    client.publish("log","reconnected; Hello");
    // ... and resubscribe
     client.subscribe("eHomeShadesUnit1/sw1");
     client.subscribe("eHomeShadesUnit1/sw2");
     client.subscribe("eHomeShadesUnit1/sw3");
   
     client.publish("eHomeShadesUnits/log","eHomeShadesUnit1 client connected");
     set_sw1=true;set_sw2=true;set_sw3=true;set_sw4=true;
     Serial.println("MQTT connected");
     sendLog("MQTT connected");
  
  }
  return client.connected();
}

///////////////////////////////////////////////////////////////////////////////





////////////////  Wifi connection management  ///////////////////////
static bool sta_was_connected = false;

static void poll_connection(void) {                                         
#ifdef ESP32
  // this is a crude workaround due to the inability of
  // the ESP32 to reconnect to an accesspoint automatically.
  //
  // https://github.com/espressif/arduino-esp32/issues/653

  static uint32_t ms = millis();

  if (!WiFi.isConnected() && (millis() - ms) > 1000 * 5) {
    ms = millis();
    sendLog("Wifi connection lost. reconnecting ..........");
    if (sta_was_connected) WiFi.reconnect(); else WiFi.begin();
  }
#endif
}
/////////////////////////////////////////////////////////////////


//////////////// MQTT callback ///////////////////////////////////


void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if (String( (char *)topic) == "eHomeShadesUnit1/sw1"){
    int i;
    for (  i = 0; i<length; i++) 
      {
        sw1[i] = payload[i];
      }
        sw1[i] = '\0';
    
    set_sw1=false;

    const char *p_payload = sw1;
    sw1_remote = String( (char *)p_payload);

  
    Serial.println(sw1);
 }

  if (String( (char *)topic) == "eHomeShadesUnit1/sw2"){
    int i;
    for (  i = 0; i<length; i++) 
      {
        sw2[i] = payload[i];
      }
        sw2[i] = '\0';
    
    set_sw2=false;

    const char *p_payload = sw2;
    sw2_remote = String( (char *)p_payload);

  
    Serial.println(sw2);
 }




   if (String( (char *)topic) == "eHomeShadesUnit1/sw3"){
    int i;
    for (  i = 0; i<length; i++) 
      {
        sw3[i] = payload[i];
      }
        sw3[i] = '\0';
    
    set_sw3=false;

    const char *p_payload = sw3;
    sw3_remote = String( (char *)p_payload);

  
    Serial.println(sw3);
 }

}

/////////////////////////////////////////////////////////

void updateFW() {
  
  
  
  Serial.println("Connecting to: " + String(otaHost));
  // Connect to S3
  if (espClient.connect(otaHost.c_str(), otaPort)) {
    // Connection Succeed.
    // Fecthing the bin
    Serial.println("Fetching Bin: " + String(bin));

    // Get the contents of the bin file
    espClient.print(String("GET /") + bin + " HTTP/1.1\r\n" +
                 "Host: " + otaHost + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n" );

    // Check what is being sent
    //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
    //                 "Host: " + host + "\r\n" +
    //                 "Cache-Control: no-cache\r\n" +
    //                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (espClient.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Client Timeout !");
        espClient.stop();
        return;
      }
    }
    // Once the response is available,
    // check stuff

    /*
       Response Structure
        HTTP/1.1 200 OK
        x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
        x-amz-request-id: 2D56B47560B764EC
        Date: Wed, 14 Jun 2017 03:33:59 GMT
        Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
        ETag: "d2afebbaaebc38cd669ce36727152af9"
        Accept-Ranges: bytes
        Content-Type: application/octet-stream
        Content-Length: 357280
        Server: AmazonS3
                                   
        {{BIN FILE CONTENTS}}

    */
    while (espClient.available()) {
      // read line till /n
      String line = espClient.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty,
      // this is end of headers
      // break the while and feed the
      // remaining `client` to the
      // Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atoi((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.println("Got " + String(contentLength) + " bytes from server");
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    // Connect to S3 failed
    // May be try?
    // Probably a choppy network?
    Serial.println("Connection to " + String(otaHost) + " failed. Please check your setup");
    // retry??
    // execOTA();
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);

    // If yes, begin
    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      // No activity would appear on the Serial monitor
      // So be patient. This may take 2 - 5mins to complete
      size_t written = Update.writeStream(espClient);

      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
        // retry??
        // execOTA();
      }

      if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      // not enough space to begin OTA
      // Understand the partitions and
      // space availability
      Serial.println("Not enough space to begin OTA");
      espClient.flush();
    }
  } else {
    Serial.println("There was no content in the response");
    espClient.flush();
  }

}
///////////////////////////////////////////////////////

//////////////////////// RCSwitch declarations ////////////

RCSwitch mySwitch = RCSwitch();

/////////////////////////////////////////////////////////




void setup(void){
     // put your setup code here, to run once:
  Serial.begin(115200);


  
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);


  pinMode(REMOTE, OUTPUT  );
  pinMode(MotUp, OUTPUT);
  pinMode(MotDown, OUTPUT);
 

  digitalWrite(MotUp,LOW);
  digitalWrite(MotDown,LOW);


  

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

    /*use mdns for host name resolution*/
    if (!MDNS.begin(hostName)) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");
    /*return index page which is stored in serverIndex */
   
   
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
    server.send(200, "text/plain","device restart");
    //delay(2000);
    esp_restart();
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
        
        uint64_t  chipid=ESP.getEfuseMac(); 
        
        print_reset_reason(rtc_get_reset_reason(0));
        
        root["heap"] = ESP.getFreeHeap();
        root["ssid"] = WiFi.SSID();
        root["wifistr"]=WiFi.RSSI();
        root["IP"] = WiFi.localIP().toString();
        root["deviceId"] = deviceId;
        root["deviceType"] = deviceType;
        root["deviceLocation"] = deviceLocation;
        root["deviceScope"] = deviceScope;
        root["uptime"] = millis();
        root["lastReason"] = reset_reason;
        root["chipId"]=(uint32_t)chipid;
        root["chipRevision"]=ESP.getChipRevision();
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
      esp_wifi_wps_disable(); ESP.restart();
    },[](){
      HTTPUpload& upload = server.upload();
      if(upload.status == UPLOAD_FILE_START){
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if(!Update.begin(UPDATE_SIZE_UNKNOWN)){//start with max available size
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        /* flashing firmware to ESP*/
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
      }
    });


    
    server.begin();

    timeClient.begin();

    mySwitch.enableTransmit(REMOTE);



    timer = timerBegin(0, 80, true); //timer 0, div 80
    timerAttachInterrupt(timer, &resetModule, true);
    timerAlarmWrite(timer, 6000000, false); //set time in us
    //timerAlarmEnable(timer); //enable interrupt
}

void loop(void){
  server.handleClient();
  

poll_connection();

 if (WiFi.isConnected()) 
  {
//do something

  sw1_state=digitalRead(SW1);
  sw2_state=digitalRead(SW2);
  sw3_state=digitalRead(SW3);
  
  timerWrite(timer, 0); //reset timer (feed watchdog)

    
   if (!client.connected()) {
    

    
    long now = millis();

    ///work it locally
    


    sw1_state=digitalRead(SW1);
    //Serial.println(sw1_state);
    sw2_state=digitalRead(SW2);
    //Serial.println(sw2_state);
   
      
      if (sw1_state==HIGH){
        digitalWrite(MotUp,HIGH);
      }else{
         digitalWrite(MotUp,LOW);
      }
        if (sw2_state==LOW){
        digitalWrite(MotDown,LOW);
      }else{
         digitalWrite(MotDown,HIGH);
      }
    
    
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

    if (sw1_remote=="ON"){

       digitalWrite(MotUp,HIGH);
       //Serial.println("Jaluzea UP");
    }else if (sw1_remote=="OFF"){
       digitalWrite(MotUp,LOW);
       //Serial.println("Jaluzea UP STOP");
    }
    if (sw2_remote=="ON"){
       digitalWrite(MotDown,HIGH);
       //Serial.println("Jaluzea Down");
    }
     else if (sw2_remote=="OFF"){
       digitalWrite(MotDown,LOW);
       //Serial.println("Jaluzea Down STOP");
    }

}  //end ! client connected else

}//end wifi connected


   if (! WiFi.isConnected()) {
    timerWrite(timer, 0); //reset timer (feed watchdog)

     
    //work it without wifi

    sw1_state=digitalRead(SW1);
    sw2_state=digitalRead(SW2);

   
      
      if (sw1_state==HIGH){
        digitalWrite(MotUp,HIGH);
      }else{
         digitalWrite(MotUp,LOW);
      }
        if (sw2_state==LOW){
        digitalWrite(MotDown,LOW);
      }else{
         digitalWrite(MotDown,HIGH);
      }
    
   
  }


 timerWrite(timer, 0); //reset timer (feed watchdog)


}

