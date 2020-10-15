#include "mSensorService.h"


SensorServiceBase::SensorServiceBase(BLEServer * pServer,BLEUUID m_ServiceUUID,SensorCharacteristicBase* SensorCharacteristic){
    m_p_SensorService = pServer->createService(m_ServiceUUID);
    m_p_SensorCharacteristic = SensorCharacteristic;
    m_p_SensorService->addCharacteristic(m_p_SensorCharacteristic->m_p_SensorCharateristic);
    m_p_SensorService->addCharacteristic(m_p_SensorCharacteristic->m_p_ControlCharateristic);
}
SensorServiceBase::SensorServiceBase(BLEServer * pServer,std::string str_ServiceUUID,SensorCharacteristicBase* SensorCharacteristic){
    m_ServiceUUID = BLEUUID(str_ServiceUUID);
    m_p_SensorService = pServer->createService(m_ServiceUUID);
    m_p_SensorCharacteristic = SensorCharacteristic;
    m_p_SensorService->addCharacteristic(m_p_SensorCharacteristic->m_p_SensorCharateristic);
    m_p_SensorService->addCharacteristic(m_p_SensorCharacteristic->m_p_ControlCharateristic);
}