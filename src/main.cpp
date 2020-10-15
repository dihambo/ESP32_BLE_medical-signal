#include "mSensorService.h"

#include <Arduino.h>


// SensorDataCollectorBase* ECGData = new ECG_SensorDataCollector(36);
void setup(){

    SensorCharacteristicBase* ECGCharacteristic = new ECG_SensorCharacteristic(ECG_CHARACTERISTIC_UUID,ECG_CHARACTERISTIC_UUID_CTR,36);
    SensorServiceBase* ECGSensorService = new SensorServiceBase()

}

void loop(){

}