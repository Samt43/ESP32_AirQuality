# ESP32_AirQuality
Device that measures and displays PM concentations using SDS011 sensor

## Hardware  : 

- SDS011 PM sensor

- Screen OLED 128 x 64 px 0,96 Pouces, U8g2lib compatible

- ESP32 NodeMCU WLAN CP2102

###Pinout  : 

* SDS011 PINOUT :
 
   * SDS011 pineout on ESP_32 Serial2
 
   * SDS011 TXD on GPIO-16 (ESP_32 RXD)
 
   * SDS011 RXD on GPIO-17 (ESP_32 TXD)
 
   * GRND + 5V wherever you want.
 
 
* SCREEN PINOUT, default i2c :

   * SDA : GPIO 21
 
   * SCL : GPIO 22
 
   * VCC is 3.3V in my case
 

  
  
