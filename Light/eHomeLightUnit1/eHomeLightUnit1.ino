//14.06
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
<<<<<<< HEAD

IPAddress local_IP(192, 168, 2, 21);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(192, 168, 2, 1); //optional
IPAddress secondaryDNS(8, 8, 8, 8); //optional

=======
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
const char* loggerHost     = "192.168.2.10";
const char* url      = "/api";
char* serverMqtt = "192.168.2.10";


// OTA HTTP Config
String otaHost = "192.168.2.10"; // Host => bucket-name.s3.region.amazonaws.com
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
long now;


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





#define SW1b_1 36
#define SW1b_2 39
#define SW1b_3 34
#define SW7b_1 35
#define SW7b_2 32
#define SW7b_3 33
#define SW8b_1 25
#define SW8b_2 26
#define SW8b_3 14
#define SW9b_1 23
#define SW9b_2 22
#define SW9b_3 21
#define SW6b_1 19
#define SW6b_2 18
#define SW6b_3 4

//TODO add spot living poz 6 


#define LAMP1 15   //rand 1
#define LAMP2 16  //rand 2
#define LAMP3 17 //rand 3

#define REMOTE 27

WiFiUDP ntpUDP;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);



int SW7b_1_state,SW7b_2_state,SW7b_3_state ,SW7b_1PreviousState=LOW,SW7b_2PreviousState=LOW,SW7b_3PreviousState=LOW;
int SW8b_1_state,SW8b_2_state,SW8b_3_state,SW8b_1PreviousState=LOW,SW8b_2PreviousState=LOW ,SW8b_3PreviousState=LOW ;
int SW9b_1_state,SW9b_2_state,SW9b_3_state,SW9b_1PreviousState=LOW,SW9b_2PreviousState=LOW ,SW9b_3PreviousState=LOW ;
int SW1b_1_state,SW1b_2_state,SW1b_3_state,SW1b_1PreviousState=LOW,SW1b_2PreviousState=LOW ,SW1b_3PreviousState=LOW ;
int SW6b_1_state,SW6b_2_state,SW6b_3_state,SW6b_1PreviousState=LOW,SW6b_2PreviousState=LOW ,SW6b_3PreviousState=LOW ;

bool set_SW7b_2 ,set_SW8b_1,set_SW8b_3,set_SW7b_1 ,set_SW7b_3,set_SW8b_2,set_SW6b_2;


bool set_rel1 ,set_rel2,set_rel3; //remote states
String rel1_remote,rel2_remote,rel3_remote;
char rel1[25],rel2[25],rel3[25];
bool on_executed=false;
bool off_executed=false;

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


  http.begin("http://192.168.2.10:2000/api/log"); //Specify destination for HTTP request
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

  pinMode(SW1b_1, INPUT_PULLDOWN);
  pinMode(SW1b_2, INPUT_PULLDOWN);
  pinMode(SW1b_3, INPUT_PULLDOWN);
  pinMode(SW7b_1, INPUT_PULLDOWN);
  pinMode(SW7b_2, INPUT_PULLDOWN);
  pinMode(SW7b_3, INPUT_PULLDOWN);
  pinMode(SW8b_1, INPUT_PULLDOWN);
  pinMode(SW8b_2, INPUT_PULLDOWN);
  pinMode(SW8b_3, INPUT_PULLDOWN);
  pinMode(SW9b_1, INPUT_PULLDOWN);
  pinMode(SW9b_2, INPUT_PULLDOWN);
  pinMode(SW9b_3, INPUT_PULLDOWN);
  pinMode(SW6b_1, INPUT_PULLDOWN);
  pinMode(SW6b_2, INPUT_PULLDOWN);
  pinMode(SW6b_3, INPUT_PULLDOWN);


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
    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
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

now = millis();

