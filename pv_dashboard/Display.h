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
  * @file Display.h
  * 
  * Main class for drawing the content to the e-paper display.
  */
#pragma once
#include "Data.h"
#include "Icons.h"


M5EPD_Canvas canvas(&M5.EPD); // Main canvas of the e-paper

/* Main class for drawing the content to the e-paper display. */
class SolarDisplay
{
protected:
   MyData &myData; //!< Reference to the global data
   int     maxX;   //!< Max width of the e-paper
   int     maxY;   //!< Max height of the e-paper

protected:
   void   DrawCircle            (int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom = 0, int32_t degTo = 360);
   void   DrawIcon              (int x, int y, const uint16_t *icon, int dx = 64, int dy = 64, bool highContrast = false);
   void   DrawGraph             (int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[]);
   String FormatString          (String format, double data, int fillLen = 4); 
   void   DrawHead              (int x, int y, int dx, int dy);
   void   DrawHeadVersion       (int x, int y);
   void   DrawHeadUpdated       (int x, int y);
   void   DrawHeadRSSI          (int x, int y);
   void   DrawHeadBattery       (int x, int y);
   void   DrawBody              (int x, int y, int dx, int dy);
   void   DrawBatteryInfo       (int x, int y, int dx, int dy);
   void   DrawSolarSymbol       (int x, int y, int dx, int dy);
   void   DrawSolarArrow        (int x, int y, int dx, int dy);
   void   DrawGridInfo          (int x, int y, int dx, int dy);
   void   DrawBatterySymbol     (int x, int y, int dx, int dy);
   void   DrawBatteryArrow      (int x, int y, int dx, int dy);
   void   DrawInverterSymbol    (int x, int y, int dx, int dy);
   void   DrawInverterArrow     (int x, int y, int dx, int dy);
   void   DrawHouseSymbol       (int x, int y, int dx, int dy);
   void   DrawGridArrow         (int x, int y, int dx, int dy);
   void   DrawGridSymbol        (int x, int y, int dx, int dy);
   void   DrawSolarInfo         (int x, int y, int dx, int dy);

public:
   SolarDisplay(MyData &md, int x = 960, int y = 540)
      : myData(md)
      , maxX(x)
      , maxY(y)
   {
   }

   void Show();
   void ShowWiFiError(String ssid);
};

/* Draw a circle with optional start and end point */
void SolarDisplay::DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom /* = 0 */, int32_t degTo /* = 360 */)
{
   for (int i = degFrom; i < degTo; i++) {
      double radians = i * PI / 180;
      double px      = x + r * cos(radians);
      double py      = y + r * sin(radians);
      
      canvas.drawPixel(px, py, color);
   }
} 

/* Draw one icon from the binary data */
void SolarDisplay::DrawIcon(int x, int y, const uint16_t *icon, int dx /*= 64*/, int dy /*= 64*/, bool highContrast /*= false*/)
{
   for (int yi = 0; yi < dy; yi++) {
      for (int xi = 0; xi < dx; xi++) {
         uint16_t pixel = icon[yi * dx + xi];

         if (highContrast) {
            if (15 - (pixel / 4096) > 0) canvas.drawPixel(x + xi, y + yi, M5EPD_Canvas::G15);
         } else {
            canvas.drawPixel(x + xi, y + yi, 15 - (pixel / 4096));
         }
      }
   }
}

