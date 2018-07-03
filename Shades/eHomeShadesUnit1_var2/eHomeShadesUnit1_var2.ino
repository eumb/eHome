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
const char* loggerHost     = "192.168.2.10";
const char* url      = "/api";
char* serverMqtt = "192.168.2.10";

const char* deviceId = "eHomeShadesUnit1";


// OTA HTTP Config
String otaHost = "192.168.2.10"; // Host => bucket-name.s3.region.amazonaws.com
int otaPort = 80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String bin = "eHomeShadesUnit1.ino.esp32.bin"; // bin file name with a slash in front.
int contentLength = 0;
bool isValidContentType = false;

const char* eHomeFwVer = "1.0";
const char* deviceType = "Shades";
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


//Living1
//Living2
//Terasa1
//Terasa2
//Bucatarie


//A zone

//Control buttons
#define swUP 33
#define swDOWN 32

//SW definition
#define swLiv1 35
#define swLiv2 35
#define swTer1 35
#define swTer2 35
#define swBuc 35

//Relay definition
#define Liv1Up 16
#define Liv1Down  17
#define Liv2Up 18
#define Liv2Down  19
#define Ter1Up 16
#define Ter1Down  17
#define Ter2Up 18
#define Ter2Down  19
#define KitUp 18
#define KitDown  19

//B zone
//controll buttons
#define swOfficeUp 35
#define swOfficeDown 35

//Relays
#define OfficeUp 18
#define OfficeDown  19


//switches remote control
#define REMOTE 25

WiFiUDP ntpUDP;

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);


//bool sw1,sw2,sw3,remote_set;

int swLiv1_state, swLiv2_state, swTer1_state, swTer2_state, swKit_state;
int swLiv1PreviousState = LOW, swLiv2PreviousState = LOW, swTer1PreviousState = LOW, swTer2PreviousState = LOW, swKitPreviousState = LOW;

char sw_Liv1[25], sw_Liv2[25], sw_Ter1[25], sw_Ter2[25], sw_Kit[25];
String swLiv1_remote, swLiv2_remote, swTer1_remote, swTer2_remote, swKit_remote;

bool set_swLiv1 , set_swLiv2, set_swTer1, set_swTer2, set_swKit  ;

long lastReconnectAttempt = 0;
long lastConnection = 0;

//watcdog control variable
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
  ets_printf("watchdog interrupt triggered reboot\n");

  esp_restart();
}

String reset_reason;

void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : reset_reason = "Vbat power on reset"; break;       /**<1,  Vbat power on reset*/
    case 3 : reset_reason = "Software reset digital core"; break;            /**<3,  Software reset digital core*/
    case 4 : reset_reason = "Legacy watch dog reset digital core"; break;          /**<4,  Legacy watch dog reset digital core*/
    case 5 : reset_reason = "Deep Sleep reset digital core"; break;     /**<5,  Deep Sleep reset digital core*/
    case 6 : reset_reason = "Reset by SLC module, reset digital core"; break;          /**<6,  Reset by SLC module, reset digital core*/
    case 7 : reset_reason = "Timer Group0 Watch dog reset digital core"; break;    /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : reset_reason = "Timer Group1 Watch dog reset digital core"; break;    /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : reset_reason = "RTC Watch dog Reset digital core"; break;    /**<9,  RTC Watch dog Reset digital core*/
    case 10 : reset_reason = "Instrusion tested to reset CPU"; break;    /**<10, Instrusion tested to reset CPU*/
    case 11 : reset_reason = "Time Group reset CPU"; break;    /**<11, Time Group reset CPU*/
    case 12 : reset_reason = "Software reset CPU"; break;       /**<12, Software reset CPU*/
    case 13 : reset_reason = "RTC Watch dog Reset CPU"; break;   /**<13, RTC Watch dog Reset CPU*/
    case 14 : reset_reason = "for APP CPU, reseted by PRO CPU"; break;      /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : reset_reason = "Reset when the vdd voltage is not stable"; break; /**<15, Reset when the vdd voltage is not stable*/
    case 16 : reset_reason = "RTC Watch dog reset digital core and rtc module"; break;   /**<16, RTC Watch dog reset digital core and rtc module*/
    default : reset_reason == "NO_MEAN";
  }
}






////////////////////////  Log ////////////////////////////////////////////


