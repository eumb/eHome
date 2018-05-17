#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32WebServer.h> //https://github.com/nhatuan84/esp32-webserver
#include <ESPmDNS.h>
#include <Update.h>
#include "ArduinoJson.h"
#include "esp_system.h"
#include "FS.h" //esp file system library
#include "esp_wps.h"
#include <PubSubClient.h> //mqtt
#include <rom/rtc.h> //to get the reset reason
#include <NTPClient.h>  https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>  /used for NTP 
#include <RCSwitch.h>
#include <HTTPClient.h>
#include "EEPROM.h"

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addr = 0;
#define EEPROM_SIZE 64


const char* hostName = "eHomeLightUnit1";

const char* ssid = "eHome";
const char* password = "allI0Td3v1c3s!";
const char* loggerHost     = "192.168.2.3";
const char* url      = "/api";
char* serverMqtt = "192.168.2.3";




//const char* ssid = "Yoyo_home";
//const char* password = "sccsa25g";
//const char* loggerHost     = "192.168.1.40";
//const char* url      = "/api";
//char* serverMqtt = "192.168.1.40";






// OTA HTTP Config
String otaHost = "192.168.2.3"; // Host => bucket-name.s3.region.amazonaws.com
int otaPort = 80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String bin = "eHomeLightUnit1.ino.esp32.bin"; // bin file name with a slash in front.
int contentLength = 0;
bool isValidContentType = false;


const char* deviceId = "eHomeLightUnit1";
const char* eHomeFwVer = "1.0";
const char* deviceType = "Light";
const char* deviceLocation = "Ground floor";
const char* deviceScope = "Livingroom lights";

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

//used for alert mechanism
bool alert_sent=false; bool rel1_ON_status =false; bool rel2_ON_status =false; bool rel3_ON_status =false;  
int onTime=0;int alertTime;



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





#define SW1 34
#define SW2 35
#define SW3 25
#define SW1_1 22
#define SW2_1 21
#define SW3_1 19


#define LAMP1 5
#define LAMP2 0
#define LAMP3 4

#define REMOTE 27 

WiFiUDP ntpUDP;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);



int sw1_state,sw2_state,sw3_state,sw1PreviousState=LOW,sw2PreviousState=LOW,sw3PreviousState=LOW;
int sw1_1_state,sw2_1_state,sw3_1_state,sw1_1PreviousState=LOW,sw2_1PreviousState=LOW,sw3_1PreviousState=LOW;

char sw1[25],sw2[25],sw3[25];
char sw1_1[25],sw2_1[25],sw3_1[25];

String sw1_remote,sw2_remote,sw3_remote;
String sw1_1_remote,sw2_1_remote,sw3_1_remote;

bool set_sw1 ,set_sw2,set_sw3;
bool set_sw1_1 ,set_sw2_1,set_sw3_1;

bool set_rel1 ,set_rel2,set_rel3; //remote states
String rel1_remote,rel2_remote,rel3_remote;
char rel1[25],rel2[25],rel3[25];


long lastReconnectAttempt = 0;
long lastConnection = 0;

//watcdog control variable
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule(){
    ets_printf("watchdog interrupt triggered reboot\n");
    
    esp_restart();
}

void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);

      EEPROM.commit();
      
      }
