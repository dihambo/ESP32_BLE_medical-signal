#pragma once

#include "mSensorData.h"

#include <Arduino.h>
#include <Ticker.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE2904.h>

class SensorCharacteristicBase{
    
    BLE2902* m_p_s_2902 = new BLE2902;
    // BLE2904* m_2904 = new BLE2904;

    virtual void startTicker()=0;
public:
    uint32_t m_SampleRate = 30;
    

protected:    
    Ticker* m_p_s_Ticker = new Ticker;
    SensorDataCollectorBase* m_SensorData;
    uint32_t m_SensorProperties = BLECharacteristic::PROPERTY_NOTIFY
                                    |BLECharacteristic::PROPERTY_READ;
    uint32_t m_ControlProperties = BLECharacteristic::PROPERTY_READ
                                    |BLECharacteristic::PROPERTY_WRITE;
    BLEUUID m_SensorCharacteristicUUID;
    BLEUUID m_ControlCharacteristicUUID;
    BLECharacteristic* m_p_SensorCharateristic;
    BLECharacteristic* m_p_ControlCharateristic;
    
    std::string m_Data;

    virtual void startTicker();
    virtual void stopTicker();

    virtual ~SensorCharacteristicBase(){
        if(m_p_s_Ticker!=NULL){
            delete m_p_s_Ticker;
            m_p_s_Ticker=NULL;
        }
    };
};

/**
 * @brief 心电信号的特性封装
 * 
 */
class ECG_SensorCharacteristic:protected SensorCharacteristicBase, public BLECharacteristicCallbacks
{
    static ECG_SensorCharacteristic* ECGCharCurl;
   
public:
    ECG_SensorCharacteristic(std::string sensorUUID, std::string controlUUID,int ECG_PinOUT){
        
        ECGCharCurl = this;
        m_SensorData = new ECG_SensorDataCollector(ECG_PinOUT);
        m_Data = m_SensorData->getData().c_str();

        m_SensorCharacteristicUUID = BLEUUID(sensorUUID);
        m_ControlCharacteristicUUID = BLEUUID(controlUUID);
        m_p_SensorCharateristic = &BLECharacteristic(m_SensorCharacteristicUUID,m_SensorProperties);
        m_p_ControlCharateristic = &BLECharacteristic(m_ControlCharacteristicUUID,m_ControlProperties);

        m_p_ControlCharateristic->setCallbacks(this);
    }

    void onWrite(BLECharacteristic *HRateCharacteristic)
    {
        std::string HRate_rxValue = HRateCharacteristic->getValue();
        if (HRate_rxValue.length() > 0){
            if(HRate_rxValue=="start"){
                startTicker();
            }
            else if(HRate_rxValue=="stop"){
                stopTicker();
            }
            Serial.println("");
        }
    }

    void startTicker(){
        m_p_s_Ticker->attach_ms(m_SampleRate,m_TickerCallback);
    }
    void stopTicker(){
        m_p_s_Ticker->detach();
    }


private:
    static void m_TickerCallback(){
        ECGCharCurl->m_p_SensorCharateristic->setValue(ECGCharCurl->m_Data);
        ECGCharCurl->m_p_SensorCharateristic->notify();
    }

    
};