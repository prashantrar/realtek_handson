/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <avr/dtostrf.h>


#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11

#define BOARD_ID "4000"



// Update these with values suitable for your network.

char ssid[] = "IOT_MERCURY_59E0";     // your network SSID (name)
char pass[] = "1234567890";  // your network password
int status  = WL_IDLE_STATUS;    // the Wifi radio's status

char mqttServer[] = "m10.cloudmqtt.com";

char clientId[]       = "amebaClient";
char clientUser[]     = "qjetscfh";
char clientPass[]     = "0M49osqO3srw";
char publishTopic[]   = "PubTop";
char publishPayload[] = "hello world";
char subscribeTopic[] = "SubTop";

const int SenPin = 13;
bool SenState = HIGH;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId, clientUser, clientPass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(publishTopic, publishPayload);
      // ... and resubscribe
      client.subscribe(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

DHT dht(DHTPIN, DHTTYPE);
void setup()
{
  Serial.begin(38400);
  pinMode(SenPin, INPUT);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  dht.begin();
  client.setServer(mqttServer, 12670);
  client.setCallback(callback);

  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  
  // Wait a few seconds between measurements.
  delay(10000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();


  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  static char humidity[10];
  static char temperature[10];
  dtostrf(h, 5, 3, humidity);
  dtostrf(t, 5, 3, temperature);
  static char buffer[128];
  snprintf(buffer, 20, "%s,%s", humidity, temperature);
  client.publish(BOARD_ID, buffer);
  delay(300);
  client.loop();
}
