/*
   Copyright (C) 2022 SFini

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  * @file Data.h
  * 
  * Class with all the global runtime data.
  */
#pragma once

#include "Utils.h"
#include "weather.h"
#include <nvs.h>

#define CHARGE_HISTORY_SIZE 725
#define PPV_HISTORY_SIZE    725
#define GRID_HISTORY_SIZE   320
#define MAX_FORECAST  8

const DateTime EmptyDateTime(2000, 1, 1, 0, 0, 0);

class Huawei
{
public:   
   double      voltage;     
   double      ampere;       
   double      power;        
   double      grid_power; 
   DateTime    lastChange;   
   double      boiler_status;
   double      boiler_power;
   double      boiler_water;
   double      grid_l1_power;
   double      grid_l1_voltage;
   double      grid_l1_current;
   double      grid_l2_power;
   double      grid_l2_voltage;
   double      grid_l2_current;
   double      grid_l3_power;
   double      grid_l3_voltage;
   double      grid_l3_current;
   double      pv1_voltage;
   double      pv1_current;
   double      pv2_voltage;
   double      pv2_current;
   double      pv_peak;
   double      gas;
   double      water;
   double      elektrika;
   double      temp;
   String      fve_state;
   String      alive;        
   float       historyPower[MAX_FORECAST];
   int         maxPower;
   float       historyYeld[MAX_FORECAST];
   int         maxYeld;
   double      panelPower;
   double      yieldToday;

public:
   Huawei()
      : voltage(0.0)
      , ampere(0.0)
      , power(0.0)
      , grid_power(0.0)
      , lastChange(EmptyDateTime)
      , boiler_status(0.0)
      , boiler_power(0.0)
      , boiler_water(0.0)
      , grid_l1_power(0.0)
      , grid_l1_voltage(0.0)
      , grid_l1_current(0.0)
      , grid_l2_power(0.0)
      , grid_l2_voltage(0.0)
      , grid_l2_current(0.0)
      , grid_l3_power(0.0)
      , grid_l3_voltage(0.0)
      , grid_l3_current(0.0)
      , pv1_voltage(0.0)
      , pv1_current(0.0)
      , pv2_voltage(0.0)
      , pv2_current(0.0)
      , pv_peak(0.0)
      , gas(0.0)
      , water(0.0)
      , elektrika(0.0)
      , temp(0.0)
      , maxPower(0)
      , maxYeld(0)
      , panelPower(0.0)
      , yieldToday(0.0)
   {
   }

   void Dump();
};

/**
  * Class for collecting all the global data.
  */
class MyData
{
public:
   uint16_t     nvsCounter;       //!< Non volatile counter

   int          wifiRSSI;         //!< The wifi signal strength
   float        batteryVolt;      //!< The current battery voltage
   int          batteryCapacity;  //!< The current battery capacity
   int          sht30Temperatur;  //!< SHT30 temperature
   int          sht30Humidity;    //!< SHT30 humidity

   Huawei       huawei;     //!< The Tasmota Elite data
   Weather      weather;          //!< All the openweathermap data

public:
   MyData()
      : wifiRSSI(0)
      , batteryVolt(0.0)
      , batteryCapacity(0)
      , sht30Temperatur(0)
      , sht30Humidity(0)
   {
   }

   void Dump();
   void LoadNVS();
   void SaveNVS();
};


/* helper function to dump all the collected data */
void MyData::Dump()
{
   Serial.println("DateTime: "         + getRTCDateTimeString());
    
   Serial.println("WifiRSSI: "         + String(wifiRSSI));
   Serial.println("BatteryVolt: "      + String(batteryVolt));
   Serial.println("BatteryCapacity: "  + String(batteryCapacity));
   Serial.println("Sht30Temperatur: "  + String(sht30Temperatur));
   Serial.println("Sht30Humidity: "    + String(sht30Humidity));
   
}

/* Load the NVS data from the non volatile memory */
void MyData::LoadNVS()
{
   nvs_handle nvs_arg;
   nvs_open("Setting", NVS_READONLY, &nvs_arg);
   nvs_get_u16(nvs_arg, "nvsCounter", &nvsCounter);
   nvs_close(nvs_arg);
}

/* Store the NVS data to the non volatile memory */
void MyData::SaveNVS()
{
   nvs_handle nvs_arg;
   nvs_open("Setting", NVS_READWRITE, &nvs_arg);
   nvs_set_u16(nvs_arg, "nvsCounter", nvsCounter);
   nvs_commit(nvs_arg);
   nvs_close(nvs_arg);
}


