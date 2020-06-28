/*
Based on code from:
https://codeiot.org.br/ IOT105EN Apps for mobile devices - Lesson 9
https://circuitdigest.com/microcontroller-projects/build-a-nodemcu-webserver-to-control-led-from-webpage

Usage
http://IPADDRESS/LED=ON
http://IPADDRESS/LED=OFF
http://IPADDRESS/FREQ=1000
*/

#include <ESP8266WiFi.h>
WiFiServer server(80);

const char* ssid = "SECRET_SSID";  // Enter SSID here
const char* password = "SECRET_PWD";  //Enter Password here

String header; // Variable to store the HTTP request
String LEDState = "off";
int blinkFreq = 0;

#define LED_PIN 2 //Onboard LED ESP board
//#define LED_PIN D0 //Onboard LED NodeMCU
// #define LED_PIN LED_BUILTIN //e.g. Wemos D1 mini  

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);     // Initialize the LED_PIN pin as an output
  digitalWrite(LED_PIN, HIGH);  //Logic level reversed on builtin LED: HIGH=OFF
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected-->");
  Serial.println("Copy this IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {                             // If a new client connects,
    Serial.println("new client connected");
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected())
      {
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("/LED=ON HTTP") != -1)
            {
              Serial.println("LED_PIN on");
              blinkFreq = 0;
              LEDState = "on";
            }
            else if (header.indexOf("/LED=OFF HTTP") != -1)
            {
              Serial.println("LED_PIN off");
              blinkFreq = 0;
              LEDState = "off";
            }
            else if (header.indexOf("GET /FREQ") != -1)
            {
              // searches number to delay used in blink
              int start_pos = header.indexOf("GET /FREQ") + 10;
              int end_pos = header.indexOf("HTTP/1.1") - 1;
              String valueStr = header.substring(start_pos, end_pos);
              blinkFreq = valueStr.toInt();
              LEDState = "flash";
              Serial.println("Blink frequency: " + blinkFreq);
            }

            break;
            
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }  
  }

  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<body><h1>WIFI LED</h1>");
  client.println("<p>LED_State: " + LEDState + "</p>");
  client.println("</body></html>");
  client.println();

  // Set the LED state
  if (LEDState == "on") {
    digitalWrite(LED_PIN, LOW);
  }
  else if (LEDState == "off") {
    digitalWrite(LED_PIN, HIGH);
  }
  else if (LEDState = "flash") {
    digitalWrite(LED_PIN, HIGH);
    delay(blinkFreq);
    digitalWrite(LED_PIN, LOW);
    delay(blinkFreq);
  }
  
  header = "";
  client.stop();
  Serial.println("Client disconnected");
  Serial.println("");
}
