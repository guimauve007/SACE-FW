#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_MCP4728.h>

Adafruit_MCP4728 mcp;

// Replace the next variables with your SSID/Password combination
const char* ssid = "SACE-2.4GHz";
const char* password = "Admin123";

// MQTT Broker IP address:
const char* mqtt_server = "192.168.10.129";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
bool spindleFanState = 0;


// Accessoires
const int SpindleFanPin = 16;


void setup_wifi() {
  delay(10);
  // On se connecte au réseau WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  //filtrage des messages
  if (String(topic) == "ctl/spindleFanButton") {
    Serial.print("Received message:");
    Serial.print(messageTemp);    
    if(messageTemp == "TOGGLE"){
      spindleFanState = !spindleFanState;
    }
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
   
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(SpindleFanPin, OUTPUT);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("ctl/spindleFanButton");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    delay(1);
    if(spindleFanState){
      client.publish("/state/spindleFan", "ON");
    }else{
      client.publish("/state/spindleFan", "OFF");
    }
    Serial.println(spindleFanState);
    digitalWrite(SpindleFanPin, spindleFanState);
  
}