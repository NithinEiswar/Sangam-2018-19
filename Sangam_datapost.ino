#include <ArduinoJson.h>

#include <FirebaseArduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#define FIREBASE_HOST "sangam-a3c1a.firebaseio.com" //Your Firebase Project URL goes here without "http:" , "\" and "/" 
#define FIREBASE_AUTH "f4sWbNdFasemcHw3IRJKSJr8pQkxf0hOCYo9540z" //Your Firebase Database Secret goes here
#define address 10
#define imp 0.00028
 int meter_pin = D7;
 int relay=D6;
 
// Replace with your network credentials
const char* ssid = "Sudhar..!";
const char* password = "ni6ga2rd";
 
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
 
String page = "<html> \n <body>";
double data =0.000; 
void setup(void){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  pinMode(D5,OUTPUT);
  pinMode(meter_pin, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(115200);
   EEPROM.begin(512);
   data=EEPROM.read(address);
   delay(1000);
   Serial.println("data value is");
   Serial.println(data);
   delay(1000);
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/data.txt", [](){

    server.send(200, "text/html", String(data));
  });
  server.on("/turnoff",turnoff);
  server.on("/turnon",turnon); 
  server.on("/", [](){
   page = "<h1>Sensor to Node MCU Web Server</h1><h1>Data:</h1> <h1 id=\"data\">""</h1>\r\n";
   page += "<script>\r\n";
   page += "var x = setInterval(function() {loadData(\"data.txt\",updateData)}, 1000);\r\n";
   page += "function loadData(url, callback){\r\n";
   page += "var xhttp = new XMLHttpRequest();\r\n";
   page += "xhttp.onreadystatechange = function(){\r\n";
   page += " if(this.readyState == 4 && this.status == 200){\r\n";
   page += " callback.apply(xhttp);\r\n";
   page += " }\r\n";
   page += "};\r\n";
   page += "xhttp.open(\"GET\", url, true);\r\n";
   page += "xhttp.send();\r\n";
   page += "}\r\n";
   page += "function updateData(){\r\n";
   page += " document.getElementById(\"data\").innerHTML = this.responseText;\r\n";
   page += "}\r\n";
   page += "</script>\r\n";
   server.send(200, "text/html", page);
  });
  server.begin();
  Serial.println("Web server started!");
  digitalWrite(relay,HIGH);
  Firebase.setFloat("Units",data);                                                 // post to firebase here
        Serial.println("DATA to FIREBASE uploaded HERE");
 }
 int prev=0; unsigned int i=0;
 void turnoff()
 {
   digitalWrite(relay,LOW);
   server.send(200,"text/html","<html> <h1> Supply cut Due to unpaid bills....</h1> </html>");
 }
  void turnon()
 {
   digitalWrite(relay,HIGH);
   server.send(200,"text/html","<html> <h1> Supply resumes.....</h1> </html>");
 }
 void firebasereconnect()
{
  Serial.println("Trying to reconnect");
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  }
void loop(void)
{
  if (Firebase.failed()) 
  {
      Serial.print("setting number failed:");
      Serial.println(Firebase.error());
      firebasereconnect();
      return;
  }
  if((digitalRead(meter_pin) == LOW) && prev!=digitalRead(meter_pin))
{
  digitalWrite(D5,HIGH);
  data+=imp;
  i++;
        Firebase.setFloat("Units",data);                                                 // post to firebase here
        Serial.println("DATA to FIREBASE uploaded HERE");
  if(i%3600 == 0)
  {
    EEPROM.write(address,data);
    EEPROM.commit();
    Serial.println("IN!!!");
  }
  Serial.println();
  Serial.print(" Impulse number=");
  Serial.print(i);
  Serial.print(" Units consumed=");
  Serial.print(data,5);
  digitalWrite(LED_BUILTIN,LOW);
}
  prev=digitalRead(meter_pin);
  delay(10);
  digitalWrite(LED_BUILTIN,HIGH);
  server.handleClient();
  }
