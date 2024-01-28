/**
 * @ Author: Maciej Żyrek, Maciej Węcki
 * @ Create Time: 01.04.2021 12:45:06
 * @ Modified by: Maciej Żyrek
 * @ Modified time: 15-12-2021 23:11:46
 * @ Description: ESP8266 webServer(sensorStation), PLC(desk)
 */

/*___Library_used_source_GitHub___*/
/*___Project_inspired_LINK___*/
/*___ https://tttapa.github.io/ESP8266/Chap01%20-%20ESP8266.html ___*/

#include <ArduinoOTA.h>       // wireless upload sketch
#include <DHT.h>              // library for DHT sensor
#include <ESP8266WiFi.h>      // esp wifi & server lib
#include <ESP8266WiFiMulti.h> // esp wifi & server lib
#include <ESP8266WebServer.h> // esp wifi & server lib
#include <ESP8266mDNS.h>      // esp wifi & server lib
#include <WebSocketsServer.h> // esp wifi & server lib
#include <WiFiUdp.h>          // esp wifi & server lib
#include <Streaming.h>        // Arduino library for supporting the << streaming operator
#include <stdio.h>            // standard i/o communication

/*____________________________________________________Global_variable_and_others____________________________________________________*/

#define ONE_HOUR 3600000UL
#define ONE_MINUTE 60000UL
                                                // pin on ESP board
#define pir              4                      // D2, PIR pin delaration
#define LED_RED         12                      // D6, specify the pins with an RGB LED connected
#define LED_GREEN       15                      // D8
#define LED_BLUE        14                      // D5
#define relayK1          0                      // D3
#define relayK2         16                      // D0
#define relayK3         13                      // D7
#define relayK4          2                      // D4
#define DHTPIN           5                      // D1, Digital pin connected to the DHT sensor

#define DHTTYPE      DHT22                      // Declaration type of DH T sensor
#define PhotoR  3.3/1024.0                      // Resolution of analog read
#define PhotoR_v       3.1                      // Value when PIR() can be enabled (sensitive of light)

typedef unsigned long ulong;                    // short version of unsigned long

const ulong intervalPirMethod = 500;            // time of checking state move sensor
const ulong intervalOverWriting = ONE_HOUR*48;  // Start overWritting data (temp.csv, hum.csv) after difference equal or bigger than 86400(minute from NTP)
const ulong intervalNTP = ONE_HOUR;             // Update the time every hour
const ulong intervalTemp = 4000;       // Do a temperature measurement every minute
const ulong DS_delay = 750;                     // Reading the temperature from the DS18x20 can take up to 750ms
const ulong NTP_interval = 4*ONE_HOUR;
const ulong intervalDrukTime = ONE_MINUTE*15;

/*_______________________Timmings______________________*/
ulong dhtTime = 0;
ulong espResetTime = 0;
ulong loopTimePir = 0, pirTime = 0;
ulong prevActualTime = 0;
ulong prevNTP = 0;
ulong lastNTPResponse = 0;
ulong prevTemp = 0;
ulong prevMillis = 0;
ulong prevDrukTime = 0;
ulong pirOffDelay = ONE_MINUTE;                          // Variable time for metgod motion sensor

/*_______________________bool_variables_________________*/
bool tmpRequested = false;                    // Status of read DHT sensor
bool rainbow = false;                         // The rainbow effect is turned off on startup
bool pirMode = false;                         // Statement of sensor motion On/Off
bool K1=false, K2=false, K3=false, K4=false;  // Logic of statment relays


const char *ssid = "ESP8266 Access Point";    // The name of the Wi-Fi network that will be created
const char *password = "thereisnospoon";      // The password required to connect to it, leave blank for an open network
const char *OTAName = "ESP8266";              // A name and a password for the OTA service
const char *OTAPassword = "esp8266";
const char* mdnsName = "DESK";                // Domain name for the mDNS responder
const char* NTPServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48;               // NTP time stamp is in the first 48 bytes of the message

int MP = 0;                                   // Variable for method motion sensor, allow to turn light by PIR sensor
int hue = 0;

