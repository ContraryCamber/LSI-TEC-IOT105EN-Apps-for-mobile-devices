//Based on code from https://circuitdigest.com/microcontroller-projects/build-a-nodemcu-webserver-to-control-led-from-webpage

#include <ESP8266WiFi.h>
const char* ssid = "Your_SSID_here";  // Enter SSID here
const char* password = "Your_WLAN_password";  //Enter Password here
// Variable to store the HTTP request
String header;
String LEDState = "off";
WiFiServer server(80);

#define LED_PIN 2 //Onboard LED ESP-12
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
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("/LED=ON HTTP") != -1) {
              Serial.println("LED_PIN on");
              LEDState = "on";
              digitalWrite(LED_PIN, LOW);
            } else if (header.indexOf("/LED=OFF HTTP") != -1) {
              Serial.println("LED_PIN off");
              LEDState = "off";
              digitalWrite(LED_PIN, HIGH);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Cairo; display: inline; margin: 0px auto; text-align: center; background-color: #ccffb3;}");
            client.println(".button { background-color: #006699; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 35px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>ESP12 LED Switch</h1>");
            client.println("<p>LED_State: " + LEDState + "</p>");
            // If the LEDState is off, it displays the ON button
            if (LEDState == "off") {
              client.println("<p><a href=\"/LED=ON\"><button class=\"button\">ON</button></a></p>");

            } else {
              client.println("<p><a href=\"/LED=OFF\"><button class=\"button button2\">OFF</button></a></p>");

            }
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
  }
  header = "";
  client.stop();
  Serial.println("Client disconnected");
  Serial.println("");
}
