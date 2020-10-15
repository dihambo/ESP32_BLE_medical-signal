#include "mSensorCharacteristic.h"

SensorCharacteristicBase::SensorCharacteristicBase(){};
SensorCharacteristicBase::~SensorCharacteristicBase(){};
//----------------------------------------------------------------

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


//----------------------------------------------------------------


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
 * todo 这个函数是要到处重复的，可能需要之后重构，但是现在主要完成功能，暂时搁置一下
 */
void ECG_SensorCharacteristic::m_TickerCallback(){      
    ECGCharCurl_this->m_Data = ECGCharCurl_this->m_SensorData->getData().c_str();       // todo 这里其实应该多一道处理数据的程序的，因为目前来说数据格式不可能全部一致。                          
    ECGCharCurl_this->m_p_SensorCharateristic->setValue(ECGCharCurl_this->m_Data);      // 设置数据
    ECGCharCurl_this->m_p_SensorCharateristic->notify();                                // 发送数据
}

void ECG_SensorCharacteristic::SetControlCharCallback(SensorCharacteristicCallbackBase* SensorCharCallback){
    m_p_ControlCharateristic->setCallbacks(SensorCharCallback);
}