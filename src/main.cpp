#include "mSensorService.h"
#include "mConfig.h"

#include <Arduino.h>
#include <BLEDevice.h>

// SensorDataCollectorBase* ECGData = new ECG_SensorDataCollector(36);
void setup(){
    BLEDevice::init(BLE_NAME);
    BLEServer *pServer = BLEDevice::createServer(); 
    // todo 服务器回调
    SensorCharacteristicBase* ECGCharacteristic = new ECG_SensorCharacteristic(ECG_CHARACTERISTIC_UUID,ECG_CHARACTERISTIC_UUID_CTR,36);
    SensorServiceBase* ECGSensorService = new SensorServiceBase(pServer,ECG_SERVICE_UUID,ECGCharacteristic);

}

void loop(){

}