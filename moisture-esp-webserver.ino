#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include <ArduinoJson.h>
//#include <timer.h>


#define SERIAL_BUFFER_SIZE 500


const char* ssid = "wifi-network";
const char* password = "wifi-password";

//Static IP address configuration
IPAddress staticIP(192, 168, 1, 10); //ESP static ip
IPAddress gateway(192, 168, 1, 254);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //netmask
IPAddress dns(8, 8, 8, 8);  //DNS

/*
StaticJsonDocument<256> http_response;
JsonArray sensors = http_response.createNestedArray("sensors");
JsonObject sensor_0 = sensors.createNestedObject();
JsonObject sensor_1 = sensors.createNestedObject();
JsonObject sensor_2 = sensors.createNestedObject();
JsonObject sensor_3 = sensors.createNestedObject();
JsonObject sensor_4 = sensors.createNestedObject();
JsonObject sensor_5 = sensors.createNestedObject();
*/

//Timer<6> timer;

int serialCounter=0;

ESP8266WebServer server(80);

char sensors_json[500];

void wifi() {

  
  WiFi.mode( WIFI_STA );
  WiFi.config(staticIP, gateway, subnet, dns);
  WiFi.hostname("grass_monitor");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
}

void handleRoot() {

  //char resp[256];
  //serializeJson(http_response, resp);
          
  digitalWrite(LED_BUILTIN, 1);
  server.send(200, "text/json", "Garden sensors");
  digitalWrite(LED_BUILTIN, 0);
  
}

void handleStatus() { 
   server.send(200, "text/json", "OK");
}

void handleSensors() {
  
  server.send(200, "text/json", sensors_json);
  
}

void handleNotFound() {
  
  digitalWrite(LED_BUILTIN, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, 0);
}


void httpServer() {

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/sensors", handleSensors);
  server.onNotFound(handleNotFound);


  server.on("/patatina", [](){
    server.send(200, "text/plain", "Love you babe - kiss me!!!");
  });


  server.begin();
  Serial.println("HTTP server started");
  
}

void pin_setup() {
  
  //sd card workaround
  pinMode(LED_BUILTIN, OUTPUT);
  
}

/*void serial_read() {

  if (Serial.available() > 0) {
    
    sensors_json[serialCounter]=char(Serial.read()); 
    serialCounter++;
  }
  else {
    if (serialCounter > 0) {
      Serial.print("Received data: ");
      Serial.println(serialCounter);
      sensors_json[serialCounter]=NULL;
      serialCounter=0;
      Serial.println(sensors_json);
    }
  }
  
}*/

void serial_read() {

  if (Serial.available() > 0) {

    char _char = char(Serial.read());
    
    
    if (_char == '#') {
      serialCounter=0;//Serial.print("-----START---> ");Serial.println(serialCounter);
    }
    else if (_char == '$') { 
     sensors_json[serialCounter]=NULL;
     //serialCounter=0;
     //Serial.print("-----------END-------------- ");Serial.println(serialCounter);
     //Serial.println(sensors_json);
    }
    else {
      sensors_json[serialCounter]=_char; 
      //Serial.print(sensors_json[serialCounter]);Serial.print(" ");Serial.println(serialCounter);
      serialCounter++;
    }
  }
  else {
    if (serialCounter > 0) {
      //Serial.print("Received data: ");
      //Serial.println(serialCounter);
      //sensors_json[serialCounter]=NULL;
      //serialCounter=0;
      //Serial.println(sensors_json);
    }
  }

}

void setup() {

  Serial.begin(115200);
  wifi();
  httpServer();
  
  
}

void loop() {
  
  server.handleClient();
  serial_read();

}
