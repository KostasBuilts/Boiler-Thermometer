#include <Arduino.h>
#include <ATTinyCore.h>
#include <avr/interrupt.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <sleep.h>

// Data wire is plugged into port 1 on the Arduino
#define ONE_WIRE_BUS PB1

// Data wire is plugged into port 3 on the Arduino
#define ONE_WIRE_BUS2 PB0

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS2);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire ONeWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature Sensors(&ONeWire);

//Initializing the sleep class and setting the watchdog brownout timer
SleepClass sleep(5);

unsigned int msgCount = 0; 
unsigned int mean_water_temp = 0; 
unsigned int vbat = 0; 
unsigned int mean_air_temp = 0;
float bat;
int count = 0;

/**
 * @brief Creates a JSON object
 * @param None 
 * @return A string JSON object
 */
String json_data_prep()
{
  String json = "{\"Sensor\":\"Boiler\",\"ID\":12345678,\"msg\":";
  json += (unsigned int)msgCount;
  json += ",\"waterTemp\":";
  json += (unsigned int)mean_water_temp;
  json += ",\"airTemp\":";
  json += (unsigned int)mean_air_temp;
  json += ",\"VBat\":";
  json += (unsigned int)vbat;
  json += ",\"RSSI\":\"xxx\"}";

  return json;
}

void setup() 
{
  sensors.begin();
  Sensors.begin();
  // Start loRa
  if (!LoRa.begin(412E6)) 
  {
    while (1);
  }
}

void loop()
{
  Sensors.requestTemperatures();
  float tempC = Sensors.getTempCByIndex(0)*100.0;
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    mean_air_temp = mean_air_temp + tempC;
  } 
  else
  {
    mean_air_temp = mean_air_temp -255;
  }

  sensors.requestTemperatures();
  float tempC2 = sensors.getTempCByIndex(0)*100.0;
  if(tempC2 != DEVICE_DISCONNECTED_C) 
  {
    mean_water_temp = mean_water_temp + tempC2;
  } 
  else
  {
    mean_water_temp = mean_water_temp -255;
  }

  ++msgCount;
  ++count;

  if(count == 8)
  {
    mean_air_temp = mean_air_temp / 8;
    mean_water_temp = mean_water_temp / 8;
    bat = (analogRead(PA3)*(0.00322580645161))*2000.0;
    vbat = bat;
    LoRa.beginPacket();
    LoRa.print(json_data_prep()); //send packet formed by the JSON object generator
    LoRa.endPacket();
    mean_air_temp = 0;
    count = 0;
  }
  sleep.system_sleep();
}

ISR(WDT_vect)
{
  // nothing here
}

//LoRa TX current 107mA
//LoRa Sleep current 0mA
//Attiny idle 8Mhz 4.7mA
//Attiny idle 1Mhz 1.2mA