long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
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

 // http.begin("http://192.168.1.40:2000/api/log"); //Specify destination for HTTP request

  
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
  
  if (client.connect("eHomeLightUnit1")) {
    // Once connected, publish an announcement...
    client.publish("log","reconnected; Hello");
    // ... and resubscribe
     
     client.subscribe("eHomeLightUnit1Remote/rel2");
     client.subscribe("eHomeLightUnit1Remote/rel3");
     client.subscribe("eHomeLightUnit1Remote/rel1");
     
     client.publish("eHomeLightUnits/log","eHomeLightUnit1 client connected");
     set_rel1=true;set_rel2=true;set_rel3=true;
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

  if (String( (char *)topic) == "eHomeLightUnit1Remote/rel1"){
    int i;
    for (  i = 0; i<length; i++) 
             {
        rel1[i] = payload[i];
      }
        rel1[i] = '\0';
    
    set_rel1=false; 

    const char *p_payload = rel1;

    rel1_remote = String( (char *)p_payload);

  
    Serial.println(rel1);
 }


 

 
 
  if (String( (char *)topic) == "eHomeLightUnit1Remote/rel2"){
    int i;
    for (  i = 0; i<length; i++) 
      {
        rel2[i] = payload[i];
      }
        rel2[i] = '\0';
    
    set_rel2=false; 

    const char *p_payload = rel2;

    rel2_remote = String( (char *)p_payload);

  
    Serial.println(rel2);
 }

 
  if (String( (char *)topic) == "eHomeLightUnit1Remote/rel3"){
    int i;
    for (  i = 0; i<length; i++) 
      {
        rel3[i] = payload[i];
      }
        rel3[i] = '\0';
           
    set_rel3=false; 

    const char *p_payload = rel3;

    rel3_remote = String( (char *)p_payload);

  
    Serial.println(rel3);
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


  EEPROM.begin(EEPROM_SIZE);//init eeprom

  
  pinMode(SW1, INPUT_PULLDOWN);
  pinMode(SW2, INPUT_PULLDOWN);
  pinMode(SW3, INPUT_PULLDOWN);
  pinMode(SW1_1, INPUT_PULLDOWN);
  pinMode(SW2_1, INPUT_PULLDOWN);
  pinMode(SW3_1, INPUT_PULLDOWN);

  pinMode(REMOTE, OUTPUT  );
  
  pinMode(LAMP1, OUTPUT);
  pinMode(LAMP2, OUTPUT);
  pinMode(LAMP3, OUTPUT);

  digitalWrite(LAMP1,HIGH);
  digitalWrite(LAMP2,HIGH);
  digitalWrite(LAMP3,HIGH);

  

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

  server.on("/settings",[](){
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain","ok");
    //delay(2000);
    
    int val;
    String argument = server.arg("alertTime");
    if (argument.length() != 0) alertTime = argument.toInt();
    
    EEPROMWritelong(addr, alertTime);
    
    Serial.print("alertTime: ");
    Serial.println(EEPROMReadlong(addr));
    
    //sendLog(alertTime.toString());
    sendLog("alertTime changed");
  
    
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
    timerAlarmWrite(timer, 600000, false); //set time in us
    //timerAlarmEnable(timer); //enable interrupt
}

void loop(void){
  server.handleClient();
  
long now = millis();

poll_connection();

 if (WiFi.isConnected()) 
  {
  //timerWrite(timer, 0); //reset timer (feed watchdog)
  sw1_state=digitalRead(SW1);
  sw2_state=digitalRead(SW2);
  sw3_state=digitalRead(SW3);
  sw1_1_state=digitalRead(SW1_1);
  sw2_1_state=digitalRead(SW2_1);
  sw3_1_state=digitalRead(SW3_1);
  
  timerWrite(timer, 0); //reset timer (feed watchdog)

    
   if (!client.connected()) {
    

    
    

    ///work it locally

//    sw1_state=digitalRead(SW1);
//    sw2_state=digitalRead(SW2);
//    sw3_state=digitalRead(SW3);
//    sw1_1_state=digitalRead(SW1_1);
//    sw2_1_state=digitalRead(SW2_1);
//    sw3_1_state=digitalRead(SW3_1);
//
//      
//      if (sw1_state==LOW){
//        digitalWrite(LAMP1,HIGH);
//      }else{
//         digitalWrite(LAMP1,LOW);
//      }
//        if (sw2_state==LOW){
//        digitalWrite(LAMP2,HIGH);
//      }else{
//         digitalWrite(LAMP2,LOW);
//      }
//    
//        if (sw3_state==LOW){
//        digitalWrite(LAMP3,HIGH);
//      }else{
//         digitalWrite(LAMP3,LOW);
//      }
//
//      
//
//       if (sw1_1_state==LOW){
//        digitalWrite(LAMP1,HIGH);
//      }else{
//         digitalWrite(LAMP1,LOW);
//      }
//        if (sw2_1_state==LOW){
//        digitalWrite(LAMP2,HIGH);
//      }else{
//         digitalWrite(LAMP2,LOW);
//      }
//    
//        if (sw3_1_state==LOW){
//        digitalWrite(LAMP3,HIGH);
//      }else{
//         digitalWrite(LAMP3,LOW);
//      }
//



      
      //try to reconnect from time to time
    
    if (now - lastReconnectAttempt > 10000) {
      sendLog("MQTT not connected. Working in local mode");
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {


    client.loop();



 

///////REL1


  //check if the start switch time is higher than the specified interval and that no alert has been sent 
  // and send the message
  if (rel1_ON_status==true){
   
  
    if (now - onTime >= EEPROMReadlong(addr)) { //default on 10 min

     onTime = now;
      Serial.println(onTime);
        if (alert_sent==false){
            client.publish("eHomeLightUnit1/rel1/alert","Alert timeout occured");
            alert_sent=true;
            Serial.println(EEPROMReadlong(addr));
            Serial.println("Alert sent");
           
        }
     
     //Serial.println(alert_sent);
     //Serial.println(onTime);
     alert_sent=false;
    }
   
  }


     //alert variables
  


  ////////////////////////////////////////////////////////


    if ( (sw1_1_state==HIGH && sw1_1_state!=sw1_1PreviousState)  ){ // 
       Serial.println("REL1 HIGH from sw 1_1");
     digitalWrite(LAMP1,LOW);
     mySwitch.send("000000000000000000001111"); 
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel1","ON");
    
    

    sw1_1PreviousState=HIGH;

    //alert variables
    rel1_ON_status=true;
    onTime = now;
  
   
  }


 if ( (sw1_1_state==LOW && sw1_1_state!=sw1_1PreviousState) ){   //
     Serial.println("REL1 Low from sw1_1");
   digitalWrite(LAMP1,HIGH);
   mySwitch.send("000000000000000000001110");
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel1","OFF");
       
    
    sw1_1PreviousState=LOW; 
    //alert variables
    rel1_ON_status=false;
 
   
    
  }
 

  ////////////////////////////////////////////////////////



    if ( (sw1_state==HIGH && sw1_state!=sw1PreviousState)  ){ // 
       Serial.println("REL1 HIGH from sw1");
      digitalWrite(LAMP1,LOW);
      mySwitch.send("000000000000000000001111");
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel1","ON");
    
    
    sw1PreviousState=HIGH;


    //alert variables
    rel1_ON_status=true;
    onTime = now;
   
   
   
  }
   
  
  
  if ( (sw1_state==LOW && sw1_state!=sw1PreviousState) ){   //
     Serial.println("REL1 Low from sw1");
   digitalWrite(LAMP1,HIGH);
   mySwitch.send("000000000000000000001110"); 
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel1","OFF");
       
    
    sw1PreviousState=LOW;
 
    //alert variables
    rel1_ON_status=false;
  
   
    
  }







if (set_rel1==false){   //if a new remote message arrived 
  //Serial.println("processing incomming message");
  if (rel1_remote=="OFF"){
      delay(100);
      mySwitch.send("000000000000000000001110");  //process it
      client.publish("eHomeLightUnit1/rel1","OFF");
      digitalWrite(LAMP1,HIGH);
      Serial.println("remote OFF");
      set_rel1=true;
       client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL1 remote control");
       
       //alert variables
       rel1_ON_status=false;
    
  
  }
    if (rel1_remote=="ON"){
      delay(100);
      mySwitch.send("000000000000000000001111");  //process it
      client.publish("eHomeLightUnit1/rel1","ON");
      digitalWrite(LAMP1,LOW);
      Serial.println("remote ON");
      set_rel1=true;
      client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL1 remote control");
      
      //alert variables

      rel1_ON_status=true;
      onTime = now;
    
  }
 

}




///////REL2


  //check if the start switch time is higher than the specified interval and that no alert has been sent 
  // and send the message
  if (rel2_ON_status==true){
   
  
    if (now - onTime >= EEPROMReadlong(addr)) { //default on 10 min

     onTime = now;
      Serial.println(onTime);
        if (alert_sent==false){
            client.publish("eHomeLightUnit1/rel2/alert","Alert timeout occured");
            alert_sent=true;
            Serial.println(EEPROMReadlong(addr));
            Serial.println("Alert sent");
           
        }
     
     //Serial.println(alert_sent);
     //Serial.println(onTime);
     alert_sent=false;
    }
   
  }


     //alert variables
  


  ////////////////////////////////////////////////////////


    if ( (sw2_1_state==HIGH && sw2_1_state!=sw2_1PreviousState)  ){ // 
       Serial.println("REL2 HIGH from sw 2_1");
       mySwitch.send("000000000000000000000111");
       digitalWrite(LAMP2,LOW);
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel2","ON");
    
    

    sw2_1PreviousState=HIGH;

    //alert variables
    rel2_ON_status=true;
    onTime = now;
    
   
  }


 if ( (sw2_1_state==LOW && sw2_1_state!=sw2_1PreviousState) ){   //
     Serial.println("REL2 Low from sw2_1");
      mySwitch.send("000000000000000000000110");
   digitalWrite(LAMP2,HIGH);
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel2","OFF");
       
    

    sw2_1PreviousState=LOW; 
    //alert variables
    rel2_ON_status=false;

    
  }

  

  ////////////////////////////////////////////////////////



    if ( (sw2_state==HIGH && sw2_state!=sw2PreviousState)  ){ // 
       Serial.println("REL2 HIGH from sw2");
       mySwitch.send("000000000000000000000111");
       digitalWrite(LAMP2,LOW);
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel2","ON");
    
    
    sw2PreviousState=HIGH;


    //alert variables
    rel2_ON_status=true;
    onTime = now;
    
   
  }
   
  
  
  if ( (sw2_state==LOW && sw2_state!=sw2PreviousState) ){   //
     Serial.println("REL2 Low from sw2");
      mySwitch.send("000000000000000000000110");
   digitalWrite(LAMP2,HIGH);
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel2","OFF");
       
    
    sw2PreviousState=LOW;
 
    //alert variables
    rel2_ON_status=false;

    
  }







if (set_rel2==false){   //if a new remote message arrived 
  //Serial.println("processing incomming message");
  if (rel2_remote=="OFF"){
      mySwitch.send("000000000000000000000110");  //process it
      client.publish("eHomeLightUnit1/rel2","OFF");
      digitalWrite(LAMP2,HIGH);
      Serial.println("remote OFF");
      set_rel2=true;
       client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL2 remote control");
       
       //alert variables
       rel2_ON_status=false;
    
  
  }
    if (rel2_remote=="ON"){
      mySwitch.send("000000000000000000000111");  //process it
      client.publish("eHomeLightUnit1/rel2","ON");
      digitalWrite(LAMP2,LOW);
      Serial.println("remote ON");
      set_rel2=true;
      client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL2 remote control");
      
      //alert variables

      rel2_ON_status=true;
      onTime = now;
    
  }
 

}









///////REL3


  //check if the start switch time is higher than the specified interval and that no alert has been sent 
  // and send the message
  if (rel3_ON_status==true){
   
  
    if (now - onTime >= EEPROMReadlong(addr)) { //default on 10 min

     onTime = now;
      Serial.println(onTime);
        if (alert_sent==false){
            client.publish("eHomeLightUnit1/rel3/alert","Alert timeout occured");
            alert_sent=true;
            Serial.println(EEPROMReadlong(addr));
            Serial.println("Alert sent");
           
        }
     
     //Serial.println(alert_sent);
     //Serial.println(onTime);
     alert_sent=false;
    }
   
  }


     //alert variables
  


  ////////////////////////////////////////////////////////


    if ( (sw3_1_state==HIGH && sw3_1_state!=sw3_1PreviousState)  ){ // 
       Serial.println("REL3 HIGH from sw 3_1");
       mySwitch.send("000000000000000000001011");
       digitalWrite(LAMP3,LOW);
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel3","ON");
    
    

    sw3_1PreviousState=HIGH;

    //alert variables
    rel3_ON_status=true;
    onTime = now;
    
   
  }


 if ( (sw3_1_state==LOW && sw3_1_state!=sw3_1PreviousState) ){   //
     Serial.println("REL3 Low from sw3_1");
     mySwitch.send("000000000000000000001010"); 
   digitalWrite(LAMP3,HIGH);
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel3","OFF");
       
    

    sw3_1PreviousState=LOW; 
    //alert variables
    rel3_ON_status=false;

    
  }

  

  ////////////////////////////////////////////////////////



    if ( (sw3_state==HIGH && sw3_state!=sw3PreviousState)  ){ // 
       Serial.println("REL3 HIGH from sw3");
       mySwitch.send("000000000000000000001011");
       digitalWrite(LAMP3,LOW);
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel3","ON");
    
    
    sw3PreviousState=HIGH;


    //alert variables
    rel3_ON_status=true;
    onTime = now;
    
   
  }
   
  
  
  if ( (sw3_state==LOW && sw3_state!=sw3PreviousState) ){   //
     Serial.println("REL3 Low from sw3");
     mySwitch.send("000000000000000000001010"); 
   digitalWrite(LAMP3,HIGH);
          Serial.println("Publishing");
          client.publish("eHomeLightUnit1/rel3","OFF");
       
    
    sw3PreviousState=LOW;
 
    //alert variables
    rel3_ON_status=false;

    
  }







if (set_rel3==false){   //if a new remote message arrived 
  //Serial.println("processing incomming message");
  if (rel3_remote=="OFF"){
      mySwitch.send("000000000000000000001010");  //process it
      client.publish("eHomeLightUnit1/rel3","OFF");
      digitalWrite(LAMP3,HIGH);
      Serial.println("remote OFF");
      set_rel3=true;
       client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL3 remote control");
       
       //alert variables
       rel3_ON_status=false;
    
  
  }
    if (rel3_remote=="ON"){
      mySwitch.send("000000000000000000001011");  //process it
      client.publish("eHomeLightUnit1/rel3","ON");
      digitalWrite(LAMP3,LOW);
      Serial.println("remote ON");
      set_rel3=true;
      client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL3 remote control");
      
      //alert variables

      rel3_ON_status=true;
      onTime = now;
    
  }
 

}







//////////////////////////////////////////////////////////////////////////// 

}  //end client connected else

}//end wifi connected

   if (! WiFi.isConnected()) {
    timerWrite(timer, 0); //reset timer (feed watchdog)

      sw1_state=digitalRead(SW1);
      sw2_state=digitalRead(SW2);
      sw3_state=digitalRead(SW3);
      //sw4_state=digitalRead(SW4);
      if (sw1_state==LOW){
        digitalWrite(LAMP1,HIGH);
      }else{
         digitalWrite(LAMP1,LOW);
      }
        if (sw2_state==LOW){
        digitalWrite(LAMP2,HIGH);
      }else{
         digitalWrite(LAMP2,LOW);
      }
    
        if (sw3_state==LOW){
        digitalWrite(LAMP3,HIGH);
      }else{
         digitalWrite(LAMP3,LOW);
      }

//          if (sw4_state==LOW){
//        digitalWrite(LAMP4,HIGH);
//      }else{
//         digitalWrite(LAMP4,LOW);
//      }
    
    
   
  }


 timerWrite(timer, 0); //reset timer (feed watchdog)


}

