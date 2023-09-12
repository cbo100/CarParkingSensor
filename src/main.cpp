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
#include <ReactESP.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include "distance.h"

using namespace reactesp;

ReactESP app;

int currentDistance = 0;
int awakeUntil = 0;


void displayStatus(int dist)
{
  Serial.print(dist);
  Serial.println("mm");
}

int previousAwakeCheckDistance = 0;
void awakeCheck(int dist) 
{
  Serial.println("waking");
  // have we moved > 100mm in the awake check duration
  if (abs(dist - previousAwakeCheckDistance) > 100)
    awakeUntil = millis() + 30000;

  if (awakeUntil > millis())
    digitalWrite(LED_BUILTIN_AUX, LOW);
  else
    digitalWrite(LED_BUILTIN_AUX, HIGH);

  previousAwakeCheckDistance = dist;
}

void setup()
{
  // Led.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN_AUX, OUTPUT);

  // Initialize serial for output.
  Serial.begin(74800);

  initialiseDistanceMeasurement();

  Serial.println("Starting...");
  app.onRepeat(10, [] { currentDistance = getDistanceMeasurement(); });
  // app.onRepeat(10000, [] { awakeCheck(currentDistance); });
  app.onRepeat(2000, [] { displayStatus(currentDistance); });
  app.onRepeat(20, [] {
    if (currentDistance < 400) // && awakeUntil > millis())
      digitalWrite(LED_BUILTIN, LOW);
    else
      digitalWrite(LED_BUILTIN, HIGH);
  });
}



void loop()
{
  app.tick();
}