void sendLog(String message) {

  HTTPClient http;

  timeClient.update();
  String actual_time = timeClient.getFormattedTime();

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();


  root["time"] = actual_time;
  root["deviceId"] = deviceId;
  root["message"] = message;



  char JSONmessageBuffer[300];

  root.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));


  http.begin("http://192.168.2.10:2000/api/log"); //Specify destination for HTTP request
  http.addHeader("Content-Type", "application/json"); //Specify content-type header
  //int httpResponseCode = http.POST("POSTING from ESP32"); //Send the actual POST request
  int httpResponseCode = http.POST(JSONmessageBuffer); //Send the actual POST request

  if (httpResponseCode > 0) {

    String response = http.getString();  //Get the response to the request



    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer

  } else {

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
    client.publish("log", "reconnected; Hello");
    // ... and resubscribe
    client.subscribe("eHomeShadesUnit1/swLiv1Remote");
    client.subscribe("eHomeShadesUnit1/swLiv2Remote");
    client.subscribe("eHomeShadesUnit1/swTer1Remote");
    client.subscribe("eHomeShadesUnit1/swTer2Remote");
    client.subscribe("eHomeShadesUnit1/swKitRemote");

    client.publish("eHomeShadesUnits/log", "eHomeShadesUnit1 client connected");
    set_swLiv1 = true; set_swLiv2 = true; set_swTer1 = true; set_swTer2 = true; set_swKit = true;
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

  if (String( (char *)topic) == "eHomeShadesUnit1/swLiv1") {
    int i;
    for (  i = 0; i < length; i++)
    {
      sw_Liv1[i] = payload[i];
    }
    sw_Liv1[i] = '\0';

    set_swLiv1 = false;

    const char *p_payload = sw_Liv1;
    swLiv1_remote = String( (char *)p_payload);


    Serial.println(sw_Liv1);
  }

  if (String( (char *)topic) == "eHomeShadesUnit1/swLiv2") {
    int i;
    for (  i = 0; i < length; i++)
    {
      sw_Liv2[i] = payload[i];
    }
    sw_Liv2[i] = '\0';

    set_swLiv2 = false;

    const char *p_payload = sw_Liv2;
    swLiv2_remote = String( (char *)p_payload);


    Serial.println(sw_Liv2);
  }




  if (String( (char *)topic) == "eHomeShadesUnit1/swTer1") {
    int i;
    for (  i = 0; i < length; i++)
    {
      sw_Ter1[i] = payload[i];
    }
    sw_Ter1[i] = '\0';

    set_swTer1 = false;

    const char *p_payload = sw_Ter1;
    swTer1_remote = String( (char *)p_payload);


    Serial.println(sw_Ter1);
  }


  if (String( (char *)topic) == "eHomeShadesUnit1/swTer2") {
    int i;
    for (  i = 0; i < length; i++)
    {
      sw_Ter2[i] = payload[i];
    }
    sw_Ter2[i] = '\0';

    set_swTer2 = false;

    const char *p_payload = sw_Ter2;
    swTer2_remote = String( (char *)p_payload);


    Serial.println(sw_Ter1);
  }



  if (String( (char *)topic) == "eHomeShadesUnit1/swKit") {
    int i;
    for (  i = 0; i < length; i++)
    {
      sw_Kit[i] = payload[i];
    }
    sw_Kit[i] = '\0';

    set_swTer1 = false;

    const char *p_payload = sw_Kit;
    swKit_remote = String( (char *)p_payload);


    Serial.println(sw_Kit);
  }




}

/////////////////////////////////////////////////////////




void SwitchTreatment() {


}






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




