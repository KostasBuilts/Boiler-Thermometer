#include <Arduino.h>
#include <ATTinyCore.h>
#include <avr/interrupt.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <sleep.h>

#define CPU_PRESCALER 0x80

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 1

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//Initializing the sleep class and setting the watchdog brownout timer
SleepClass sleep(5);

unsigned int msgCount = 0; 
unsigned int water_temp = 3800; 
unsigned int vbat = 3333; 
unsigned int mean_air_temp = 0;
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
  json += (unsigned int)water_temp;
  json += ",\"airTemp\":";
  json += (unsigned int)mean_air_temp;
  json += ",\"VBat\":";
  json += (unsigned int)vbat;
  json += ",\"RSSI\":\"xxx\"}";

  return json;
}

void setup() 
{
  CLKPR = CPU_PRESCALER;
  sensors.begin();
  pinMode(9, OUTPUT);
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
    mean_air_temp = mean_air_temp + (tempC * 100);
  } 
  else
  {
    mean_air_temp = mean_air_temp -255;
  }

  ++msgCount;
  ++count;

  if(count == 8)
  {
    mean_air_temp = mean_air_temp / 8;
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