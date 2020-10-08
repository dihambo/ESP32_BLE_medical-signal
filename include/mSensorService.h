#pragma once

#include "mSensorData.h"

#include <Arduino.h>
#include <Ticker.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE2904.h>

class SensorServiceBase
{
    Ticker m_Ticker;
    SensorDataCollectorBase* m_SensorData;

    BLEUUID m_ServiceUUID;
    BLEService* m_p_SensorService;

    SensorServiceBase(BLEServer * pServer);
public:

};

