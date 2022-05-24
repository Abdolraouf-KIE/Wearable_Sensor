/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>

/*
* LAB: 22
* Name: ESP32 LM35 Temperature Sensor + Serial
* Author: Khaled Magdy
* For More Info Visit: www.DeepBlueMbedded.com
*/
 
#include "esp_adc_cal.h"
 
#define LM35_Sensor1    34  //32
 
int LM35_Raw_Sensor1 = 0;
float LM35_TempC_Sensor1 = 0.0;
float LM35_TempF_Sensor1 = 0.0;
float Voltage = 0.0;


uint32_t readADC_Cal(int ADC_Raw) // double check if this function works ok.
{
  esp_adc_cal_characteristics_t adc_chars;
  
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
}
 
unsigned long time;
unsigned long lastMsg;
bool print=0;
int count=0;

void loop()
{
    long now = millis();
    if (now - lastMsg > 100) {
    // Read LM35_Sensor1 ADC Pin
    LM35_Raw_Sensor1 = analogRead(LM35_Sensor1);  
    // Calibrate ADC & Get Voltage (in mV)
    Voltage = readADC_Cal(LM35_Raw_Sensor1);
    // TempC = Voltage(mV) / 10
    LM35_TempC_Sensor1 = Voltage / 10;
    LM35_TempF_Sensor1 = (LM35_TempC_Sensor1 * 1.8) + 32;
    lastMsg = now;
    count++;
    // tag for print
    // if (LM35_TempC_Sensor1>15)
    // {
        print=1;
    // }
    
    }

    if (print){
        // Print The Readings
        Serial.print(count/10);
        Serial.print(", ");
        // Serial.print("Temperature = ");
        Serial.print(LM35_TempC_Sensor1);
        Serial.print(", ");
        Serial.println(int(LM35_Raw_Sensor1));
        // Serial.println(" °C");
        // Serial.print("Temperature = ");
        // Serial.print(LM35_TempF_Sensor1);
        // Serial.println(" °F");
        print=0;
    }


    // delay(100);
}