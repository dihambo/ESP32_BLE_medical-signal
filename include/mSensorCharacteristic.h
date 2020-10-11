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
    uint32_t m_SampleRate = 30;
    
    virtual void startTicker();
    virtual void stopTicker();

protected:    
    Ticker* m_p_s_Ticker = new Ticker;
    SensorDataCollectorBase* m_SensorData;
    uint32_t m_SensorProperties = BLECharacteristic::PROPERTY_NOTIFY           // ! 这里其实让特性的属性可以自调的，但是目前来说只需要完成发送数据和接受指令这些有限的功能，所以先暂时默认这些属性好了。之后有需求再改。
                                    |BLECharacteristic::PROPERTY_READ;
    uint32_t m_ControlProperties = BLECharacteristic::PROPERTY_READ
                                    |BLECharacteristic::PROPERTY_WRITE;
    BLEUUID m_SensorCharacteristicUUID;
    BLEUUID m_ControlCharacteristicUUID;
    BLECharacteristic* m_p_SensorCharateristic;
    BLECharacteristic* m_p_ControlCharateristic;
    
    std::string m_Data;

    virtual ~SensorCharacteristicBase(){
        if(m_p_s_Ticker!=NULL){
            delete m_p_s_Ticker;
            m_p_s_Ticker=NULL;
        }
    };
};

/**
 * @brief BLE的特性回调类的过渡封装，传入传感器特性基类，以便之后复用
 * 
 * ! 这个封装只是暂时的，之后要进行多态处理，让他能够适应不同回调的情景。
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
 * @brief Construct a new Sensor Characteristic Callback Base:: Sensor Characteristic Callback Base object
 * 
 * @param p_SensorChar 
 */
SensorCharacteristicCallbackBase::SensorCharacteristicCallbackBase(SensorCharacteristicBase* p_SensorChar)
{
   m_p_SensorChar = p_SensorChar;
}

/**
 * @brief Destroy the Sensor Characteristic Callback Base:: Sensor Characteristic Callback Base object
 * 
 */
SensorCharacteristicCallbackBase::~SensorCharacteristicCallbackBase()
{
}
/**
 * @brief 特性回调里的写函数的回调
 * 
 * @param p_Char 需要附的特性
 */
void SensorCharacteristicCallbackBase::onWrite(BLECharacteristic * p_Char){
    std::string HRate_rxValue = p_Char->getValue();
    if (HRate_rxValue.length() > 0){
        if(HRate_rxValue=="start"){
            m_p_SensorChar->startTicker();
        }
        else if(HRate_rxValue=="stop"){
            m_p_SensorChar->stopTicker();
        }
        Serial.println("");
    }    
}


/**
 * @brief 心电信号的特性封装，主要就是把发送数据的特性和负责控制的特性在这个类里被打包好。
 * 
 * ! 但是这里面的callback类好像得用new开开辟一个新的来用。因为之后可能会遇到onRead的callback，这时候复用性就不好了。
 * 
 * ? 好像可以用多态来派生。反正都写好Base这个类名了，之后有需要或者有时间再把它分出去再改。
 *  
 */
class ECG_SensorCharacteristic:protected SensorCharacteristicBase
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


/**
 * @brief 传感器特性封装构造函数
 * 
 * @param sensorUUID 负责发送数据的Char的UUID
 * @param controlUUID 负责进行控制的Char的UUID
 * @param ECG_PinOUT ECG模块的数据引脚
 */
ECG_SensorCharacteristic::ECG_SensorCharacteristic(std::string sensorUUID, std::string controlUUID,int ECG_PinOUT){
    ECGCharCurl_this = this;     //初始化静态指针，以供静态callback调用类内成员。
    m_SensorData = new ECG_SensorDataCollector(ECG_PinOUT);
    
    m_SensorCharacteristicUUID = BLEUUID(sensorUUID);
    m_ControlCharacteristicUUID = BLEUUID(controlUUID);
    m_p_SensorCharateristic = &BLECharacteristic(m_SensorCharacteristicUUID,m_SensorProperties);
    m_p_ControlCharateristic = &BLECharacteristic(m_ControlCharacteristicUUID,m_ControlProperties);
}

/**
 * @brief 开启计时器，按照给定的周期进行采样。
 * 
 */
void ECG_SensorCharacteristic::startTicker(){
    m_p_s_Ticker->attach_ms(m_SampleRate,m_TickerCallback);
}

/**
 * @brief 停止采样。
 * 
 */
void ECG_SensorCharacteristic::stopTicker(){
    m_p_s_Ticker->detach();
}

/**
 * @brief 计时器每周期所执行的函数，这里就是将采取到的数据进行发送
 * 
 * ! 这个函数是要到处重复的，可能需要之后重构，但是现在主要完成功能，暂时搁置一下
 */
void ECG_SensorCharacteristic::m_TickerCallback(){      
    ECGCharCurl_this->m_Data = ECGCharCurl_this->m_SensorData->getData().c_str();       // 获取数据并刷新                               
    ECGCharCurl_this->m_p_SensorCharateristic->setValue(ECGCharCurl_this->m_Data);      // 设置数据
    ECGCharCurl_this->m_p_SensorCharateristic->notify();                                // 发送数据
}

void ECG_SensorCharacteristic::SetControlCharCallback(SensorCharacteristicCallbackBase* SensorCharCallback){
    m_p_ControlCharateristic->setCallbacks(SensorCharCallback);
}