#pragma once

#include "mSensorData.h"

#include <Arduino.h>
#include <Ticker.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE2904.h>
/**
 * @brief 传感器数据特性封装基类。
 * 
 * 目前来看这个基类里的东西似乎很多，也承担了过多的责任，没有很好的符合分工明确这一原则。但我目前的想法是先将一个功能实现在看添加类似新功能时会不会出现困难的情况。
 * 但是我现在能完成的就是尽量减少接口，让整个接口尽量紧凑一点。
 * 
 */
class SensorCharacteristicBase
{
    
    // BLE2902* m_p_s_2902 = new BLE2902;
    // BLE2904* m_2904 = new BLE2904;

public:
    SensorCharacteristicBase();
    ~SensorCharacteristicBase(){
        if(m_p_s_Ticker!=NULL){
            delete m_p_s_Ticker;
            m_p_s_Ticker=NULL;
        }
    };

    uint32_t m_SampleRate = 30;         // todo 这个借口也要改一下，用个函数进行设置。
    void startTicker();
    void stopTicker();

    BLECharacteristic* m_p_SensorCharateristic;
    BLECharacteristic* m_p_ControlCharateristic;

protected:    
    Ticker* m_p_s_Ticker = new Ticker;
    SensorDataCollectorBase* m_SensorData;
    uint32_t m_SensorProperties = BLECharacteristic::PROPERTY_NOTIFY           // todo 这里其实让特性的属性可以自调的，但是目前来说只需要完成发送数据和接受指令这些有限的功能，所以先暂时默认这些属性好了。之后有需求再改。
                                    |BLECharacteristic::PROPERTY_READ;
    uint32_t m_ControlProperties = BLECharacteristic::PROPERTY_READ
                                    |BLECharacteristic::PROPERTY_WRITE;
    BLEUUID m_SensorCharacteristicUUID;
    BLEUUID m_ControlCharacteristicUUID;
    
    std::string m_Data;

};

/**
 * @brief BLE的特性回调类的过渡封装，传入传感器特性基类，以便之后复用
 * 
 * todo 这个封装只是暂时的，之后要进行多态处理，让他能够适应不同回调的情景。
 */
class SensorCharacteristicCallbackBase:public BLECharacteristicCallbacks
{
private:
    /* data */
    SensorCharacteristicBase* m_p_SensorChar;
public:
    SensorCharacteristicCallbackBase(SensorCharacteristicBase* p_SensorChar);
    ~SensorCharacteristicCallbackBase();

    void onWrite(BLECharacteristic * p_Char);
};




/**
 * @brief 心电信号的特性封装，主要就是把发送数据的特性和负责控制的特性在这个类里被打包好。
 * 
 * ! 但是这里面的callback类好像得用new开开辟一个新的来用。因为之后可能会遇到onRead的callback，这时候复用性就不好了。
 * ? 好像可以用多态来派生。反正都写好Base这个类名了，之后有需要或者有时间再把它分出去再改。
 * 
 * todo 对了还有描述符忘了加，之后再说吧
 *  
 */
class ECG_SensorCharacteristic:public SensorCharacteristicBase
{
    static ECG_SensorCharacteristic* ECGCharCurl_this;
    SensorCharacteristicCallbackBase* m_p_SensorCharCallback;
   
public:
    ECG_SensorCharacteristic(std::string sensorUUID, std::string controlUUID,int ECG_PinOUT);
    void startTicker();
    void stopTicker();
    void SetControlCharCallback(SensorCharacteristicCallbackBase* SensorCharCallback);
private:
    static void m_TickerCallback();
};