void setup(void) {
  // put your setup code here, to run once:
  Serial.begin(115200);



  pinMode(swLiv1, INPUT_PULLDOWN);
  pinMode(swLiv2, INPUT_PULLDOWN);
  pinMode(swTer1, INPUT_PULLDOWN);
  pinMode(swTer2, INPUT_PULLDOWN);
  pinMode(swKit, INPUT_PULLDOWN);

  pinMode(REMOTE, OUTPUT);


  pinMode(Liv1Up, OUTPUT);
  pinMode(Liv1Down, OUTPUT);
  pinMode(Liv2Up, OUTPUT);
  pinMode(Liv2Down, OUTPUT);
  pinMode(Ter1Up, OUTPUT);
  pinMode(Ter1Down, OUTPUT);
  pinMode(Ter2Up, OUTPUT);
  pinMode(Ter2Down, OUTPUT);
  pinMode(KitUp, OUTPUT);
  pinMode(KitDown, OUTPUT);

  digitalWrite(Liv1Up, LOW);
  digitalWrite(Liv1Down, LOW);
  digitalWrite(Liv2Up, LOW);
  digitalWrite(Liv2Down, LOW);
  digitalWrite(Ter1Up, LOW);
  digitalWrite(Ter1Down, LOW);
  digitalWrite(Ter2Up, LOW);
  digitalWrite(Ter2Down, LOW);
  digitalWrite(KitUp, LOW);
  digitalWrite(KitDown, LOW);


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
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */


  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });


  server.on("/httpUpdate", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", "ok");
    //delay(2000);
    updateFW();
  });


  server.on("/restart", HTTP_POST, []() {
    server.send(200, "text/plain", "device restart");
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

  server.on("/status", HTTP_GET, []() {

    long currentTime = millis();

    String response;
    timeClient.update();
    String actual_time = timeClient.getFormattedTime();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");

    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();

    uint64_t  chipid = ESP.getEfuseMac();

    print_reset_reason(rtc_get_reset_reason(0));

    root["heap"] = ESP.getFreeHeap();
    root["ssid"] = WiFi.SSID();
    root["wifistr"] = WiFi.RSSI();
    root["IP"] = WiFi.localIP().toString();
    root["deviceId"] = deviceId;
    root["deviceType"] = deviceType;
    root["deviceLocation"] = deviceLocation;
    root["deviceScope"] = deviceScope;
    root["uptime"] = millis();
    root["lastReason"] = reset_reason;
    root["chipId"] = (uint32_t)chipid;
    root["chipRevision"] = ESP.getChipRevision();
    root["FlashSize"] = ESP.getFlashChipSize();
    root["SDKVersion"] = ESP.getSdkVersion();
    root["eHomeFwVer"] = eHomeFwVer;
    root["time"] = actual_time;


    root.printTo(response);
    server.send(200, "text/json", response);
  });



  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    esp_wifi_wps_disable(); ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
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

void loop(void) {
  server.handleClient();


  poll_connection();

  if (WiFi.isConnected())
  {








    ////////////////////LOCAL command//////////////////////////

    swLiv1_state = digitalRead(swLiv1);
    swLiv2_state = digitalRead(swLiv2);
    swTer1_state = digitalRead(swTer1);
    swTer2_state = digitalRead(swTer2);
    swKit_state = digitalRead(swKit);


    if (swLiv1_state == HIGH) {
      Serial.println("Living shades 1 selected");
      if (swUP == HIGH) {
        Serial.println("going up");
        //must send command to turn down the swDown
        digitalWrite(Liv1Down, LOW); //safety to have only one relay on at one time
        digitalWrite(Liv1Up, HIGH);
      }
      if (swDown == HIGH) {
        Serial.println("going down");
        //must send command to turn down the swUp
        digitalWrite(Liv1Up, LOW); //safety to have only one relay on at one time
        digitalWrite(Liv1Down, HIGH);
      }
    }




    if (swLiv2_state == HIGH) {
      Serial.println("Living shades 2 selected");
      if (swUP == HIGH) {
        Serial.println("going up");
        //must send command to turn down the swDown
        digitalWrite(Liv2Down, LOW); //safety to have only one relay on at one time
        digitalWrite(Liv2Up, HIGH);
      }
      if (swDown == HIGH) {
        Serial.println("going down");
        //must send command to turn down the swUp
        digitalWrite(Liv2Up, LOW); //safety to have only one relay on at one time
        digitalWrite(Liv2Down, HIGH);
      }
    }



    if (swTer1_state == HIGH) {
      Serial.println("Terase shades 1 selected");
      if (swUP == HIGH) {
        Serial.println("going up");
      }
      if (swDown == HIGH) {
        Serial.println("going down");
      }
    }
    if (swTer2_state == HIGH) {
      Serial.println("Terase shades 2 selected");
      if (swUP == HIGH) {
        Serial.println("going up");
      }
      if (swDown == HIGH) {
        Serial.println("going down");
      }
    }
    if (swKit_state == HIGH) {
      Serial.println("Kitchen shades selected");
      if (swUP == HIGH) {
        Serial.println("going up");
      }
      if (swDown == HIGH) {
        Serial.println("going down");
      }
    }







    if (sw2_state == HIGH) {

      if ( (sw1_state == HIGH && sw1_state != sw1PreviousState)  ) { //
        Serial.println("Jaluzea 1 UP");
        digitalWrite(Rel1Up, HIGH);

        sw1PreviousState = HIGH;

      }
      if ( (sw1_state == LOW && sw1_state != sw1PreviousState)  ) { //
        Serial.println("Jaluzea 1 up stop");
        digitalWrite(Rel1Up, LOW);

        sw1PreviousState = LOW;

      }


      if ( (sw3_state == HIGH && sw3_state != sw3PreviousState) ) { //
        Serial.println("Jaluzea 1 Down");
        digitalWrite(Rel1Down, HIGH);

        sw3PreviousState = HIGH;



      }
      if ( (sw3_state == LOW && sw3_state != sw3PreviousState) ) { //
        Serial.println("Jaluzea 1 down stop");
        digitalWrite(Rel1Down, LOW);

        sw3PreviousState = LOW;



      }

      if ( sw3_state == HIGH && sw1_state == HIGH) {

        mySwitch.send("000000000000000000001010");
        mySwitch.send("000000000000000000000110");
        mySwitch.send("000000000000000000001110");
        digitalWrite(Rel1Down, LOW);
        digitalWrite(Rel1Up, LOW);


      }


    }


    if (sw2_state == LOW) {

      if ( (sw1_state == HIGH && sw1_state != sw1PreviousState)  ) { //
        Serial.println("Jaluzea 2 UP");
        digitalWrite(Rel2Up, HIGH);

        sw1PreviousState = HIGH;

      }
      if ( (sw1_state == LOW && sw1_state != sw1PreviousState)  ) { //
        Serial.println("Jaluzea 2 up stop");
        digitalWrite(Rel2Up, LOW);

        sw1PreviousState = LOW;

      }


      if ( (sw3_state == HIGH && sw3_state != sw3PreviousState) ) { //
        Serial.println("Jaluzea 2 Down");
        digitalWrite(Rel2Down, HIGH);

        sw3PreviousState = HIGH;


      }
      if ( (sw3_state == LOW && sw3_state != sw3PreviousState) ) { //
        Serial.println("Jaluzea 2 down stop");
        digitalWrite(Rel2Down, LOW);

        sw3PreviousState = LOW;



      }

      if ( sw3_state == HIGH && sw1_state == HIGH) {

        //        mySwitch.send("000000000000000000001010");
        //        mySwitch.send("000000000000000000000110");
        //        mySwitch.send("000000000000000000001110");

        mySwitch.send("000000000000000000000111");
        mySwitch.send("000000000000000000001111");
        mySwitch.send("000000000000000000001011");

        digitalWrite(Rel2Down, LOW);
        digitalWrite(Rel2Up, LOW);


      }

    }












    timerWrite(timer, 0); //reset timer (feed watchdog)


    if (!client.connected()) {



      long now = millis();

      ///work it locally without MQTT client connected








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


      //TODO for MQTT client connected



      /////////////////REMOTE command//////////////////////
      //    if (sw1_remote=="ON"){
      //
      //       digitalWrite(MotUp,HIGH);
      //       //Serial.println("Jaluzea UP");
      //    }else if (sw1_remote=="OFF"){
      //       digitalWrite(MotUp,LOW);
      //       //Serial.println("Jaluzea UP STOP");
      //    }
      //    if (sw2_remote=="ON"){
      //       digitalWrite(MotDown,HIGH);
      //       //Serial.println("Jaluzea Down");
      //    }
      //     else if (sw2_remote=="OFF"){
      //       digitalWrite(MotDown,LOW);
      //       //Serial.println("Jaluzea Down STOP");
      //    }

      ////////////////////LOCAL command//////////////////////////



    }  //end ! client connected else

  }//end wifi connected


  if (! WiFi.isConnected()) {
    timerWrite(timer, 0); //reset timer (feed watchdog)


    // TODO work it without wifi



  }


  timerWrite(timer, 0); //reset timer (feed watchdog)


}

