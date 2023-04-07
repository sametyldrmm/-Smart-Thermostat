#include <PubSubClient.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SICAKLIK_PIN 32
#define UP_TEMPERATURE 35
#define DOWN_TEMPERATURE 34
#define ON_RELAY_LED 33
#define OFF_RELAY_LED 25


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// WiFi settings
const char* ssid = "redmi";
const char* password = "12345678";
WiFiClient espClient;

// RELAY settings
const int RELAY_PIN = 25;

// DHT settings
//#define DHTTYPE    DHT11
//DHT dht(SICAKLIK_PIN, DHTTYPE);
int __SICAKLIK;
int __ISTENEN_SICAKLIK = 25;

// MQTT settings
const char* mqtt_server = "broker.hivemq.com";
const char* __STD_TOPIC = "5325892003";
const char* __CLIENT_ID = "ODA";
PubSubClient client(espClient);
char __SPLITTED[6][300];

int __RELAY_STATE = 0;
// MQTT Functions
//halletik
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
//

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

// CLIENT_ID - SUANKI SICAKLIK - ISTENEN SICAKLIK - ON/OFF
// yeni mesaj geldiği zaman ne yapılacağı fonksiyonu
// düzenlenecek
void callback(char* topic, byte* message, unsigned int length) {

    String messageTemp;
    
    for (int i = 0; i < length; i++) {
        messageTemp += (char)message[i];
    }
    if (String(topic) == __STD_TOPIC) {
        ft_split((char*)message, '-');
        if (strcmp(__SPLITTED[0], "MOBIL") == 0) {
            __ISTENEN_SICAKLIK = atoi(__SPLITTED[2]);
              Serial.print("Message arrived on topic: ");
              Serial.print(topic);
              Serial.print(". Message: ");
              Serial.println(messageTemp);
        }
    }
}

/*String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}
*/

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

uint8_t up_btn_prev = 0;
uint8_t down_btn_prev = 0;

void setup() {
    Serial.begin(115200);
    
    initWiFi(); // wifi initializer
    pinMode(SICAKLIK_PIN, INPUT);
    pinMode(UP_TEMPERATURE,INPUT_PULLUP);
    pinMode(DOWN_TEMPERATURE,INPUT_PULLUP);
    pinMode(ON_RELAY_LED,OUTPUT);
    pinMode(OFF_RELAY_LED,OUTPUT);
    
    up_btn_prev = digitalRead(UP_TEMPERATURE);
    
    down_btn_prev = digitalRead(DOWN_TEMPERATURE);

    //dht.begin();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
    }
    delay(2000);
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    // Display static text
    display.println("Hello, world!");
    display.display();
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
  
    uint8_t btn_up = digitalRead(UP_TEMPERATURE);
    if (btn_up == LOW && up_btn_prev == HIGH)
    {
        __ISTENEN_SICAKLIK++;
    }
    up_btn_prev = digitalRead(UP_TEMPERATURE);

    uint8_t btn_down = digitalRead(DOWN_TEMPERATURE);
    if (btn_down == LOW && down_btn_prev == HIGH)
    {
        __ISTENEN_SICAKLIK--;
    }
    down_btn_prev = digitalRead(DOWN_TEMPERATURE);
  
    char sending_message[500];

    __SICAKLIK = analogRead(SICAKLIK_PIN);

    if (__SICAKLIK < __ISTENEN_SICAKLIK + 1)
    {
        __RELAY_STATE = 1;
        sprintf(sending_message, "%s-%d-%d-%d", __CLIENT_ID, __SICAKLIK, __ISTENEN_SICAKLIK, __RELAY_STATE);
        int len = strlen(sending_message);
        client.publish(__STD_TOPIC, (unsigned char *) sending_message,len,true);
        Serial.println(sending_message);

    } else {
        __RELAY_STATE = 0;
        sprintf(sending_message, "%s-%d-%d-%d", __CLIENT_ID, __SICAKLIK, __ISTENEN_SICAKLIK, __RELAY_STATE);
        int len = strlen(sending_message);
        client.publish(__STD_TOPIC, (unsigned char *)sending_message,len,true);
        //print to serial monitor
        Serial.println(sending_message);
    }

    if (__RELAY_STATE == 1)
    {
        digitalWrite(OFF_RELAY_LED,LOW);
        digitalWrite(ON_RELAY_LED, HIGH);
    } else
    {
        digitalWrite(ON_RELAY_LED, LOW);
        digitalWrite(OFF_RELAY_LED, HIGH);
    }
    
    delay(1000);

    /*
    if (dht.readTemperature() < __SICAKLIK + 1)
    {
        sprintf(sending_message, "%s-%s-%d-%d-%s", __CLIENT_ID, readDHTTemperature().c_str(), __ISTENEN_SICAKLIK, 1, "ON");
        int len = strlen(sending_message) + 1;
        client.publish(__STD_TOPIC, (unsigned char *) sending_message,len,true);
        Serial.println(sending_message);

    } else
    {
        sprintf(sending_message, "%s-%s-%d-%d-%s", __CLIENT_ID, readDHTTemperature().c_str(), __ISTENEN_SICAKLIK, 1, "OFF");
        int len = strlen(sending_message) + 1;
        client.publish("MERKEZ/POWER", (unsigned char *)sending_message,len,true);
        //print to serial monitor
        Serial.println(sending_message);
    }   
    */    
}
