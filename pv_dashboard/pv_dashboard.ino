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
  * @file m5paper.ino
  * 
  * Main file with setup() and loop()
  */
  
#include <M5EPD.h>
#include "RTClib.h"
#include <TimeLib.h>
#include <HTTPClient.h>
#include "Config.h"
//#include "ConfigOverride.h" // Remove this line
#include "Data.h"
#include "Display.h"
#include "Battery.h"
#include "EPD.h"
#include "EPDWifi.h"
#include "getJsonData.h"
#include "SHT30.h"
#include "RTCTime.h"
#include "Utils.h"
#include <ArduinoJson.h>
#include "weather.h"

MyData       myData;            // The collection of the global data
SolarDisplay myDisplay(myData); // The global display helper class


/* Start and M5Paper instance */
void setup()
{


   // Serial default speed 115200
   InitEPD(true);
   if (!StartWiFi(myData.wifiRSSI)) {
      myDisplay.ShowWiFiError(WIFI_SSID);
   } else {
      UpdateRTCFromNTP();
      GetBatteryValues(myData);
      GetSHT30Values(myData);
      GetHTTPValues(myData);
      myData.weather.Get();
      myData.Dump();
      myDisplay.Show();
      StopWiFi();
   }
   ShutdownEPD(10 * 60); // every 10 minutes
}

/* Main loop. Never reached because of shutdown */
void loop()
{
}