float t = 0.0, tM = 0.0;                      // Temperature variable and previous correct Temp
float h = 0.0, hM = 0.0;

WiFiUDP UDP;                                  // Create an instance of the WiFiUDP class to send and receive
IPAddress timeServerIP;                       // time.nist.gov NTP server address
ESP8266WiFiMulti wifiMulti;                   // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);                  // create a web server on port 80
WebSocketsServer webSocket(81);               // create a websocket server on port 81
File fsUploadFile;                            // a File variable to temporarily store the received file

byte NTPBuffer[NTP_PACKET_SIZE];              // buffer to hold incoming and outgoing packets
byte x = 0;
uint32_t timeUNIX = 0;
DHT dht(DHTPIN, DHTTYPE);

uint8_t globalAdressCilent;                   //  TODO: Globalny adres client, do funkcji WebSerial
bool webSerialStatus = false;                 //  TODO: Jeżeli jest client to serial jest dostępny

int redGlobal,greenGlobal,blueGlobal;         //  Global memory of current light setup

/*__________________________________________________________SETUP__________________________________________________________*/

void setup() {
  /*___________________________________________________Setup_pinouts_______________________________________________________*/
  pinMode(relayK1,    OUTPUT);    // pin of relays
  pinMode(relayK2,    OUTPUT);
  pinMode(relayK3,    OUTPUT);
  pinMode(relayK4,    OUTPUT);
  pinMode(LED_RED,    OUTPUT);    // the pins with LEDs connected are outputs
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_BLUE,   OUTPUT);
  pinMode(pir,         INPUT);    // pin of motion sensor

  /*___________________________________________________Set_all_relay_off__________________________________________________*/

  setK(1);
  setK(2);
  setK(3);
  setK(4);

  /*___________________________________________________Start_communication________________________________________________*/
  Serial.begin(115200);        // Start the Serial communication to send messages to the computer

  dht.begin();                 // Start the dht22 communication

  delay(10);

  Serial << "\r\n";

  startWiFi();                 // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  startUDP();                  // Start the UDP service, allow communicate between devices TODO: Przeczytać do czego to jest wykorzystywane
  startOTA();                  // Start the OTA service
  startSPIFFS();               // Start the SPIFFS and list all contents
  startWebSocket();            // Start a WebSocket server
  startMDNS();                 // Start the mDNS responder
  startServer();               // Start a HTTP server with a file read handler and an upload handler

  if (!WiFi.hostByName(NTPServerName, timeServerIP)) { // Get the IP address of the NTP server
    Serial << "DNS lookup failed. Rebooting." << endl;
    Serial.flush();
    ESP.reset();
  }

  Serial << "Time server IP:\t" << timeServerIP << endl << "\r\nSending NTP request ..." << endl;

  sendNTPpacket(timeServerIP);

  ArduinoOTA.setHostname("ESP8266");
  //ArduinoOTA.setPassword("esp8266");

  ArduinoOTA.onStart([]() {
    Serial << "Start" << endl;
  });

  ArduinoOTA.onEnd([]() {
    Serial << "\nEnd" << endl;
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial << "Auth Failed" << endl;
    else if (error == OTA_BEGIN_ERROR) Serial << "Begin Failed" << endl;
    else if (error == OTA_CONNECT_ERROR) Serial << "Connect Failed" << endl;
    else if (error == OTA_RECEIVE_ERROR) Serial << "Receive Failed" << endl;
    else if (error == OTA_END_ERROR) Serial << "End Failed" << endl;
  });

  ArduinoOTA.begin();

  Serial << "OTA ready" << endl;
  Serial << "Zaczynam 28.10.2021" << endl;
}

/*__________________________________________________________LOOP__________________________________________________________*/

