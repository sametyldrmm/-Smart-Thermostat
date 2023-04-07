# Smart-thermostat

This project has been developed for the purpose of automating thermostats used in households. The devices and software used in the project measure the temperature in the house and control the heating/cooling system according to the desired temperature value.

## Material list 

To run the project, the following components are required:

- 2x ESP8266
- 2x DHT11
- 1x 16x2 LCD screen
- 2x buttons
- 1x 5V relay module
- 2x perforated plates

## Electronic

<p align="center">
  <img src="https://github.com/bilalorhanlr/Smart-Thermostat/blob/main/Schematic/boiler-schematic.png" width="503" title="hover text">
  <img src="https://github.com/bilalorhanlr/Smart-Thermostat/blob/main/Schematic/controller-schematic1.png" width="480" title="hover text">
</p>

## How does it work ?

<p align="center">
  <img src="https://github.com/bilalorhanlr/Smart-Thermostat/blob/main/Schematic/How-does-it-work%3F.png" width="900" title="hover text">
</p>

## .Stl files

- [Room Cover](https://github.com/bilalorhanlr/Smart-Thermostat/blob/main/Thermostat%20Box%20STL/kapak_oda.stl)
- [Room Box](https://github.com/bilalorhanlr/Smart-Thermostat/blob/main/Thermostat%20Box%20STL/oda_g%C3%B6vde.stl)
- [Boiler Cover](https://github.com/bilalorhanlr/Smart-Thermostat/blob/main/Thermostat%20Box%20STL/merkez_kapak.stl)
- [Boiler Box](https://github.com/bilalorhanlr/Smart-Thermostat/blob/main/Thermostat%20Box%20STL/merkez_g%C3%B6vde.stl)

## Source Code

Lastly, configure your own WiFi and MQTT settings and complete the project

##### Center-Box Codes

```c
// WiFi settings
const char* ssid = "phone_name";
const char* password = "12345678";
```


```c
// MQTT settings
const char* mqtt_server = "broker.hivemq.com";
const char* __STD_TOPIC = "553323xxxx";
const char* __CLIENT_ID = "MERKEZ";
```


##### Room-Box Codes

```c
// WiFi settings
const char* ssid = "phone_name";
const char* password = "12345678";
WiFiClient espClient;
```

```c
// MQTT settings
const char* mqtt_server = "broker.hivemq.com";
const char* __STD_TOPIC = "553323xxxx";
const char* __CLIENT_ID = "ODA";
```
