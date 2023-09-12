/*
 * To use this sketch you need to connect the VL53L3CX satellite sensor directly to the Nucleo board with wires in this way:
 * pin 1 (Interrupt) of the VL53L3CX satellite connected to pin A2 of the Nucleo board 
 * pin 2 (SCL_I) of the VL53L3CX satellite connected to pin D15 (SCL) of the Nucleo board with a Pull-Up resistor of 4.7 KOhm
 * pin 3 (XSDN_I) of the VL53L3CX satellite connected to pin A1 of the Nucleo board
 * pin 4 (SDA_I) of the VL53L3CX satellite connected to pin D14 (SDA) of the Nucleo board with a Pull-Up resistor of 4.7 KOhm
 * pin 5 (VDD) of the VL53L3CX satellite connected to 3V3 pin of the Nucleo board
 * pin 6 (GND) of the VL53L3CX satellite connected to GND of the Nucleo board
 * pins 7, 8, 9 and 10 are not connected.
 */
/* Includes ------------------------------------------------------------------*/
// #define I2C_BUFFER_LENGTH 256

#include <Arduino.h>
#include <Wire.h> //I2C_BUFFER_LENGTH=256
#include <vl53lx_class.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>


#define DEV_I2C Wire
#define SerialPort Serial

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif
#define LedPin LED_BUILTIN

// Components.
VL53LX sensor_vl53lx_sat(&DEV_I2C, D0);


// smoothing distance measurement
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;

/* Setup ---------------------------------------------------------------------*/

void setup()
{
   // Led.
   pinMode(LedPin, OUTPUT);

   for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings[thisReading] = 0;
   }      

   // Initialize serial for output.
   SerialPort.begin(115200);
   SerialPort.println("Starting...");

   // Initialize I2C bus.
   DEV_I2C.begin();

   // Configure VL53LX satellite component.
   sensor_vl53lx_sat.begin();

   // Switch off VL53LX satellite component.
   sensor_vl53lx_sat.VL53LX_Off();

   //Initialize VL53LX satellite component.
   sensor_vl53lx_sat.InitSensor(0x29);

   sensor_vl53lx_sat.VL53LX_SetMeasurementTimingBudgetMicroSeconds(300);
   
   // Start Measurements
   sensor_vl53lx_sat.VL53LX_StartMeasurement();
   sensor_vl53lx_sat.VL53LX_StopMeasurement();
   sensor_vl53lx_sat.VL53LX_StartMeasurement();
}

// allow sleeping if distance hasn't changed
int lastDistance = 0;
int wakeUntil = 0;



void displayStatus(int dist) {
   lastDistance = dist;
   SerialPort.print(dist);
   SerialPort.println("mm");
}



void loop()
{
   VL53LX_MultiRangingData_t MultiRangingData;
   VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
   uint8_t NewDataReady = 0;
   int no_of_object_found = 0, j;
   char report[64];
   int status;
   delay(100);
   do
   {
      status = sensor_vl53lx_sat.VL53LX_GetMeasurementDataReady(&NewDataReady);
      // if (wakeUntil > millis()) {
      // } else {
      //    delay(1000);
      // }
   } while (!NewDataReady);

   //Led on
   digitalWrite(LedPin, HIGH);

   if((!status)&&(NewDataReady!=0))
   {
      status = sensor_vl53lx_sat.VL53LX_GetMultiRangingData(pMultiRangingData);
      no_of_object_found=pMultiRangingData->NumberOfObjectsFound;
      if (no_of_object_found > 0) {
         if (pMultiRangingData->RangeData[0].RangeStatus == VL53LX_RANGESTATUS_RANGE_VALID) {
            int dist = pMultiRangingData->RangeData[0].RangeMilliMeter;

            total -= readings[readIndex];
            readings[readIndex] = dist;
            total += readings[readIndex];
            readIndex++;
            if (readIndex >= numReadings)
               readIndex = 0;

            displayStatus(total / numReadings);
         } else {
            Serial.print("Invalid Status:");
            Serial.println(pMultiRangingData->RangeData[0].RangeStatus);
         }
      }

      if (status==0)
      {
         status = sensor_vl53lx_sat.VL53LX_ClearInterruptAndStartMeasurement();
      }
   }

   digitalWrite(LedPin, LOW);
}