void loop() {

  ArduinoOTA.handle();                        //  run Over the Air service(load sketch by WiFi connection)
  webSocket.loop();                           //  constantly check for websocket events
  server.handleClient();                      //  run the server
  ArduinoOTA.handle();                        //  listen for OTA events

  ulong currentMillis = millis();             //  current time loop

  if (currentMillis - prevNTP > intervalNTP) { // If a minute has passed since last NTP request
    prevNTP = currentMillis;
    Serial << "\r\nSending NTP request ..." << endl;
    sendNTPpacket(timeServerIP);               // Send an NTP request
  }

  uint32_t time = getTime();                   // Check if an NTP response has arrived and get the (UNIX) time
  if (time) {                                  // If a new timestamp has been received
    timeUNIX = time;
    Serial << "NTP response:\t" << endl << timeUNIX << endl;
    lastNTPResponse = currentMillis;
  } else if ((currentMillis - lastNTPResponse) > NTP_interval) {
    Serial << "More than 1 hour since last NTP response. Rebooting." << endl;
    Serial.flush();
    ESP.reset();
  }
  uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse) / 1000;

  if (actualTime != prevActualTime && timeUNIX != 0) { // If a second has passed since last print
    prevActualTime = actualTime;
    //Serial.printf("\rUTC time:\t%d:%d:%d   ", getHours(actualTime), getMinutes(actualTime), getSeconds(actualTime));
  }

/*______________________________________________________Measurments_DHT22_________________________________________________*/ //TODO:
  if (timeUNIX != 0) {
    if (currentMillis - prevTemp > intervalTemp) {  //  Every minute, request the temperature

      t = dht.readTemperature();                  //  Get new data from sensor
      h = dht.readHumidity();

      if (isnan(t) || isnan(h)) {                 //  if sensor return data type NaN
        t = tM;                                   //  overwrite new data with last data
        h = hM;
        WebSerial("DHT NaN problem ... ");        //  send to client problem
      } else  {
        tM = t;                                   //  SAVE new data to global variable
        hM = h;
        tmpRequested = true;

        String buffer = "Temp,"+(String)t+","+(String)h; // TODO: SPRAWDZIĆ CZY DZIAŁA
        WebSerial(buffer);
      }
      prevTemp = currentMillis;
    }
    /*_______________________________________Save_measurments_to_file__________________________________________________*/
    if (tmpRequested && (currentMillis - prevDrukTime > intervalDrukTime)) { // 750 ms after requesting the temperature
      uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse) / 1000;
      // The actual time is the last NTP time plus the time that has elapsed since the last NTP response
      tmpRequested = false;

      //Serial.printf("Appending temperature to file: %lu, %lu, %lu", actualTime, t, h);
      Serial << "Add temp.csv [czas, temp*C] = [" << actualTime << ", " << t << "], ";
      File tempLog = SPIFFS.open("/temp.csv", "a"); // Write the time and the temperature to the csv file
      tempLog.print(actualTime);                    // SAVE TO FILE
      tempLog.print(",");
      tempLog.print(t);
      tempLog.print(",");
      tempLog.println(h);
      tempLog.close();                              // CLOSE FILE

      File original = SPIFFS.open("/temp.csv", "r"); // taking oldest time from file
      String name_ = original.name();
      char Z[12];
      original.read((uint8_t *)Z,12);
      original.close();
      String Domka = Z;
      WebSerial("ZAPIS");
      char *endptr;

      if((actualTime - strtol(Z, &endptr, 10)) >= intervalOverWriting){ // After 86400(24h) data overwrite last remember data
        WebSerial("NADPISANIE");
        deleteFirstLine((String)"/temp.csv");
      }
      prevDrukTime = millis();
    }
  } else {                                    // If we didn't receive an NTP response yet, send another request
    sendNTPpacket(timeServerIP);
    delay(500);
  }

/*________________________________________________light_programs_setup__________________________________________________________*/
  if(rainbow) {                               // if the rainbow effect is turned on
    if(millis() > prevMillis + 32) {
      if(++hue == 360)                        // Cycle through the color wheel (increment by one degree every 32 ms)
        hue = 0;
      setHue(hue);                            // Set the RGB LED to the right color
      prevMillis = millis();
    }
  } else if ((pirMode)&&( currentMillis-loopTimePir >= intervalPirMethod )) {                              // Sensor motion execute
    PIR();                                                                                            // Function PIR
    loopTimePir = millis();
  }
}

/*__________________________________________________________SETUP_FUNCTIONS__________________________________________________________*/

