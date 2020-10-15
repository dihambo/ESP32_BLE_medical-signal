#pragma once

#include "mSensorService.h"

#include <BLEdevice.h>
#include <BLEServer.h>

class BLEServerCallbacksBase : public BLEServerCallbacks
{
public:
    bool deviceConnected;

    BLEServerCallbacksBase();
    ~BLEServerCallbacksBase();
    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);
};

BLEServerCallbacksBase::BLEServerCallbacksBase(){
    deviceConnected = false;
}

void BLEServerCallbacksBase::onConnect(BLEServer *pServer)
{
    deviceConnected = true;
    String conId = String(pServer->getConnId());
    Serial.print(conId);Serial.println(" connected.");
};

void BLEServerCallbacksBase::onDisconnect(BLEServer *pServer){
    deviceConnected = false;
}

class BLEDeviceBase
{
public:
    BLEDeviceBase(std::string BLE_name);
    ~BLEDeviceBase();

    BLEServer *m_p_BLEServer;

    void AddService();
};

BLEDeviceBase::BLEDeviceBase(std::string BLE_name){
    BLEDevice::init(BLE_name);
    m_p_BLEServer = BLEDevice::createServer();
    m_p_BLEServer->setCallbacks(new BLEServerCallbacksBase());
}

void BLEDevice::AddService(BLEService* service){

}