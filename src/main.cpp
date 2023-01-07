#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <sleep.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 3

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//Initializing the sleep class and setting the watchdog brownout timer
SleepClass sleep(9);

unsigned int msgCount = 0; 
unsigned int water_temp = 3800; 
unsigned int vbat = 3333; 
unsigned int air_temp = 0;

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
  json += (unsigned int)water_temp;
  json += ",\"airTemp\":";
  json += (unsigned int)air_temp;
  json += ",\"VBat\":";
  json += (unsigned int)vbat;
  json += ",\"RSSI\":\"xxx\"}";

  return json;
}

void setup() 
{
  sensors.begin();
  // Start loRa
  if (!LoRa.begin(412E6)) 
  {
    while (1);
  }
}

void loop()
{
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    air_temp = tempC * 100;
  } 
  else
  {
    air_temp = -255;
  }
  ++msgCount;
  LoRa.beginPacket();
  LoRa.print(json_data_prep()); //send packet formed by the JSON object generator
  LoRa.endPacket();
  sleep.system_sleep();
  //delay(1000);
}

ISR(WDT_vect)
{
  // nothing here
}

//LoRa TX current 107mA
//LoRa Sleep current 0mA
//Attiny idle 8Mhz 4.7mA
//Attiny idle 1Mhz 1.2mA