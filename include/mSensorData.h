#pragma once 

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <MAX30105.h>
#include <heartRate.h>

// ! TickerCallback函数是一个需要定制化的函数，因为这个计时器回调函数是各个传感器用来采样的函数，每个函数的方法必定不一样，

/**
* @brief 外围数据采集对象。采集外部信号，并提供一个数据接口供ESP32进行数据读取。
*/
class SensorDataCollectorBase
{
public:
    virtual String getData()=0;
    Ticker* m_p_s_Ticker = new Ticker;

private:
    virtual void setup()=0;
};

/**
 * @brief AD8236模块（ECG）数据采集类
 * @param PINout    AD8236接ESP32模块引脚 
 */
class ECG_SensorDataCollector:public SensorDataCollectorBase
{
    int m_PINOut;
public:
    ECG_SensorDataCollector(int PINOut);
    String getData();
private:
    void setup();
};

/**
 * @brief 温度模块数据采集器
 * 
 */
class TEMP_SensorDataCollector:public SensorDataCollectorBase
{
    int m_PINOut;
    OneWire m_oneWire;
    DallasTemperature m_temp;

public:
    TEMP_SensorDataCollector(int PINOut);
    String getData();

private:
    void setup();
};

/**
 * @brief MAX30102模块数据采集器
 * 
 */
class MAX30102_SensorDataCollector:public SensorDataCollectorBase
{
    MAX30105 m_max30105;

public:
    String getData();
private:
    void setup();
};