/* Draw a graph with x- and y-axis and values */
void SolarDisplay::DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[])
{
   String yMinString = String(yMin);
   String yMaxString = String(yMax);
   int    textWidth  = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
   int    graphX     = x + 5 + textWidth + 5;
   int    graphY     = y + 35;
   int    graphDX    = dx - textWidth - 20;
   int    graphDY    = dy - 35 - 20;
   float  xStep      = graphDX / (xMax - xMin);
   float  yStep      = graphDY / (yMax - yMin);
   int    iOldX      = 0;
   int    iOldY      = 0;

   canvas.setTextSize(2);
   canvas.drawCentreString(title, x + dx / 2, y + 10, 1);
   canvas.setTextSize(1);
   canvas.drawString(yMaxString, x + 5, graphY - 5);   
   canvas.drawString(yMinString, x + 5, graphY + graphDY - 3);   
   for (int i = 0; i <= (xMax - xMin); i++) {
      canvas.drawString(String(i), graphX + i * xStep, graphY + graphDY + 5);   
   }
   
   canvas.drawRect(graphX, graphY, graphDX, graphDY, M5EPD_Canvas::G15);   
   if (yMin < 0 && yMax > 0) { // null line?
      float yValueDX = (float) graphDY / (yMax - yMin);
      int   yPos     = graphY + graphDY - (0.0 - yMin) * yValueDX;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.drawString("0", graphX - 20, yPos);   
      for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10) {
         canvas.drawLine(xDash, yPos, xDash + 5, yPos, M5EPD_Canvas::G15);         
      }
   }
   for (int i = xMin; i <= xMax; i++) {
      float yValue   = values[i - xMin];
      float yValueDY = (float) graphDY / (yMax - yMin);
      int   xPos     = graphX + graphDX / (xMax - xMin) * i;
      int   yPos     = graphY + graphDY - (yValue - yMin) * yValueDY;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.fillCircle(xPos, yPos, 2, M5EPD_Canvas::G15);
      if (i > xMin) {
         canvas.drawLine(iOldX, iOldY, xPos, yPos, M5EPD_Canvas::G15);         
      }
      iOldX = xPos;
      iOldY = yPos;
   }
}

/* Printf operation as String. */
String SolarDisplay::FormatString(String format, double data, int fillLen /*= 9*/)
{
   char buff[100];

   memset(buff, 0, sizeof(buff));
   sprintf(buff, format.c_str(), data);

   String fill;
   int    padLen = (int) fillLen - (int) strlen(buff);

   for (int i = 0; i < padLen; i++) {
      fill += ' ';
   }

   return fill + buff;
}

/* ********************************************************************************************* */

/* Draw the version text.  */
void SolarDisplay::DrawHeadVersion(int x, int y)
{
   canvas.drawString(VERSION, x, y);
}

/* Draw the information when are these data updated. */
void SolarDisplay::DrawHeadUpdated(int x, int y)
{
   String updatedString = "Updated " + getDateTimeString(GetRTCTime());
   
   canvas.drawCentreString(updatedString, x, y, 1);
}

/* Draw the wifi reception strength */
void SolarDisplay::DrawHeadRSSI(int x, int y)
{
   int iQuality = WifiGetRssiAsQualityInt(myData.wifiRSSI);

   canvas.drawRightString(WifiGetRssiAsQuality(myData.wifiRSSI) + "%", x - 2, y - 14, 1);
   
   if (iQuality >= 80) DrawCircle(x + 12, y, 16, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 40) DrawCircle(x + 12, y, 12, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 20) DrawCircle(x + 12, y,  8, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 10) DrawCircle(x + 12, y,  4, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >=  0) DrawCircle(x + 12, y,  2, M5EPD_Canvas::G15, 225, 315); 
}

/* Draw the state of charge. */
void SolarDisplay::DrawHeadBattery(int x, int y)
{
   canvas.drawRect(x, y, 40, 16, M5EPD_Canvas::G15);
   canvas.drawRect(x + 40, y + 3, 4, 10, M5EPD_Canvas::G15);
   for (int i = x; i < x + 40; i++) {
      canvas.drawLine(i, y, i, y + 15, M5EPD_Canvas::G15);
      if ((i - x) * 100.0 / 40.0 > myData.batteryCapacity) {
         break;
      }
   }
   canvas.drawRightString(String(myData.batteryCapacity) + "%", x - 2, y + 1, 1);
}

