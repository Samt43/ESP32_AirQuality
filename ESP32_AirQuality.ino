#include <U8g2lib.h>

/* SDS011 PINEOUT
 * SDS011 pineout on ESP_32 Serial2
 * SDS011 TXD on GPIO-16 (ESP_32 RXD)
 * SDS011 RXD on GPIO-17 (ESP_32 TXD)
 * GRND + 5V wherever you want.
 */

/* SCREEN PIENOUT, default i2c is on
 *  SDA : GPIO 21
 *  SCL : GPIO 22
 *  VCC is 3.3V in my case
 */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

/*
 * LED is blinking each time we receive a data from sds011
 * 
 */
int LED_BUILTIN = 0;

/*
 * sds011 output size
 */
static int T_SIZE = 10;

/*
 * Print sds011 output on serial, for debuging
 */
void printTrame(const uint8_t* data) {

  for (int i = 0 ; i < T_SIZE ; i++)
  {
      char hexCar[3];
      hexCar[3] = '\n';
      sprintf(hexCar, "%02X", data[i]);
      Serial.print(hexCar);
  }
  
  Serial.println("");
}

/*
 * Setup function runs once when you press reset or power the board
 */
void setup() {
  /*
   * initialize digital pin LED_BUILTIN as an output.
   * On my ESP_32, this prevent from flashing, don't do it
   */
  //pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);
  Serial2.begin(9600); // 9600 baud, SERIAL_8N1 by default is OK for sds011 (see datasheet)

  // Init screen library
  u8g2.begin();

  initBLEService();
}

/*
 * Check SDS011 dreceived data is valid according to checksum and datasheet.
 * returns True if valid, false otherwise
 */
bool checkTrame(const uint8_t* data)
{
    bool ret = false;
    if (data[0] == 0xAA && data[1] == 0xC0 && data[T_SIZE - 1] == 0xAB)
    {
         uint8_t total = (data[2] + data[3] + data[4] + data[5] + data[6] + data[7]);
         uint8_t check = data[8];
         if (check == total)
         {
             Serial.println("Valid Checksum !");
             ret = true; 
         }
         else
         {
             Serial.println("InValid Checksum !");
             Serial.print(check);
             Serial.print(":");
             Serial.println(total);
         }
    }
    else
    {
            Serial.println("Invalid Header / END");
    }

    return ret;
 
}

/*
 * Process a received Trame given in parameter
 * returns True if success, false otherwise
 */
bool processTrame(const uint8_t* data)
{
    bool ret= false;
    // We check the datas are valid before doing anything
    if (checkTrame(data))
    {
        Serial.println("Processing :");

        // See SDS011 datasheet for this
        float pm_2_5 = (data[2] + (data[3] << 8)) / 10.f;
        float pm_10 = (data[4] + (data[5] << 8)) / 10.f;

        // Print datas on debug serial, for debuging
        Serial.println("Concentrations : ");
        Serial.print("PM 2.5 : ");
        Serial.println(pm_2_5);
        Serial.print("PM 10 : ");
        Serial.println(pm_10);

        // We display datas on device's screen
        display(pm_2_5, pm_10);

        // BLE update
        setBLEPMValues(pm_2_5, pm_10);
        // Success !
        ret = true;
    }
    else
    {
        Serial.println("Cannot process invalid Trame");
    }

    return ret;
}

/*
 * Display on Device's screen measures from sds011
 */
void display(float pm_2_5, float pm_10)
{
    char text1[25];
    char text2[25];
    sprintf(text1, "pm2.5: %.1fug/m3", pm_2_5);
    sprintf(text2, "pm10: %.1fug/m3", pm_10);
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_t0_15b_mf);
        u8g2.drawStr(0,15,text1);
        u8g2.drawStr(0,40,text2);
    } while ( u8g2.nextPage() );
}

// the loop function runs over and over again forever
void loop() {
    uint8_t data[T_SIZE];
    uint8_t* dataPtr = data;
    int lastTimeReceived = 0;

    while (true)
    {
        // As soon as we have data avalable
        while (Serial2.available()) {
            digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
            int oldLasTimeReceived = lastTimeReceived;
            int currentTime = millis();

            if ((currentTime - lastTimeReceived) > 500)
            {
               /*
                * Due to the fact previous datas are old, and SDS011 emits new datas every 1s we are sure that this is the begining of a new Trame here !
                * We process the old one before begining receiving the new one.
                */
                digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)  
                printTrame(data);

               if (!processTrame(data))
               {
                   Serial.println("Error occured During Processing");
               }

               Serial.println("\nReset, a new trame is coming");
               dataPtr = data;
            }

            // Update lastTimeReceived timestamp so that we can detect the begining of a new Trame
            lastTimeReceived = currentTime;
            // Read the current data
            int byteRead = Serial2.read();

            /*
            * SDS011 should always send data in 10 bytes, we check anyway we don't overflow here
            */
            if ((dataPtr - data) <= sizeof(data))
            {
                *dataPtr = byteRead;
            }
            else
            {
                Serial.println("\nError, too many datas in a Trame !!!");
            }
 
            dataPtr++;
        }
    }
}