poll_connection();

 if (WiFi.isConnected())
  {
  //timerWrite(timer, 0); //reset timer (feed watchdog)
  SW7b_1_state=digitalRead(SW7b_1);
  SW7b_2_state=digitalRead(SW7b_2);
  SW7b_3_state=digitalRead(SW7b_3);

  SW8b_1_state=digitalRead(SW8b_1);
  SW8b_2_state=digitalRead(SW8b_2);
  SW8b_3_state=digitalRead(SW8b_3);

  SW9b_1_state=digitalRead(SW9b_1);
  SW9b_2_state=digitalRead(SW9b_2);
  SW9b_3_state=digitalRead(SW9b_3);

  SW1b_1_state=digitalRead(SW1b_1);
  SW1b_2_state=digitalRead(SW1b_2);
  SW1b_3_state=digitalRead(SW1b_3);

  SW6b_1_state=digitalRead(SW6b_1);
  SW6b_2_state=digitalRead(SW6b_2);
  SW6b_3_state=digitalRead(SW6b_3);
  
  timerWrite(timer, 0); //reset timer (feed watchdog)


   if (!client.connected()) {




<<<<<<< HEAD
SW1b_1_on();
SW1b_1_off();
SW7b_1_on();
SW7b_1_off();
SW8b_1_on();
SW8b_1_off();
SW9b_1_on();
SW9b_1_off();
SW6b_1_on();
SW6b_1_off();

  SW1b_2_on();
  SW1b_2_off();
  SW7b_2_on();
  SW7b_2_off();
  SW8b_2_on();
  SW8b_2_off();
  SW9b_2_on();
  SW9b_2_off();
  SW6b_2_on();
  SW6b_2_off();


  SW1b_3_on();
  SW1b_3_off();
  SW7b_3_on();
  SW7b_3_off();
  SW8b_3_on();
  SW8b_3_off();
  SW9b_3_on();
  SW9b_3_off();
  SW6b_3_on();
  SW6b_3_off();
=======

// TODO

>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca


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
<<<<<<< HEAD


     //alert variables


//REL1 //rand1
////////////////////////////////////////////////////////
=======


     //alert variables
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca

SW7b_1_on();
//SW8b_1_on();
SW9b_1_on();
SW6b_1_on();
SW1b_1_on();

<<<<<<< HEAD

SW7b_1_off();
SW8b_1_off();
SW9b_1_off();
SW6b_1_off();
SW1b_1_off();

=======
//REL1 //rand1
////////////////////////////////////////////////////////
SW1b_1_on();
SW1b_1_off();
SW7b_1_on();
SW7b_1_off();
SW8b_1_on();
SW8b_1_off();
SW9b_1_on();
SW9b_1_off();
SW6b_1_on();
SW6b_1_off();
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca

if (set_rel1==false){   //if a new remote message arrived
  //Serial.println("processing incomming message");
  if (rel1_remote=="OFF"){
      delay(100);
<<<<<<< HEAD
=======
      mySwitch.send("000000000000000000001110");  //process it
    //  client.publish("eHomeLightUnit1/rel1","OFF");
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
      digitalWrite(LAMP1,HIGH);
      mySwitch.send("000000000000000000001110");  //process it
    //  client.publish("eHomeLightUnit1/rel1","OFF");
      
      Serial.println("remote OFF");
      set_rel1=true;
       client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL1 remote control");

       //alert variables
       rel1_ON_status=false;


  }
    if (rel1_remote=="ON"){
      delay(100);
<<<<<<< HEAD
=======
      mySwitch.send("000000000000000000001111");  //process it
      //client.publish("eHomeLightUnit1/rel1","ON");
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
      digitalWrite(LAMP1,LOW);
      mySwitch.send("000000000000000000001111");  //process it
      //client.publish("eHomeLightUnit1/rel1","ON");
      
      Serial.println("remote ON");
      set_rel1=true;
      client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL1 remote control");

      //alert variables

      rel1_ON_status=true;
      onTime = now;

  }


}




///////REL2   //rand2
///////////////////////////////////////////////////////////////////////////////////

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
<<<<<<< HEAD
  SW1b_2_on();
  SW1b_2_off();
=======
  SW7b_2_on();
  SW7b_2_off();
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
  SW7b_2_on();
  SW7b_2_off();
  SW8b_2_on();
  SW8b_2_off();
  SW9b_2_on();
  SW9b_2_off();
  SW6b_2_on();
  SW6b_2_off();



if (set_rel2==false){   //if a new remote message arrived
  //Serial.println("processing incomming message");
  if (rel2_remote=="OFF"){
<<<<<<< HEAD
=======
      mySwitch.send("000000000000000000000110");  //process it
      //client.publish("eHomeLightUnit1/rel2","OFF");
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
      digitalWrite(LAMP2,HIGH);
      mySwitch.send("000000000000000000000110");  //process it
      //client.publish("eHomeLightUnit1/rel2","OFF");
      
      Serial.println("remote OFF");
      set_rel2=true;
       client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL2 remote control");

       //alert variables
       rel2_ON_status=false;


  }
    if (rel2_remote=="ON"){
<<<<<<< HEAD
=======
      mySwitch.send("000000000000000000000111");  //process it
      //client.publish("eHomeLightUnit1/rel2","ON");
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
      digitalWrite(LAMP2,LOW);
      mySwitch.send("000000000000000000000111");  //process it
      //client.publish("eHomeLightUnit1/rel2","ON");
      
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
<<<<<<< HEAD

  }


     //alert variables



  ////////////////////////////////////////////////////////
  SW1b_3_on();
  SW1b_3_off();
  SW7b_3_on();
  SW7b_3_off();
  SW8b_3_on();
  SW8b_3_off();
  SW9b_3_on();
  SW9b_3_off();
  SW6b_3_on();
  SW6b_3_off();

if (set_rel3==false){   //if a new remote message arrived
  //Serial.println("processing incomming message");
  if (rel3_remote=="OFF"){
      digitalWrite(LAMP3,HIGH);
      mySwitch.send("000000000000000000001010");  //process it
      //client.publish("eHomeLightUnit1/rel3","OFF");
      
      Serial.println("remote OFF");
      set_rel3=true;
       client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL3 remote control");

       //alert variables
       rel3_ON_status=false;


  }
    if (rel3_remote=="ON"){
      digitalWrite(LAMP3,LOW);
      mySwitch.send("000000000000000000001011");  //process it
      //client.publish("eHomeLightUnit1/rel3","ON");
      
      Serial.println("remote ON");
      set_rel3=true;
      client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL3 remote control");

      //alert variables

      rel3_ON_status=true;
      onTime = now;
=======
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca

  }


<<<<<<< HEAD
}







////////////////////////////////////////////////////////////////////////////

}  //end client connected else

}//end wifi connected

   if (! WiFi.isConnected()) {
    timerWrite(timer, 0); //reset timer (feed watchdog)

  SW7b_1_state=digitalRead(SW7b_1);
  SW7b_2_state=digitalRead(SW7b_2);
  SW7b_3_state=digitalRead(SW7b_3);

  SW8b_1_state=digitalRead(SW8b_1);
  SW8b_2_state=digitalRead(SW8b_2);
  SW8b_3_state=digitalRead(SW8b_3);

  SW9b_1_state=digitalRead(SW9b_1);
  SW9b_2_state=digitalRead(SW9b_2);
  SW9b_3_state=digitalRead(SW9b_3);

  SW1b_1_state=digitalRead(SW1b_1);
  SW1b_2_state=digitalRead(SW1b_2);
  SW1b_3_state=digitalRead(SW1b_3);

  SW6b_1_state=digitalRead(SW6b_1);
  SW6b_2_state=digitalRead(SW6b_2);
  SW6b_3_state=digitalRead(SW6b_3);
  

SW1b_1_on();
SW1b_1_off();
SW7b_1_on();
SW7b_1_off();
SW8b_1_on();
SW8b_1_off();
SW9b_1_on();
SW9b_1_off();
SW6b_1_on();
SW6b_1_off();

  SW1b_2_on();
  SW1b_2_off();
  SW7b_2_on();
  SW7b_2_off();
  SW8b_2_on();
  SW8b_2_off();
  SW9b_2_on();
  SW9b_2_off();
  SW6b_2_on();
  SW6b_2_off();


  SW1b_3_on();
  SW1b_3_off();
=======
     //alert variables



  ////////////////////////////////////////////////////////
  SW7b_3_on();
  SW7b_3_off();
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
  SW7b_3_on();
  SW7b_3_off();
  SW8b_3_on();
  SW8b_3_off();
  SW9b_3_on();
  SW9b_3_off();
  SW6b_3_on();
  SW6b_3_off();

<<<<<<< HEAD



  }


 timerWrite(timer, 0); //reset timer (feed watchdog)


}