/* Draw all the information about the battery status. */
void SolarDisplay::DrawBatteryInfo(int x, int y, int dx, int dy)
{
   //DateTime toDay                          = GetRTCTime();
   //TimeSpan timeSpan                       = toDay - myData.bmv.lastChange;
   String   pv1voltage                       = FormatString("%.0fV",  myData.huawei.pv1_voltage,                 4);
   String   pv1current                       = FormatString("%.2fA",  myData.huawei.pv1_current,                 4);
   String   pv2voltage                       = FormatString("%.0fV",  myData.huawei.pv2_voltage,                 4);
   String   pv2current                       = FormatString("%.2fA",  myData.huawei.pv2_current,                 4);
   String   powerpeak                        = FormatString("%.0fW",  myData.huawei.pv_peak,                     4);

   canvas.drawRect(x, y, dx, dy, M5EPD_Canvas::G15);

   canvas.drawString("PV1 Voltage:", x +   5, y +  14); canvas.drawString(pv1voltage,                x +   170, y +  14);
   canvas.drawString("PV1 Current:", x +   5, y +  34); canvas.drawString(pv1current,                x +   170, y +  34);

   canvas.drawString("PV2 Voltage:", x +   5, y +  64); canvas.drawString(pv2voltage,                x +   170, y +  64);
   canvas.drawString("PV2 Current:", x +   5, y +  84); canvas.drawString(pv2current,                x +   170, y +  84);

   canvas.drawString("PV PowerPeak:", x +   5, y +  114); canvas.drawString(powerpeak,                x +   170, y +  114);
   canvas.drawString(String(myData.huawei.fve_state), x +   5, y +  134); 

   DrawIcon(x + dx - 34, y + dy - 34, (uint16_t *) image_data_SolarIconSmall, 30, 30);
}

/* Draw a solar symbol. */
void SolarDisplay::DrawSolarSymbol(int x, int y, int dx, int dy)
{
   DrawIcon(x, y, (uint16_t *) image_data_SolarIcon, 150, 150);

   canvas.setTextSize(3);
   canvas.drawRightString(String(myData.huawei.panelPower, 0)      + "W ", x + dx, y + 13, 1);
   canvas.drawRightString(String(myData.huawei.yieldToday, 1) + "kWh", x + dx, y + 45, 1);
   canvas.setTextSize(2);
}

/* Draw the Solar arror if the mppt current flows. */
void SolarDisplay::DrawSolarArrow(int x, int y, int dx, int dy)
{
   DrawIcon(x, y, (uint16_t *) image_data_LineUpDown, 4, 47);
   if (myData.huawei.panelPower > 0.0) {
      DrawIcon(x + 14, y + 5, (uint16_t *) image_data_ArrowDown, 20, 39);
   }
}

