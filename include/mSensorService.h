#pragma once

#include "mSensorCharacteristic.h"
#include <Arduino.h>
#include <BLEServer.h>


/**
 * @brief 服务封装的基类，但是貌似现在一个基类就够用了，就是把服务和自己附到传进来的server里
 * @param pServer 要附的Server对象指针
 * @param UUID ServiceUUID
 * @param p_SensorCharacteristic 特性指针
 */
class SensorServiceBase
{
    BLEUUID m_ServiceUUID;
    BLEService* m_p_SensorService;
    SensorCharacteristicBase* m_p_SensorCharacteristic;
public:
    SensorServiceBase(BLEServer * pServer,BLEUUID m_ServiceUUID,SensorCharacteristicBase* p_SensorCharacteristic);
    SensorServiceBase(BLEServer * pServer,std::string m_ServiceUUID,SensorCharacteristicBase* p_SensorCharacteristic);

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