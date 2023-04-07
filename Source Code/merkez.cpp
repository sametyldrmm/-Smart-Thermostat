#include <PubSubClient.h>
#include <Arduino.h>
#include <WiFi.h>
#include <string>

char __SPLITTED[6][300];
// WiFi settings
const char* ssid = "redmi";
const char* password = "12345678";
WiFiClient espClient;

// MQTT settings
const char* mqtt_server = "broker.hivemq.com";
const char* __STD_TOPIC = "5325892003";
const char* __CLIENT_ID = "MERKEZ";
PubSubClient client(espClient);

//merkez settings
#define relay_pin 25
char ON_OFF = '0';
int __RELAY_STATE = 0;

void ft_split(char *s, char c)
{
	size_t	i;
	size_t	w_i;

	i = 0;
	while (i < 4)
	{
		while (*s == c && *s != '\0')
			s++;
		w_i = 0;
		while (*s != c && *s != '\0')
			__SPLITTED[i][w_i++] = *s++;
		__SPLITTED[i][w_i] = '\0';
		i++;
	}
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(__CLIENT_ID)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(__STD_TOPIC);
      Serial.println("Subscribed");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
// // CLIENT_ID - SUANKI SICAKLIK - ISTENEN SICAKLIK - ON/OFF

void callback(char* topic, byte* message, unsigned int length) {

    String messageTemp;

    for (int i = 0; i < length; i++) {
        messageTemp += (char)message[i];
    }
    if (String(topic) == __STD_TOPIC) {
        ft_split((char*)message, '-');
        if ((strcmp(__SPLITTED[0], "MOBIL") == 0) || (strcmp(__SPLITTED[0], "ODA") == 0)) {
            ON_OFF = __SPLITTED[3][0];
            Serial.print("Message arrived on topic: ");
            Serial.print(topic);
            Serial.print(". Message: ");
            Serial.println(messageTemp);

            char sending_message[300];
            if (ON_OFF == '1') {
                __RELAY_STATE = 1;
            }
            else if (ON_OFF == '0') {
                __RELAY_STATE = 0;
            }
            sprintf(sending_message, "%s-%s-%d-%d", __CLIENT_ID, __SPLITTED[1], __SPLITTED[2], __RELAY_STATE);
            int len = strlen(sending_message);
            client.publish(__STD_TOPIC, (unsigned char *)sending_message,len,true);
            digitalWrite(relay_pin, __RELAY_STATE);
            Serial.print("RELAY_STATE : ");
            Serial.println(__RELAY_STATE);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    
    initWiFi(); // wifi initializer
    
    pinMode(relay_pin,OUTPUT);
    
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}