/* Draw all the Grid information. */
void SolarDisplay::DrawGridInfo(int x, int y, int dx, int dy)
{
   double      t_voltage;
   double      t_current;
   double      t_power;

   t_voltage = (myData.huawei.grid_l1_voltage + myData.huawei.grid_l2_voltage + myData.huawei.grid_l3_voltage) / 3;
   t_current = myData.huawei.grid_l1_current + myData.huawei.grid_l2_current + myData.huawei.grid_l3_current;
   t_power = myData.huawei.grid_l1_power + myData.huawei.grid_l2_power + myData.huawei.grid_l3_power;   

   //DateTime toDay       = GetRTCTime();
   //TimeSpan timeSpan    = toDay - myData.tasmotaElite.lastChange;
   String   voltageInfo1 = FormatString("%.0fV", myData.huawei.grid_l1_voltage,  5);
   String   ampereInfo1  = FormatString("%.1fA", myData.huawei.grid_l1_current,  5);
   String   powerInfo1  = FormatString("%.0fW", myData.huawei.grid_l1_power,  5);

   String   voltageInfo2 = FormatString("%.0fV", myData.huawei.grid_l2_voltage,  5);
   String   ampereInfo2  = FormatString("%.1fA", myData.huawei.grid_l2_current,  5);
   String   powerInfo2  = FormatString("%.0fW", myData.huawei.grid_l2_power,  5);

   String   voltageInfo3 = FormatString("%.0fV", myData.huawei.grid_l3_voltage,  5);
   String   ampereInfo3  = FormatString("%.1fA", myData.huawei.grid_l3_current,  5);
   String   powerInfo3  = FormatString("%.0fW", myData.huawei.grid_l3_power,  5);

   String   t1 = FormatString("%.0fV", t_voltage,  5);
   String   t2  = FormatString("%.1fA", t_current,  5);
   String   t3  = FormatString("%.0fW", t_power,  5);

   canvas.drawRect(x, y, dx, dy, M5EPD_Canvas::G15);
   //(x + 436, y +  10, 486, 166)
   canvas.drawRect(x + 10, y + 10, 100, 146, M5EPD_Canvas::G15);
   canvas.setTextSize(2);
   canvas.drawString("L1", x + 14, y + 14);
   canvas.setTextSize(3);
   canvas.drawString(voltageInfo1, x + 15, y + 52);
   canvas.drawString(ampereInfo1,  x + 15, y + 80);
   canvas.drawString(powerInfo1,  x + 15, y + 108);

   canvas.drawRect(x + 120, y + 10, 100, 146, M5EPD_Canvas::G15);
   canvas.setTextSize(2);
   canvas.drawString("L2", x + 124, y + 14);
   canvas.setTextSize(3);
   canvas.drawString(voltageInfo2, x + 126, y + 52);
   canvas.drawString(ampereInfo2,  x + 126, y + 80);
   canvas.drawString(powerInfo2,  x + 126, y + 108);

   canvas.drawRect(x + 230, y + 10, 100, 146, M5EPD_Canvas::G15);
   canvas.setTextSize(2);
   canvas.drawString("L3", x + 234, y + 14);
   canvas.setTextSize(3);
   canvas.drawString(voltageInfo3, x + 235, y + 52);
   canvas.drawString(ampereInfo3,  x + 235, y + 80);
   canvas.drawString(powerInfo3,  x + 235, y + 108);


   canvas.drawRect(x + 340, y + 10, 100, 146, M5EPD_Canvas::G15);
   canvas.setTextSize(2);
   canvas.drawString("TOTAL", x + 344, y + 14);
   canvas.setTextSize(3);
   canvas.drawString(t1, x + 344, y + 52);
   canvas.drawString(t2,  x + 344, y + 80);
   canvas.drawString(t3,  x + 344, y + 108);

   canvas.setTextSize(2);
   
   DrawIcon(x + dx - 40, y + dy - 40, (uint16_t *) image_data_HouseIconSmall, 30, 30);
}

/* Draw a battery symbol. */
void SolarDisplay::DrawBatterySymbol(int x, int y, int dx, int dy)
{
   DrawIcon(x - 30, y, (uint16_t *) image_data_boiler, 98, 100);

   int zero          = y + dy - 22;
   int full          = y + 2;
   int stateOfCharge = (myData.huawei.boiler_water / 0.75);  //75Celsius the maxim temp of boiler so it's 100%
   int state         = zero - (zero - full) / 100.0 * stateOfCharge;

   for (int i = zero; i > full; i--) {
      canvas.drawLine(x + 9, i, x + dx + 4, i, M5EPD_Canvas::G15);
      if (i < state) {
         break;
      }
   }

   if (stateOfCharge < 40) {   
      canvas.drawCentreString(String(myData.huawei.boiler_water, 0) + "C", x + 38, y + (dy / 2) - 8, 1);
   } else if (stateOfCharge > 65) {   
      canvas.setTextColor(BLACK, WHITE);
      canvas.drawCentreString(String(myData.huawei.boiler_water, 0) + "C", x + 38, y + (dy / 2) - 8, 1);
   } else {
      canvas.setTextColor(BLACK, WHITE);
      canvas.drawCentreString(String(myData.huawei.boiler_water, 0) + "C", x + 38, y + dy - 30, 1);
   }
   canvas.setTextColor(WHITE, BLACK);
}

