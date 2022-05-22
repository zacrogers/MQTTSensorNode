#include <Arduino.h>
#include "main.h"

constexpr uint8_t ANALOG_PIN = A0;
constexpr uint8_t MUX_A = D5;
constexpr uint8_t MUX_B = D7;

constexpr uint8_t MAX_FAILED_CONECTIONS = 5;

sensor_vals_t sensor_vals = { 0, 0.0f, 0.0f };

uint8_t failed_connections = 0;

typedef enum
{
    LIGHT = 0,
    HUMIDITY,
    TEMPERATURE
}Topic;

const char* topics[3] = {
    "/light",
    "/humidity",
    "/temperature"
};

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;

void setup() 
{
    Serial.begin(115200);

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqtt_server, 1883);

	dht.setup(D6, DHTesp::DHT11);
	pinMode(MUX_A, OUTPUT);
	pinMode(MUX_B, OUTPUT);
	digitalWrite(MUX_A, 0);
	digitalWrite(MUX_B, 0);

    // read_sensors(&sensor_vals);
    // if (!client.connected()) 
    // {
    //     // reconnect();
    //     mqtt_connect();
    //     // client.publish(topics[TEMPERATURE], String(sensor_vals.temperature).c_str());
    // }
    // else
    // {
    //     // client.publish(topics[LIGHT],       String(sensor_vals.light).c_str());
    //     // client.publish(topics[HUMIDITY],    String(sensor_vals.humidity).c_str());
    //     client.publish(topics[TEMPERATURE], String(sensor_vals.temperature).c_str());
    // }   

    // ESP.deepSleep(10e6); // uS 
}

void loop() 
{
    read_sensors(&sensor_vals);
    if (!client.connected()) 
    {
        // reconnect();
        mqtt_connect();
    }
    else
    {
        client.publish(topics[LIGHT],       String(sensor_vals.light).c_str());
        client.publish(topics[HUMIDITY],    String(sensor_vals.humidity).c_str());
        client.publish(topics[TEMPERATURE], String(sensor_vals.temperature).c_str());
    }   
    client.loop();
}

void read_sensors(sensor_vals_t* sensor_vals)
{
	int analog_read = 0;
	digitalWrite(MUX_A, 1);
	digitalWrite(MUX_B, 0);
	sensor_vals->light = analogRead(ANALOG_PIN); 
	delay(100);

	digitalWrite(MUX_A, 0);
	digitalWrite(MUX_B, 0);
	analog_read = analogRead(ANALOG_PIN); 
	sensor_vals->temperature = (analog_read * 1.0 / 1024) * 100;
	delay(500);

    float humidity = dht.getHumidity();
    delay(2000);

    if(!isnan(humidity))
    {
        sensor_vals->humidity = humidity;
    }


    Serial.print("Temp: ");
    Serial.println(sensor_vals->temperature);
}

void mqtt_connect()
{
    // Loop until we're reconnected
    while (!client.connected()) 
    {
        Serial.print("Attempting MQTT connection...");

        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);

        // Attempt to connect
        if (client.connect(clientId.c_str())) 
        {
            Serial.println("connected");
        } 
        else 
        {
            // if(failed_connections++ >= MAX_FAILED_CONECTIONS)
            // {
            //     ESP.deepSleep(30e6); // uS 
            // }

            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }   
}