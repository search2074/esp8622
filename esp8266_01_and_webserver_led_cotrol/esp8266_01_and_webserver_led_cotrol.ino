#include "ESP8266WiFi.h"

const char* ssid = "test";//type your ssid
const char* password = "12345678";//type your password
IPAddress ip(192,168,0,74);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

WiFiServer server(80);//Service Port
int ledPin = 2; // GPIO2 of ESP8266

void setup() {
  Serial.begin(115200);

  delay(10000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(10000);
  
  

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println(" ");
  Serial.print("scan done, wifi routers: ");
  Serial.print(n);
  
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);

  Serial.println("scan start");

  
  pinMode(ledPin, OUTPUT);
//  digitalWrite(ledPin, LOW);
  
  

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      /**
       * 0 - WL_IDLE_STATUS when Wi-Fi is in process of changing between statuses
       * 1 - WL_NO_SSID_AVAIL in case configured SSID cannot be reached
       * 2 - WL_SCAN_COMPLETED
       * 3 - WL_CONNECTED after successful connection is established
       * 4 - WL_CONNECT_FAILED if password is incorrect
       * 5 - WL_CONNECTION_LOST
       * 6 - WL_DISCONNECTED if module is not configured in station mode
       * 255 - WL_NO_SHIELD // for compatibility with WiFi Shield library
       */
    
      delay(1000);
      Serial.print(". ");
      Serial.println(WiFi.status());
  }

  WiFi.config(ip, gateway, subnet);
  
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
      return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
      delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request
  
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
      digitalWrite(ledPin, HIGH);
      value = HIGH;
  } 
  if (request.indexOf("/LED=OFF") != -1){
      digitalWrite(ledPin, LOW);
      value = LOW;
  }
  
  //Set ledPin according to the request
  //digitalWrite(ledPin, value);

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  
  client.print("Led pin is now: ");
  
  if(value == HIGH) {
      client.print("On");
  } else {
      client.print("Off");
  }

  client.println("<br><br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 2 ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here turn the LED on pin 2 OFF<br>");
  client.println("</html>");
  
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
