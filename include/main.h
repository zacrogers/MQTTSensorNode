#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>

#include "credentials.h"

typedef struct 
{
    uint8_t light;
    float humidity;
    float temperature;
}sensor_vals_t;

void read_sensors(sensor_vals_t* sensor_vals);
void mqtt_connect();