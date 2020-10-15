#pragma once

#include "mSensorCharacteristic.h"
#include <Arduino.h>
#include <BLEServer.h>

#define ECG_SERVICE_UUID                "0001181C-0000-1000-8000-00805F9B34FB" // UART service UUID
#define ECG_CHARACTERISTIC_UUID         "0003181C-0000-1000-8000-00805F9B34FB"
#define ECG_CHARACTERISTIC_UUID_CTR     "0002181C-0000-1000-8000-00805F9B34FB"
#define TEMP_SERVICE_UUID               "00042A1F-0000-1000-8000-00805F9B34FB" // UART service UUID
#define TEMP_CHARACTERISTIC_UUID        "00062A1F-0000-1000-8000-00805F9B34FB"
#define TEMP_CHARACTERISTIC_UUID_CTR    "00052A1F-0000-1000-8000-00805F9B34FB"
#define HRATE_SERVICE_UUID              "00072A37-0000-1000-8000-00805F9B34FB" // UART service UUID
#define HRate_CHARACTERISTIC_UUID       "00092A37-0000-1000-8000-00805F9B34FB"
#define HRate_CHARACTERISTIC_UUID_CTR   "00082A37-0000-1000-8000-00805F9B34FB"
#define SPO2_SERVICE_UUID               "00102A37-0000-1000-8000-00805F9B34FB" // UART service UUID
#define SPO2_CHARACTERISTIC_UUID        "00122A37-0000-1000-8000-00805F9B34FB"
#define SPO2_CHARACTERISTIC_UUID_CTR    "00112A37-0000-1000-8000-00805F9B34FB"

/**
 * @brief 服务封装的基类，但是貌似现在一个基类就够用了，就是把服务和自己附到传进来的server里
 * 
 */
class SensorServiceBase
{
    BLEUUID m_ServiceUUID;
    BLEService* m_p_SensorService;
    SensorCharacteristicBase* m_p_SensorCharacteristic;
public:
    SensorServiceBase(BLEServer * pServer,BLEUUID m_ServiceUUID,SensorCharacteristicBase* m_p_SensorCharacteristic);
    SensorServiceBase(BLEServer * pServer,std::string m_ServiceUUID,SensorCharacteristicBase* m_p_SensorCharacteristic);

    BLEService* getService();
};

BLEService* SensorServiceBase::getService(){
    return m_p_SensorService;
}

// class ECG_SensorService: public SensorServiceBase
// {
// private:
    
// public:
//     ECG_SensorService(BLEServer * pServer,BLEUUID m_ServiceUUID);
//     ~ECG_SensorService();
//     ;

// };

// ECG_SensorService::ECG_SensorService(BLEServer * pServer,BLEUUID m_ServiceUUID){
//     ;
// }