/* Draw the battery inverter connection. */
void SolarDisplay::DrawBatteryArrow(int x, int y, int dx, int dy)
{
   canvas.setTextSize(3);
   canvas.drawCentreString(String(myData.huawei.boiler_power, 0) + "W", x + (dx / 2) - 5, y + 13, 1);
   canvas.setTextSize(2);
   
   DrawIcon(x, y + 48, (uint16_t *) image_data_LineLeftRight, 110, 4);
   
   if (myData.huawei.boiler_status > 0) {
      DrawIcon(x + 16, y + 68, (uint16_t *) image_data_ArrowLeft, 70, 15);
   } else {
      //DrawIcon(x + 16, y + 68, (uint16_t *) image_data_ArrowRight, 70, 15);
      canvas.setTextSize(3);
      canvas.drawString("OFF", x + 26, y + 68);
   }
}

/* Draw a inverter symbol. */
void SolarDisplay::DrawInverterSymbol(int x, int y, int dx, int dy)
{
   DrawIcon(x, y, (uint16_t *) image_data_InverterIcon, 100, 100);
}

/* Draw the inverter house connection. */
void SolarDisplay::DrawInverterArrow(int x, int y, int dx, int dy)
{
   DrawIcon(x, y + 48, (uint16_t *) image_data_LineLeftRight, 110, 4);
   canvas.setTextSize(3);

   if (myData.huawei.power > 0.0) {
      canvas.setTextSize(3);
      canvas.drawCentreString(String(myData.huawei.power, 0) + "W", x + (dx / 2) - 5, y + 13, 1);
      canvas.setTextSize(2);
      DrawIcon(x + 16, y + 68, (uint16_t *) image_data_ArrowRight, 70, 15);
    } else {    
      canvas.drawString("OFF", x + 35, y + 13);
   }
   canvas.setTextSize(2);
}

/* Draw a house symbol. */
void SolarDisplay::DrawHouseSymbol(int x, int y, int dx, int dy)
{
   DrawIcon(x, y, (uint16_t *) image_data_HouseIcon, 100, 100);
}

/* Draw the house grid connection. */
void SolarDisplay::DrawGridArrow(int x, int y, int dx, int dy)
{
   DrawIcon(x, y + 48, (uint16_t *) image_data_LineLeftRight, 110, 4);
   canvas.setTextSize(3);
   if (myData.huawei.grid_power > 0.0) {
      canvas.drawString(String(myData.huawei.grid_power, 0) + "W", x + 5, y + 13);
      DrawIcon(x + 16, y + 68, (uint16_t *) image_data_ArrowRight, 70, 15);
   } else {
      canvas.drawString(String(myData.huawei.grid_power*(-1), 0) + "W", x + 5, y + 13);
      DrawIcon(x + 16, y + 68, (uint16_t *) image_data_ArrowLeft, 70, 15);
   }
   canvas.setTextSize(2);
}

/* Draw a grid symbol. */
void SolarDisplay::DrawGridSymbol(int x, int y, int dx, int dy)
{
   DrawIcon(x, y, (uint16_t *) image_data_GridIcon, 60, 100);
}

