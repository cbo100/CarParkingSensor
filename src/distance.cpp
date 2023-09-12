#include <Wire.h> //I2C_BUFFER_LENGTH=256
#include <vl53lx_class.h>

#define DEV_I2C Wire
// Components.
VL53LX sensor_vl53lx_sat(&DEV_I2C, D0);

// smoothing distance measurement
const int numReadings = 3;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;

void initialiseDistanceMeasurement()
{
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = 0;
  }

  // Initialize I2C bus.
  DEV_I2C.begin();

  // Configure VL53LX satellite component.
  sensor_vl53lx_sat.begin();

  // Switch off VL53LX satellite component.
  sensor_vl53lx_sat.VL53LX_Off();

  // Initialize VL53LX satellite component.
  sensor_vl53lx_sat.InitSensor(0x29);

  sensor_vl53lx_sat.VL53LX_SetMeasurementTimingBudgetMicroSeconds(300);

  // Start Measurements
  sensor_vl53lx_sat.VL53LX_StartMeasurement();
  sensor_vl53lx_sat.VL53LX_StopMeasurement();
  sensor_vl53lx_sat.VL53LX_StartMeasurement();
}

int getDistanceMeasurement()
{
  VL53LX_MultiRangingData_t MultiRangingData;
  VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;
  int no_of_object_found = 0;
  int status;
  // Serial.println("getting measurement");
  do
  {
    status = sensor_vl53lx_sat.VL53LX_GetMeasurementDataReady(&NewDataReady);
    // if (wakeUntil > millis()) {
    // } else {
    //    delay(1000);
    // }
  } while (!NewDataReady);

  if ((!status) && (NewDataReady != 0))
  {
    status = sensor_vl53lx_sat.VL53LX_GetMultiRangingData(pMultiRangingData);
    no_of_object_found = pMultiRangingData->NumberOfObjectsFound;
    if (no_of_object_found > 0)
    {
      if (pMultiRangingData->RangeData[0].RangeStatus == VL53LX_RANGESTATUS_RANGE_VALID)
      {
        int dist = pMultiRangingData->RangeData[0].RangeMilliMeter;

        total -= readings[readIndex];
        readings[readIndex] = dist;
        total += readings[readIndex];
        readIndex++;
        if (readIndex >= numReadings)
          readIndex = 0;
      }
    }

    if (status == 0)
    {
      status = sensor_vl53lx_sat.VL53LX_ClearInterruptAndStartMeasurement();
    }

  }
  return total / numReadings;
}