void startWiFi() { // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  WiFi.softAP(ssid, password);             // Start the access point
  Serial << "Access Point \"" << ssid << "\" started\r\n" << endl;

  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
  
  Serial << "Connecting" << endl;
  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1) {  // Wait for the Wi-Fi to connect
    delay(250);
    Serial << '.';
  }
  Serial << endl;
  if(WiFi.softAPgetStationNum() == 0) {      // If the ESP is connected to an AP
    Serial << "Connected to " << WiFi.SSID() << endl << "IP address:\t" << WiFi.localIP();
    // Tell us what network we're connected to          // Send the IP address of the ESP8266 to the computer
  } else {                                   // If a station is connected to the ESP SoftAP
    Serial << "Station connected to ESP8266 AP";
  }
  Serial << endl;
}

void startOTA() { // Start the OTA service
  ArduinoOTA.setHostname(OTAName);
  //ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]() {
    Serial << "Start" << endl;
    digitalWrite(LED_RED, 0);    // turn off the LEDs
    digitalWrite(LED_GREEN, 0);
    digitalWrite(LED_BLUE, 0);
  });
  ArduinoOTA.onEnd([]() {
    Serial << "\r\nEnd" << endl;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial << "Auth Failed" << endl;
    else if (error == OTA_BEGIN_ERROR) Serial << "Begin Failed" << endl;
    else if (error == OTA_CONNECT_ERROR) Serial << "Connect Failed" << endl;
    else if (error == OTA_RECEIVE_ERROR) Serial << "Receive Failed" << endl;
    else if (error == OTA_END_ERROR) Serial << "End Failed" << endl;
  });
  ArduinoOTA.begin();
  Serial << "OTA ready\r\n" << endl;
}

void startSPIFFS() { // Start the SPIFFS and list all contents
  SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
  Serial << "SPIFFS started. Contents:" << endl;
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial << "WebSocket server started." << endl;
}

void startMDNS() { // Start the mDNS responder
  MDNS.begin(mdnsName);                        // start the multicast domain name server
  Serial << "mDNS responder started: http://" << mdnsName << ".local" << endl;
}

void startServer() { // Start a HTTP server with a file read handler and an upload handler
  server.on("/edit.html",  HTTP_POST, []() {  // If a POST request is sent to the /edit.html address,
    server.send(200, "text/plain", "");
  }, handleFileUpload);                       // go to 'handleFileUpload'

  server.onNotFound(handleNotFound);          // if someone requests any other file or page, go to function 'handleNotFound'
                                              // and check if the file exists

  server.begin();                             // start the HTTP server
  Serial << "HTTP server started." << endl;
}

void startUDP() {
  Serial << "Starting UDP" << endl;
  UDP.begin(123);                          // Start listening for UDP messages on port 123
  Serial << "Local port:\t" << UDP.localPort() << endl << endl;
}

/*__________________________________________________________SERVER_HANDLERS__________________________________________________________*/

void handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.print("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String(" \tSent file: ") + path);
    return true;
  }
  Serial.println(String(" \tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = server.upload();
  String path;
  if(upload.status == UPLOAD_FILE_START){
    path = upload.filename;
    if(!path.startsWith("/")) path = "/"+path;
    if(!path.endsWith(".gz")) {                          // The file server always prefers a compressed version of a file
      String pathWithGz = path+".gz";                    // So if an uploaded file is not compressed, the existing compressed
      if(SPIFFS.exists(pathWithGz))                      // version of that file must be deleted (if it exists)
        SPIFFS.remove(pathWithGz);
    }
    Serial << "handleFileUpload Name: " << path << endl;
    fsUploadFile = SPIFFS.open(path, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial << "handleFileUpload Size: " << upload.totalSize << endl;
      server.sendHeader((String)"Location",(String)"/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received

  globalAdressCilent = num;                                                                     //  Global Adress client to let function webSerial work properly

  switch (type) {
    case WStype_DISCONNECTED:                                                                   // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      webSerialStatus = false;
      break;
    case WStype_CONNECTED: {                                                                    // if a new websocket connection is established
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      rainbow = false;                                                                          // Turn rainbow off when a new connection is established
      webSerialStatus = true;                                                                   // start WebSerial when connected

      String buffer = "!,"+(String)redGlobal+","+(String)blueGlobal+","+(String)greenGlobal;    // TODO: Sprawdzić czy działa
      WebSerial(buffer);
    }
    break;
    case WStype_TEXT:                                                                           // if new text data is received
      Serial.printf("[%u] get Text: %s\n", num, payload);
      if (payload[0] == '#') {                                                                  // we get RGB data
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);                 // decode rgb data
        int r = ((rgb >> 20) & 0x3FF);                                                          // 10 bits per color, so R: bits 20-29
        int g = ((rgb >> 10) & 0x3FF);                                                          // G: bits 10-19
        int b =          rgb & 0x3FF;                                                           // B: bits  0-9

        redGlobal = r;
        greenGlobal = g;
        blueGlobal = b;

        if (r + g + b != 0) {
          pirMode = false;
        }
        else {
          // pirMode = true;
        }

        analogWrite(LED_RED,   r);                         // write it to the LED output pins
        analogWrite(LED_GREEN, g);
        analogWrite(LED_BLUE,  b);
      } else if (payload[0] == 'R') {                      // the browser sends an R when the rainbow effect is enabled
        rainbow = true;
        pirMode = false;
        //Serial.println("PIR3");
      } else if (payload[0] == 'N') {                      // the browser sends an N when the rainbow effect is disabled
        rainbow = false;
        //pirMode = true;
        //Serial.println("PIR4");
      } else if (payload[0] == '1'){                      // SR type of switch for K1
        setK(1);
        Serial.println("K1");
      } else if (payload[0] == '2'){
        setK(2);                                          // SR type of switch for K2
        Serial.println("K2");
      } else if (payload[0] == '3'){
        setK(3);                                           // SR type of switch for K3
        Serial.println("K3");
      } else if (payload[0] == '4'){
        setK(4);                                           // SR type of switch for K4
        Serial.println("K4");
      } else if (payload[0] == 'r'){
        ESP.reset();
        Serial.println("ESPreset");
      } else if(payload[0] == 'p'){                         // SR PIR mode
        pirMode = !pirMode;
        MP = 0;
        digitalWrite(LED_RED, MP);                          // Write the right color to the LED output pins
        digitalWrite(LED_GREEN, MP);
        digitalWrite(LED_BLUE, MP);
        Serial << pirMode << endl;
      } else if(payload[0] == 'D'){
        SPIFFS.remove("/temp.csv");
        SPIFFS.remove("/hum.csv");
      } else if(payload[0] == 'X'){
        String data = "";
        data = String(t-tM) + "," + String(h-hM);
        Serial << "data= \"" << data << "\"" << endl;
        // TODO: deltaTemp i deltaHum
        webSocket.sendTXT(num, data);
      }
    break;
  }
}

void WebSerial(String payload) {
  if (webSerialStatus) // bool if client avilable
  {
    webSocket.sendTXT(globalAdressCilent, payload);
  }
}

/*__________________________________________________________HELPER_FUNCTIONS__________________________________________________________*/

String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes + "B");
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void PIR(){
  float photoResistorVoltage = analogRead(A0)*PhotoR;
  int pirSensorState = digitalRead(pir);

  String buffer = "PhotoResistorValue = " + (String)photoResistorVoltage + ", PIR = " + (String)pirSensorState; // TODO: Sprawdzić czy działa
  WebSerial(buffer);

  Serial << "PIR = " << pirSensorState << " " << photoResistorVoltage << endl;

  if((MP == 0)&&(photoResistorVoltage >= PhotoR_v)&&(pirSensorState == 1)){
    pirTime = millis();
    MP = 1;                       // State of led LIGHT
    digitalWrite(LED_RED, MP);    // Write the right color to the LED output pins
    digitalWrite(LED_GREEN, MP);
    digitalWrite(LED_BLUE, MP);
    Serial.print("ON ");
  }
  if ((MP == 1)&&(millis() - pirTime >= pirOffDelay)) {                                                //  if millis - pirTime is greater than pirOffDelay, turn off laights
    MP = 0;                                                                                                     // State of led LIGHT                                                                                            // Reset pirOffDelay
    digitalWrite(LED_RED, MP);                                                                                  // Write the right color to the LED output pins
    digitalWrite(LED_GREEN, MP);
    digitalWrite(LED_BLUE, MP);
    Serial.print("OFF ");
  }
}

void setK(int K){                       //  function steering relay
  Serial << "setK= " << K << endl;
  if(K == 1) {
    K1 = !K1;
    digitalWrite(relayK1,K1);
  } else if(K == 2) {
    K2 = !K2;
    digitalWrite(relayK2,K2);
  } else if(K == 3) {
    K3 = !K3;
    digitalWrite(relayK3,K3);
  } else if(K == 4) {
    K4 = !K4;
    digitalWrite(relayK4,K4); // TODO: Top page info todo
  }
}

void setHue(int hue) {          // Set the RGB LED to a given hue (color) (0° = Red, 120° = Green, 240° = Blue)
  hue %= 360;                   // hue is an angle between 0 and 359°
  float radH = hue*3.142/180;   // Convert degrees to radians
  float rf, gf, bf;

  if(hue>=0 && hue<120){        // Convert from HSI color space to RGB
    rf = cos(radH*3/4);
    gf = sin(radH*3/4);
    bf = 0;
  } else if(hue>=120 && hue<240){
    radH -= 2.09439;
    gf = cos(radH*3/4);
    bf = sin(radH*3/4);
    rf = 0;
  } else if(hue>=240 && hue<360){
    radH -= 4.188787;
    bf = cos(radH*3/4);
    rf = sin(radH*3/4);
    gf = 0;
  }

  int r = rf*rf*1023;
  int g = gf*gf*1023;
  int b = bf*bf*1023;

  redGlobal = (int)r/4;
  greenGlobal = (int)g/4;
  blueGlobal = (int)b/4;

  char buffer[20];
  int pointier;

  pointier = sprintf(buffer+pointier,"!");
  pointier += sprintf(buffer+pointier,",%d",(int)(r/4));
  pointier += sprintf(buffer+pointier,",%d",(int)(g/4));
  pointier += sprintf(buffer+pointier,",%d",(int)(b/4));

  WebSerial(buffer);

  analogWrite(LED_RED,   r);    // Write the right color to the LED output pins
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE,  b);
}

uint32_t getTime() {
  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
  uint32_t UNIXTime = NTPTime - seventyYears;
  return UNIXTime;
}

void sendNTPpacket(IPAddress& address) {
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode

  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

inline int getSeconds(uint32_t UNIXTime) {
  return UNIXTime % 60;
}

inline int getMinutes(uint32_t UNIXTime) {
  return UNIXTime / 60 % 60;
}

inline int getHours(uint32_t UNIXTime) {
  return UNIXTime / 3600 % 24;
}

void deleteFirstLine(String filename){                // overwritting method
  File original = SPIFFS.open(filename, "r");
  String name_ = original.name();
  Serial.println(name_);
  if (!original) {
    Serial.print("- failed to open file "); Serial.println(filename);
  }else{
    int a = 0;
    Serial.print("- DELETING FROM FILE "); Serial.println(filename);
    //We skip the first line
    original.readStringUntil('\n');
    File temporary = SPIFFS.open("/TEMP_PATH.csv", "w+");
    if(!temporary){
      Serial.println("-- failed to open temporary file ");
    }else{
      while(original.available()){
        temporary.print(original.readStringUntil('\n')+"\n");
        a = 1;
      }
      temporary.close();
    }
    original.close();

    if(a == 1){
      if(SPIFFS.remove(filename)){
        Serial.println("Old file succesfully deleted");
      }else{
        Serial.println("Couldn't delete file");
      }
      if(SPIFFS.rename("/TEMP_PATH.csv",filename)){
        Serial.println("Succesfully renamed");
      }else{
        Serial.println("Couldn't rename file");
      }
      a = 0;
    }else{
      SPIFFS.remove(filename);
      SPIFFS.rename("/TEMP_PATH.csv",filename);
    }
  }
}
