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
  * @file getJsonData.h
  * 
  * Helper function to communicate with the IoBroker.
  */
#pragma once

#include <ArduinoJson.h>


/* ***************************************************************************** */
/* *** Get JSON DATA Values() ***************************************************** */
/* ***************************************************************************** */

// SAMPLE OF JSON
// {"l1_power":39.81,"l1_voltage":232.11,"l1_current":1.53,"l2_power":50.23,"l2_voltage":236.05,"l2_current":0.64,"l3_power":22.15,"l3_voltage":237.89,"l3_current":0.56,"fve_state":"grid-connected"
// ,"fve_pv_01_voltage":379,"fve_pv_01_current":0.39,"fve_pv_02_voltage":170.5,"fve_pv_02_current":0.38,"fve_input_power":202,"fve_grid_voltage":231.5,"fve_grid_current":0.93,"fve_day_active_power_peak":204
// ,"fve_active_power":194,"fve_accumulated_yield_energy":185.16,"fve_daily_yield_energy":0.12,"power_meter_active_power":-327,"boiler_status":0,"boiler_power":0,"boiler_water":"21.1","shelly_huawei_status":"1"
// ,"shelly_huawei_power":187,"power_history":[14.733799999998,18.210399999996,13.007399999999,11.664699999998,11.074999999997,13.0514,9.5926999999974,3.8765999999996]
// ,"yeld_history":[3.03,11.97,2.24,1.29,3.05,3.17,1.5,0.12],"water":100,"gas":1,"power":4,"temp":20.3}

void GetHTTPValues(MyData &myData)
{
String payload;
HTTPClient http;
  
  http.begin(URL);
  int httpCode = http.GET();
  // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.print("[HTTP] GET... code:");
      Serial.println(httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
      }
    } else {
      Serial.print("[HTTP] GET... failed, error:");
      Serial.println(http.errorToString(httpCode).c_str());
    }
  http.end();  

  DynamicJsonDocument doc(10 * 1024);

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonArray dayly_list  = doc["power_history"];
      for (int i = 0; i < MAX_FORECAST; i++) {
         if (i < dayly_list.size()) {
            myData.huawei.historyPower[i]  = dayly_list[i].as<float>();
            if (myData.huawei.historyPower[i] > myData.huawei.maxPower) {
               myData.huawei.maxPower = myData.huawei.historyPower[i];
            }
         }
      }

  JsonArray dayly_list2  = doc["yeld_history"];
      for (int i = 0; i < MAX_FORECAST; i++) {
         if (i < dayly_list2.size()) {
            myData.huawei.historyYeld[i]  = dayly_list2[i].as<float>();
            if (myData.huawei.historyYeld[i] > myData.huawei.maxYeld) {
               myData.huawei.maxYeld = myData.huawei.historyYeld[i];
            }
         }
      }

  myData.huawei.panelPower = doc["fve_active_power"];
  myData.huawei.yieldToday = doc["fve_daily_yield_energy"];
  myData.huawei.power = doc["shelly_huawei_power"];
  myData.huawei.grid_power = doc["power_meter_active_power"];
  myData.huawei.boiler_status = doc["boiler_status"];
  myData.huawei.boiler_power = doc["boiler_power"];
  myData.huawei.boiler_water = doc["boiler_water"];

  myData.huawei.grid_l1_power = doc["l1_power"];
  myData.huawei.grid_l1_voltage = doc["l1_voltage"];
  myData.huawei.grid_l1_current = doc["l1_current"];

  myData.huawei.grid_l2_power = doc["l2_power"];
  myData.huawei.grid_l2_voltage = doc["l2_voltage"];
  myData.huawei.grid_l2_current = doc["l2_current"];

  myData.huawei.grid_l3_power = doc["l3_power"];
  myData.huawei.grid_l3_voltage = doc["l3_voltage"];
  myData.huawei.grid_l3_current = doc["l3_current"];

  myData.huawei.pv1_voltage = doc["fve_pv_01_voltage"];
  myData.huawei.pv1_current = doc["fve_pv_01_current"];
  myData.huawei.pv2_voltage = doc["fve_pv_02_voltage"];
  myData.huawei.pv2_current = doc["fve_pv_02_current"];

  myData.huawei.pv_peak = doc["fve_day_active_power_peak"];
  myData.huawei.fve_state = doc["fve_state"].as<String>();

  myData.huawei.gas = doc["gas"];
  myData.huawei.water = doc["water"];
  myData.huawei.elektrika = doc["power"];
  myData.huawei.temp = doc["temp"];

}