/* Draw all solar panel data. */
void SolarDisplay::DrawSolarInfo(int x, int y, int dx, int dy)
{
   String   gas   = FormatString("%.2fm3",myData.huawei.gas ,   3);
   String   water   = FormatString("%.0fliters",myData.huawei.water ,   3);
   String   elektrika   = FormatString("%.2fkWh",myData.huawei.elektrika ,   3);
   String   temp   = FormatString("%.2fC",myData.huawei.temp ,   3);

  //DrawSolarInfo      (x +  10, y + 316, 912, 168);
   canvas.drawRect(x,  y + 125, dx, dy-125, M5EPD_Canvas::G15);

   //canvas.drawString("Other",            x + 14, y +  14); 
   //canvas.drawString("Gas:",             x + 15, y + 140); canvas.drawString(gas,                 x +   70, y + 140);
   //canvas.drawString("Water:",           x + 250, y + 140); canvas.drawString(water,               x +   340, y + 140);
   //canvas.drawString("Power:",           x + 485, y + 140); canvas.drawString(elektrika,           x +   570, y + 140);
   //canvas.drawString("Temp(AVG):",       x + 717, y + 140); canvas.drawString(temp,                x +   837, y + 140);
   canvas.drawString("Consumption is " + gas + " of Gas, " + water + " of Water and " +elektrika+ " of power" ,             x + 15, y + 140);
   
   canvas.setTextSize(2);
   DrawGraph( 15, 350, 232, 122, "Temperature (C)", 0, 7, -20,   30, myData.weather.forecastMaxTemp);
   DrawGraph( 15, 350, 232, 122, "Temperature (C)", 0, 7, -20,   30, myData.weather.forecastMinTemp);
   DrawGraph(247, 350, 232, 122, "Power consumption",       0, 7,   0,   myData.huawei.maxPower, myData.huawei.historyPower);
   DrawGraph(479, 350, 232, 122, "Clouds (%)",    0, 7,   0,  100, myData.weather.forecastClouds);
   DrawGraph(711, 350, 232, 122, "Yeld (kWh)",  0, 7, 0, myData.huawei.maxYeld, myData.huawei.historyYeld);
   //DrawIcon(x + dx - 40, y + dy - 40, (uint16_t *) image_data_BatteryIconSmall, 13, 24);
}

/* Draw the complete head. */
void SolarDisplay::DrawHead(int x, int y, int dx, int dy)
{
   DrawHeadVersion  (x + 5,         y + 13);
   DrawHeadUpdated( (x + (dx / 2)), y + 13);
   DrawHeadRSSI     (x + dx - 130,  y + 26);
   DrawHeadBattery  (x + dx -  49,  y + 11);
}

/* Draw the whole solar information body. */
void SolarDisplay::DrawBody(int x, int y, int dx, int dy)
{
   canvas.drawRect(x, y, dx, dy, M5EPD_Canvas::G15);

   DrawBatteryInfo    (x +  10, y +  10, 250, 166);
   DrawSolarSymbol    (x + 276, y +  30, 150, 150);
   DrawSolarArrow     (x + 346, y + 140,  50,  50);
   DrawGridInfo       (x + 436, y +  10, 486, 166);
   DrawBatterySymbol  (x +  96, y + 196,  60, 100);
   DrawBatteryArrow   (x + 171, y + 196, 110, 100);
   DrawInverterSymbol (x + 296, y + 196, 100, 100);
   DrawInverterArrow  (x + 411, y + 196, 110, 100);
   DrawHouseSymbol    (x + 536, y + 196, 100, 100);
   DrawGridArrow      (x + 651, y + 196, 110, 100);
   DrawGridSymbol     (x + 776, y + 196,  60, 100);
   DrawSolarInfo      (x +  10, y + 316, 912, 168);
}

/* Fill the screen. */
void SolarDisplay::Show()
{
   Serial.println("SolarDisplay::DrawSolarInfo");

   canvas.setTextSize(2);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);
   canvas.createCanvas(maxX, maxY);

   DrawHead(14,  0, maxX - 28, 33);
   DrawBody(14, 34, maxX - 28, maxY - 45);

   canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
   delay(2000);
}

/* Show WiFi connewction error. */
void SolarDisplay::ShowWiFiError(String ssid)
{
   Serial.println("SolarDisplay::ShowWiFiError");

   canvas.setTextSize(4);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);
   canvas.createCanvas(maxX, maxY);

   String errMsg = "WiFi error: [" + ssid + "]";
   canvas.drawCentreString(errMsg, maxX / 2, maxY / 2, 1);

   canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
}
