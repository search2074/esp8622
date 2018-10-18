
/*
  MQTT DHT22, when "temperature, c" is sent it returns the temperature in celcius
  when "humidity" is sent it returns the humidity as measured by the DHT22
  the signal pin is connected to a pull-ip resistor and GPIO 2
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* ssid = "test";
const char* password = "12345678";
const char* mqtt_server = "192.168.0.106";
const char* espTopic = "/esp/test";
String espName = "test";
bool afterIs = false;
int gpio, val;
String gpioStr,valStr;
 
WiFiClient espClient;
PubSubClient client(espClient);
char msg[100];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  //Serial.println(WiFi.macAddress());
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;

  Serial.print("Message arrived on topic: [");
  Serial.print(topic);
  Serial.print("], ");
  Serial.println(message);

  for(int i=0;i<length;i++){
    if(!afterIs){
      if(message[i]=='='){
        afterIs=true;
      }
      else{
        gpioStr+=message[i];
      }
    }
    else{
      valStr += message[i];
    }
  }
  gpioHandler();
}

void reconnectMQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(espName.c_str())) {
      Serial.println("connected");
      
      client.publish(espTopic, espName.c_str());
      client.subscribe(espTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(0,OUTPUT);
  pinMode(2,OUTPUT);
//  pinMode(1,OUTPUT);
//  pinMode(3,OUTPUT);
//  pinMode(4,OUTPUT);
//  pinMode(5,OUTPUT);
}

void loop() {

  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
}

void gpioHandler(){
  gpio = atoi(gpioStr.c_str());
  val = atoi(valStr.c_str());
  
  digitalWrite(gpio,(val==1)?HIGH:LOW);
  
  
  Serial.print("gpio: ");
  Serial.println(gpioStr);
  Serial.print("val: ");
  Serial.println(valStr);
  
  afterIs = false;
  val=0;
  valStr = "";
  gpio = 0;
  gpioStr="";
}

void blink(){
  Serial.print("BLINKING");
  digitalWrite(2,HIGH);
  delay(200);
  digitalWrite(2,LOW);
  delay(200);
  blink();
}
