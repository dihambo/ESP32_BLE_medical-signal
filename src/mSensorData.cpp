#include "mSensorData.h"



// ECG模块函数实现
/**
 * @brief Construct a new ecg sensordatacollector::ecg sensordatacollector object
 * 
 * @param PINOut 
 */
ECG_SensorDataCollector::ECG_SensorDataCollector(int PINOut){
    m_PINOut = PINOut;
    setup();
}

/**
 * @brief AD8243模块初始化
 * 
 */
void ECG_SensorDataCollector::setup(){
    pinMode(m_PINOut, INPUT);
}

/**
 * @brief AD8232模块数据采集方法
 * 
 * @return String   采集到的数据
 */
String ECG_SensorDataCollector::getData(){
    return String(analogRead(m_PINOut));
}

// TEMP模块函数实现

/**
 * @brief Construct a new temp sensordatacollector::temp sensordatacollector object
 * 
 * @param PINOut 温度模块所接引脚
 */
TEMP_SensorDataCollector::TEMP_SensorDataCollector(int PINOut){
    m_PINOut = PINOut;
    m_oneWire = OneWire(m_PINOut);
    m_temp = DallasTemperature(&m_oneWire);
    setup();
}

/**
 * @brief 温度模块初始化
 * 
 */
void TEMP_SensorDataCollector::setup(){
    m_temp.begin();
}

/**
 * @brief 温度模块获取数据
 * 
 * @return String   温度(摄氏度)
 */
String TEMP_SensorDataCollector::getData(){
    return String(m_temp.getTempCByIndex(0));
}