void SW7b_1_on(){
  
    
    if ( (SW7b_1_state==HIGH && SW7b_1_state!=SW7b_1PreviousState)  ){
     if (on_executed==false){
       Serial.println("REL1 HIGH from SW7b_1");
       digitalWrite(LAMP1,LOW);
       mySwitch.send("000000000000000000001111");
       Serial.println("Publishing");
       client.publish("eHomeLightUnit1/rel1","ON");
       SW7b_1PreviousState=HIGH;
       //alert variables
       rel1_ON_status=true;
       onTime = now;
       
       on_executed=true;
       off_executed=false;
  }
 
 }
 
}


void SW7b_1_off(){
  
    
  if ( (SW7b_1_state==LOW && SW7b_1_state!=SW7b_1PreviousState) ){   //
    if (off_executed==false){
      Serial.println("REL1 Low from SW7b_1");
      digitalWrite(LAMP1,HIGH);
      mySwitch.send("000000000000000000001110");
      Serial.println("Publishing");
      client.publish("eHomeLightUnit1/rel1","OFF");
      SW7b_1PreviousState=LOW;
      //alert variables
      rel1_ON_status=false;
      
      off_executed=true;
      on_executed=false;

      delay(500);
   }
    
    
  }

}

void SW7b_2_on(){
  if ( (SW7b_2_state==HIGH && SW7b_2_state!=SW7b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW7b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000000111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW7b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;



    }
}


void SW7b_2_off(){

    if ( (SW7b_2_state==LOW && SW7b_2_state!=SW7b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW7b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW7b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;



    }

}



void SW7b_3_on(){
  if ( (SW7b_3_state==HIGH && SW7b_3_state!=SW7b_3PreviousState)  ){ //
     Serial.println("REL3 HIGH from SW7b_3");
     digitalWrite(LAMP3,LOW);
     mySwitch.send("000000000000000000001011");
     
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel3","ON");



  SW7b_3PreviousState=HIGH;

  //alert variables
  rel3_ON_status=true;
  onTime = now;


}
}

void SW7b_3_off(){
  if ( (SW7b_3_state==LOW && SW7b_3_state!=SW7b_3PreviousState) ){   //
      Serial.println("REL3 Low from SW7b_3");
      digitalWrite(LAMP3,HIGH);
      mySwitch.send("000000000000000000001010");
    
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel3","OFF");



     SW7b_3PreviousState=LOW;
     //alert variables
     rel3_ON_status=false;


   }
}



void SW8b_1_on(){
  
  if ( (SW8b_1_state==HIGH && SW8b_1_state!=SW8b_1PreviousState)  ){ 
   if (on_executed==false){
       Serial.println("REL1 HIGH from SW8b_1");
       digitalWrite(LAMP1,LOW);
       mySwitch.send("000000000000000000001111");
       Serial.println("Publishing");
       client.publish("eHomeLightUnit1/rel1","ON");
       SW8b_1PreviousState=HIGH;
       //alert variables
       rel1_ON_status=true;
       onTime = now;
       on_executed=true;
       off_executed=false;
    }
  }
}
=======
if (set_rel3==false){   //if a new remote message arrived
  //Serial.println("processing incomming message");
  if (rel3_remote=="OFF"){
      mySwitch.send("000000000000000000001010");  //process it
      //client.publish("eHomeLightUnit1/rel3","OFF");
      digitalWrite(LAMP3,HIGH);
      Serial.println("remote OFF");
      set_rel3=true;
       client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL3 remote control");

       //alert variables
       rel3_ON_status=false;


  }
    if (rel3_remote=="ON"){
      mySwitch.send("000000000000000000001011");  //process it
      //client.publish("eHomeLightUnit1/rel3","ON");
      digitalWrite(LAMP3,LOW);
      Serial.println("remote ON");
      set_rel3=true;
      client.publish("eHomeLightUnits/log","eHomeLightUnit1 REL3 remote control");

      //alert variables
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca


void SW8b_1_off(){
  if ( (SW8b_1_state==LOW && SW8b_1_state!=SW8b_1PreviousState) ){   //
      Serial.println("REL1 Low from SW8b_1");
    digitalWrite(LAMP1,HIGH);
    mySwitch.send("000000000000000000001110");
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel1","OFF");


     SW8b_1PreviousState=LOW;
     //alert variables
     rel1_ON_status=false;



   }

}

void SW8b_2_on(){
  if ( (SW8b_2_state==HIGH && SW8b_2_state!=SW8b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW8b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000000111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW8b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;
<<<<<<< HEAD
=======

  }

>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca



    }
}


void SW8b_2_off(){

    if ( (SW8b_2_state==LOW && SW8b_2_state!=SW8b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW8b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW8b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;

<<<<<<< HEAD
=======
////////////////////////////////////////////////////////////////////////////
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca


    }

}



void SW8b_3_on(){
  if ( (SW8b_3_state==HIGH && SW8b_3_state!=SW8b_3PreviousState)  ){ //
     Serial.println("REL3 HIGH from SW8b_3");
     digitalWrite(LAMP3,LOW);
     mySwitch.send("000000000000000000001011");
     
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel3","ON");



  SW8b_3PreviousState=HIGH;

  //alert variables
  rel3_ON_status=true;
  onTime = now;

<<<<<<< HEAD

}
}

void SW8b_3_off(){
  if ( (SW8b_3_state==LOW && SW8b_3_state!=SW8b_3PreviousState) ){   //
      Serial.println("REL3 Low from SW8b_3");
       digitalWrite(LAMP3,HIGH);
       mySwitch.send("000000000000000000001010");
    
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel3","OFF");



     SW8b_3PreviousState=LOW;
     //alert variables
     rel3_ON_status=false;


   }
}


void SW9b_1_on(){
if (on_executed==false){
  if ( (SW9b_1_state==HIGH && SW9b_1_state!=SW9b_1PreviousState)  ){ //
     Serial.println("REL1 HIGH from SW9b_1");
   digitalWrite(LAMP1,LOW);
   mySwitch.send("000000000000000000001111");
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel1","ON");



  SW9b_1PreviousState=HIGH;

  //alert variables
  rel1_ON_status=true;
  onTime = now;
  on_executed=true;

}

}

}


void SW9b_1_off(){
  if ( (SW9b_1_state==LOW && SW9b_1_state!=SW9b_1PreviousState) ){   //
      Serial.println("REL1 Low from SW9b_1");
    digitalWrite(LAMP1,HIGH);
    mySwitch.send("000000000000000000001110");
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel1","OFF");


     SW9b_1PreviousState=LOW;
     //alert variables
     rel1_ON_status=false;



   }

}

void SW9b_2_on(){
  if ( (SW9b_2_state==HIGH && SW9b_2_state!=SW9b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW9b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000000111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW9b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;



    }
}


void SW9b_2_off(){

    if ( (SW9b_2_state==LOW && SW9b_2_state!=SW9b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW9b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW9b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;



    }

}



void SW9b_3_on(){
  if ( (SW9b_3_state==HIGH && SW9b_3_state!=SW9b_3PreviousState)  ){ //
     Serial.println("REL3 HIGH from SW9b_3");
     digitalWrite(LAMP3,LOW);
     mySwitch.send("000000000000000000001011");
     
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel3","ON");



  SW9b_3PreviousState=HIGH;

  //alert variables
  rel3_ON_status=true;
  onTime = now;


}
}

void SW9b_3_off(){
  if ( (SW9b_3_state==LOW && SW9b_3_state!=SW9b_3PreviousState) ){   //
      Serial.println("REL3 Low from SW9b_3");
      mySwitch.send("000000000000000000001010");
    digitalWrite(LAMP3,HIGH);
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel3","OFF");



     SW9b_3PreviousState=LOW;
     //alert variables
     rel3_ON_status=false;


   }
}


void SW1b_1_on(){
if (on_executed==false){
  if ( (SW1b_1_state==HIGH && SW1b_1_state!=SW1b_1PreviousState)  ){ //
     Serial.println("REL1 HIGH from SW1b_1");
   digitalWrite(LAMP1,LOW);
   mySwitch.send("000000000000000000001111");
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel1","ON");



  SW1b_1PreviousState=HIGH;

  //alert variables
  rel1_ON_status=true;
  onTime = now;
on_executed=true;

}

}

}


void SW1b_1_off(){
  if ( (SW1b_1_state==LOW && SW1b_1_state!=SW1b_1PreviousState) ){   //
      Serial.println("REL1 Low from SW1b_1");
    digitalWrite(LAMP1,HIGH);
    mySwitch.send("000000000000000000001110");
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel1","OFF");


     SW1b_1PreviousState=LOW;
     //alert variables
     rel1_ON_status=false;



   }

}

void SW1b_2_on(){
  if ( (SW1b_2_state==HIGH && SW1b_2_state!=SW1b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW1b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000000111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW1b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;



    }
}


void SW1b_2_off(){

    if ( (SW1b_2_state==LOW && SW1b_2_state!=SW1b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW1b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW1b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;



    }

}



void SW1b_3_on(){
  if ( (SW1b_3_state==HIGH && SW1b_3_state!=SW1b_3PreviousState)  ){ //
     Serial.println("REL3 HIGH from SW1b_3");
     digitalWrite(LAMP3,LOW);
     mySwitch.send("000000000000000000001011");
     
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel3","ON");



  SW1b_3PreviousState=HIGH;

  //alert variables
  rel3_ON_status=true;
  onTime = now;


}
}

void SW1b_3_off(){
  if ( (SW1b_3_state==LOW && SW1b_3_state!=SW1b_3PreviousState) ){   //
      Serial.println("REL3 Low from SW1b_3");
       digitalWrite(LAMP3,HIGH);
       mySwitch.send("000000000000000000001010");
    
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel3","OFF");



     SW1b_3PreviousState=LOW;
     //alert variables
     rel3_ON_status=false;


   }
}





void SW6b_1_on(){

  if (on_executed==false){

  if ( (SW6b_1_state==HIGH && SW6b_1_state!=SW6b_1PreviousState)  ){ //
     Serial.println("REL1 HIGH from SW6b_1");
   digitalWrite(LAMP1,LOW);
   mySwitch.send("000000000000000000001111");
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel1","ON");



  SW6b_1PreviousState=HIGH;

  //alert variables
  rel1_ON_status=true;
  onTime = now;

on_executed=true;

}
=======
    // TODO



>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
  }
}


void SW6b_1_off(){
  if ( (SW6b_1_state==LOW && SW6b_1_state!=SW6b_1PreviousState) ){   //
      Serial.println("REL1 Low from SW6b_1");
    digitalWrite(LAMP1,HIGH);
    mySwitch.send("000000000000000000001110");
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel1","OFF");


     SW6b_1PreviousState=LOW;
     //alert variables
     rel1_ON_status=false;



   }

}

void SW6b_2_on(){
  if ( (SW6b_2_state==HIGH && SW6b_2_state!=SW6b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW6b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000000111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW6b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;



    }
}


void SW6b_2_off(){

    if ( (SW6b_2_state==LOW && SW6b_2_state!=SW6b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW6b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW6b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;



    }

}


<<<<<<< HEAD
=======
void SW7b_1_on(){

  if ( (SW7b_1_state==HIGH && SW7b_1_state!=SW7b_1PreviousState)  ){ //
     Serial.println("REL1 HIGH from SW7b_1");
   digitalWrite(LAMP1,LOW);
   mySwitch.send("000000000000000000001111");
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel1","ON");



  SW7b_1PreviousState=HIGH;

  //alert variables
  rel1_ON_status=true;
  onTime = now;


}


}


void SW7b_1_off(){
  if ( (SW7b_1_state==LOW && SW7b_1_state!=SW7b_1PreviousState) ){   //
      Serial.println("REL1 Low from SW7b_1");
    digitalWrite(LAMP1,HIGH);
    mySwitch.send("000000000000000000001110");
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel1","OFF");


     SW7b_1PreviousState=LOW;
     //alert variables
     rel1_ON_status=false;



   }

}

void SW7b_2_on(){
  if ( (SW7b_2_state==HIGH && SW7b_2_state!=SW7b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW7b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000000111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW7b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;



    }
}


void SW7b_2_off(){

    if ( (SW7b_2_state==LOW && SW7b_2_state!=SW7b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW7b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW7b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;



    }

}



void SW7b_3_on(){
  if ( (SW7b_3_state==HIGH && SW7b_3_state!=SW7b_3PreviousState)  ){ //
     Serial.println("REL3 HIGH from SW7b_3");
     mySwitch.send("000000000000000000001011");
     digitalWrite(LAMP3,LOW);
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel3","ON");



  SW7b_3PreviousState=HIGH;

  //alert variables
  rel3_ON_status=true;
  onTime = now;


}
}

void SW7b_3_off(){
  if ( (SW7b_3_state==LOW && SW7b_3_state!=SW7b_3PreviousState) ){   //
      Serial.println("REL3 Low from SW7b_3");
      mySwitch.send("000000000000000000001010");
    digitalWrite(LAMP3,HIGH);
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel3","OFF");



     SW7b_3PreviousState=LOW;
     //alert variables
     rel3_ON_status=false;


   }
}



void SW8b_1_on(){

  if ( (SW8b_1_state==HIGH && SW8b_1_state!=SW8b_1PreviousState)  ){ //
     Serial.println("REL1 HIGH from SW8b_1");
   digitalWrite(LAMP1,LOW);
   mySwitch.send("000000000000000000001111");
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel1","ON");



  SW8b_1PreviousState=HIGH;

  //alert variables
  rel1_ON_status=true;
  onTime = now;


}


}


void SW8b_1_off(){
  if ( (SW8b_1_state==LOW && SW8b_1_state!=SW8b_1PreviousState) ){   //
      Serial.println("REL1 Low from SW8b_1");
    digitalWrite(LAMP1,HIGH);
    mySwitch.send("000000000000000000001110");
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel1","OFF");


     SW8b_1PreviousState=LOW;
     //alert variables
     rel1_ON_status=false;



   }

}

void SW8b_2_on(){
  if ( (SW8b_2_state==HIGH && SW8b_2_state!=SW8b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW8b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000000111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW8b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;



    }
}


void SW8b_2_off(){

    if ( (SW8b_2_state==LOW && SW8b_2_state!=SW8b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW8b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW8b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;



    }

}



void SW8b_3_on(){
  if ( (SW8b_3_state==HIGH && SW8b_3_state!=SW8b_3PreviousState)  ){ //
     Serial.println("REL3 HIGH from SW8b_3");
     mySwitch.send("000000000000000000001011");
     digitalWrite(LAMP3,LOW);
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel3","ON");



  SW8b_3PreviousState=HIGH;

  //alert variables
  rel3_ON_status=true;
  onTime = now;


}
}

void SW8b_3_off(){
  if ( (SW8b_3_state==LOW && SW8b_3_state!=SW8b_3PreviousState) ){   //
      Serial.println("REL3 Low from SW8b_3");
       mySwitch.send("000000000000000000001010");
    digitalWrite(LAMP3,HIGH);
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel3","OFF");



     SW8b_3PreviousState=LOW;
     //alert variables
     rel3_ON_status=false;


   }
}


void SW9b_1_on(){

  if ( (SW9b_1_state==HIGH && SW9b_1_state!=SW9b_1PreviousState)  ){ //
     Serial.println("REL1 HIGH from SW9b_1");
   digitalWrite(LAMP1,LOW);
   mySwitch.send("000000000000000000001111");
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel1","ON");



  SW9b_1PreviousState=HIGH;

  //alert variables
  rel1_ON_status=true;
  onTime = now;


}


}


void SW9b_1_off(){
  if ( (SW9b_1_state==LOW && SW9b_1_state!=SW9b_1PreviousState) ){   //
      Serial.println("REL1 Low from SW9b_1");
    digitalWrite(LAMP1,HIGH);
    mySwitch.send("000000000000000000001110");
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel1","OFF");


     SW9b_1PreviousState=LOW;
     //alert variables
     rel1_ON_status=false;



   }

}

void SW9b_2_on(){
  if ( (SW9b_2_state==HIGH && SW9b_2_state!=SW9b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW9b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000000111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW9b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;



    }
}


void SW9b_2_off(){

    if ( (SW9b_2_state==LOW && SW9b_2_state!=SW9b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW9b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW9b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;



    }

}



void SW9b_3_on(){
  if ( (SW9b_3_state==HIGH && SW9b_3_state!=SW9b_3PreviousState)  ){ //
     Serial.println("REL3 HIGH from SW9b_3");
     mySwitch.send("000000000000000000001011");
     digitalWrite(LAMP3,LOW);
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel3","ON");



  SW9b_3PreviousState=HIGH;

  //alert variables
  rel3_ON_status=true;
  onTime = now;


}
}

void SW9b_3_off(){
  if ( (SW9b_3_state==LOW && SW9b_3_state!=SW9b_3PreviousState) ){   //
      Serial.println("REL3 Low from SW9b_3");
      mySwitch.send("000000000000000000001010");
    digitalWrite(LAMP3,HIGH);
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel3","OFF");



     SW9b_3PreviousState=LOW;
     //alert variables
     rel3_ON_status=false;


   }
}


void SW1b_1_on(){

  if ( (SW1b_1_state==HIGH && SW1b_1_state!=SW1b_1PreviousState)  ){ //
     Serial.println("REL1 HIGH from SW1b_1");
   digitalWrite(LAMP1,LOW);
   mySwitch.send("000000000000000000001111");
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel1","ON");



  SW1b_1PreviousState=HIGH;

  //alert variables
  rel1_ON_status=true;
  onTime = now;


}


}


void SW1b_1_off(){
  if ( (SW1b_1_state==LOW && SW1b_1_state!=SW1b_1PreviousState) ){   //
      Serial.println("REL1 Low from SW1b_1");
    digitalWrite(LAMP1,HIGH);
    mySwitch.send("000000000000000000001110");
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel1","OFF");


     SW1b_1PreviousState=LOW;
     //alert variables
     rel1_ON_status=false;



   }

}

void SW1b_2_on(){
  if ( (SW1b_2_state==HIGH && SW1b_2_state!=SW1b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW1b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000001111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW1b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;



    }
}


void SW1b_2_off(){

    if ( (SW1b_2_state==LOW && SW1b_2_state!=SW1b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW1b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW1b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;



    }

}



void SW1b_3_on(){
  if ( (SW1b_3_state==HIGH && SW1b_3_state!=SW1b_3PreviousState)  ){ //
     Serial.println("REL3 HIGH from SW1b_3");
     mySwitch.send("000000000000000000000111");
     digitalWrite(LAMP3,LOW);
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel3","ON");



  SW1b_3PreviousState=HIGH;

  //alert variables
  rel3_ON_status=true;
  onTime = now;


}
}

void SW1b_3_off(){
  if ( (SW1b_3_state==LOW && SW1b_3_state!=SW1b_3PreviousState) ){   //
      Serial.println("REL3 Low from SW1b_3");
       mySwitch.send("000000000000000000001010");
    digitalWrite(LAMP3,HIGH);
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel3","OFF");



     SW1b_3PreviousState=LOW;
     //alert variables
     rel3_ON_status=false;


   }
}





void SW6b_1_on(){

  if ( (SW6b_1_state==HIGH && SW6b_1_state!=SW6b_1PreviousState)  ){ //
     Serial.println("REL1 HIGH from SW6b_1");
   digitalWrite(LAMP1,LOW);
   mySwitch.send("000000000000000000001111");
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel1","ON");



  SW6b_1PreviousState=HIGH;

  //alert variables
  rel1_ON_status=true;
  onTime = now;


}


}


void SW6b_1_off(){
  if ( (SW6b_1_state==LOW && SW6b_1_state!=SW6b_1PreviousState) ){   //
      Serial.println("REL1 Low from SW6b_1");
    digitalWrite(LAMP1,HIGH);
    mySwitch.send("000000000000000000001110");
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel1","OFF");


     SW6b_1PreviousState=LOW;
     //alert variables
     rel1_ON_status=false;



   }

}

void SW6b_2_on(){
  if ( (SW6b_2_state==HIGH && SW6b_2_state!=SW6b_2PreviousState)  ){ //
         Serial.println("REL2 HIGH from SW6b_2");
        digitalWrite(LAMP2,LOW);
        mySwitch.send("000000000000000000000111");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","ON");


      SW6b_2PreviousState=HIGH;


      //alert variables
      rel2_ON_status=true;
      onTime = now;



    }
}


void SW6b_2_off(){

    if ( (SW6b_2_state==LOW && SW6b_2_state!=SW6b_2PreviousState) ){   //
       Serial.println("REL2 Low from SW6b_2");
     digitalWrite(LAMP2,HIGH);
     mySwitch.send("000000000000000000000110");
            Serial.println("Publishing");
            client.publish("eHomeLightUnit1/rel2","OFF");


      SW6b_2PreviousState=LOW;

      //alert variables
      rel2_ON_status=false;



    }

}


>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca

void SW6b_3_on(){
  if ( (SW6b_3_state==HIGH && SW6b_3_state!=SW6b_3PreviousState)  ){ //
     Serial.println("REL3 HIGH from SW6b_3");
<<<<<<< HEAD
     digitalWrite(LAMP3,LOW);
     mySwitch.send("000000000000000000001011");
     
=======
     mySwitch.send("000000000000000000001011");
     digitalWrite(LAMP3,LOW);
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
        Serial.println("Publishing");
        client.publish("eHomeLightUnit1/rel3","ON");



  SW6b_3PreviousState=HIGH;

  //alert variables
  rel3_ON_status=true;
  onTime = now;


}
}

void SW6b_3_off(){
  if ( (SW6b_3_state==LOW && SW6b_3_state!=SW6b_3PreviousState) ){   //
      Serial.println("REL3 Low from SW6b_3");
<<<<<<< HEAD
      digitalWrite(LAMP3,HIGH);
      mySwitch.send("000000000000000000001010");
    
=======
      mySwitch.send("000000000000000000001010");
    digitalWrite(LAMP3,HIGH);
>>>>>>> 035654b824981fd7e0a30e68f9cc7f1b4fab87ca
           Serial.println("Publishing");
           client.publish("eHomeLightUnit1/rel3","OFF");



     SW6b_3PreviousState=LOW;
     //alert variables
     rel3_ON_status=false;


   }
}
