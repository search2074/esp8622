#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//SSID and Password to your ESP Access Point
const char* ap_ssid = "ShockerSetup";
const char* ap_password = "12345678";
int gpio0 = 0;
bool hot_point_has_inited = false;
const short int BUILTIN_LED1 = 1; //GPIO1

ESP8266WebServer server(80); //Server on port 80

/**
 * Настройка устройства при включении
 */
void setup(void){
  Serial.begin(115200);
  pinMode(BUILTIN_LED1, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.println("");

  if(digitalRead(gpio0) == 0){
    
  }
  else {
    Serial.println("device has running as websocket reader");
  }
}

/**
 * Бесконечный цикл
 */
void loop(void){
  // включили режим настройки
  if(digitalRead(gpio0) == 0){
    blinkBlueLed(200);
    // если еще не было инициализации хот спота
    if(!hot_point_has_inited){
      Serial.println("access point starting...");
      initHotSpot();
      Serial.println("access point has started");
    }
    
    server.handleClient();          //Handle client requests
//    delay(2000);
//    Serial.print("gpio0 state: ");
//    Serial.print(digitalRead(gpio0));
//    Serial.println("");
//    delay(2000);
  }
}

void blinkBlueLed(int timeout){
  digitalWrite(BUILTIN_LED1, LOW); // Turn the LED ON by making the voltage LOW
  delay(timeout); // Wait for a second
  digitalWrite(BUILTIN_LED1, HIGH); // Turn the LED off by making the voltage HIGH
  delay(timeout); // Wait for a
}

/**
 * Инициализация access point и вебсервера
 */
void initHotSpot() {
  WiFi.mode(WIFI_AP);           //Only Access point
  IPAddress ap_local_IP(192,168,1,1);
  IPAddress ap_gateway(192,168,1,254);
  IPAddress ap_subnet(255,255,255,0);
  Serial.print("Configuring access point...");
  Serial.print("Setting soft-AP configuration ... "); 
  WiFi.softAPConfig(ap_local_IP, ap_gateway, ap_subnet);
  Serial.print("Setting soft-AP ... ");
  WiFi.softAP(ap_ssid, ap_password);  //Start HOTspot removing password will disable security
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", handleRoot);      //Which routine to handle at root location
  server.onNotFound(handleNotFound);
  server.begin();                  //Start server
  Serial.println("HTTP server started");
  hot_point_has_inited = true;
}

/**
 * Роут главной страницы веб сервера
 */
void handleRoot(){
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
