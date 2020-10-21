#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE2904.h>
#include <String.h>
#include<Ticker.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <MAX30105.h>
#include <heartRate.h>

//使能标志位
bool ECG_EN;
bool TEMP_EN;
bool MAX30102_EN;

OneWire oneWire(4);

//温度模块
DallasTemperature temp(&oneWire);

//心率模块
MAX30105 MAX30102;
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
// long irValue;
long delta;

float beatsPerMinute;
int beatAvg;

String textIR = "IR= ";
String textBPM = "BPM= ";
String textAVGBPM = " Avg_BPM= ";

//BLE
BLEServer *pServer;
BLECharacteristic *ecgCharacteristic; //创建一个BLE特性ecgCharacteristic
BLECharacteristic *tempCharacteristic; //创建一个BLE特性ecgCharacteristic
BLECharacteristic *MAX30102Characteristic; //创建一个BLE特性ecgCharacteristic

bool deviceConnected = false;       //连接否标志位
uint8_t txValue = 0;                //TX的值
long lastMsg = 0;                   //存放时间的变量
String rxload = "Blank Value";      //RX的预置值
//BLE UUID
#define ECG_SERVICE_UUID              "0001181C-0000-1000-8000-00805F9B34FB" // UART service UUID
#define ECG_CHARACTERISTIC_UUID_RX    "0002181C-0000-1000-8000-00805F9B34FB"
#define ECG_CHARACTERISTIC_UUID_TX    "0003181C-0000-1000-8000-00805F9B34FB"
#define TEMP_SERVICE_UUID             "00042A1F-0000-1000-8000-00805F9B34FB" // UART service UUID
#define TEMP_CHARACTERISTIC_UUID_RX   "00052A1F-0000-1000-8000-00805F9B34FB"
#define TEMP_CHARACTERISTIC_UUID_TX   "00062A1F-0000-1000-8000-00805F9B34FB"
#define MAX30102_SERVICE_UUID            "00072A37-0000-1000-8000-00805F9B34FB" // UART service UUID
#define MAX30102_CHARACTERISTIC_UUID_RX  "00082A37-0000-1000-8000-00805F9B34FB"
#define MAX30102_CHARACTERISTIC_UUID_TX  "00092A37-0000-1000-8000-00805F9B34FB"
//计时器
Ticker ECG_SendTicker;
Ticker TEMP_SendTicker;
Ticker MAX30102_SendTicker;
//发送变量
String ecgValue ;
String tempValue;
String MAX30102Value;
//采样间隔（计时器）
uint32_t ECG_Sample=50;//采样间隔 ms
uint32_t TEMP_Sample=1000;//采样间隔 ms
uint32_t MAX30102_Sample=30;

//服务器回调
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    String conId = String(pServer->getConnId());
    Serial.print(conId);Serial.println(" connected.");
  };
  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

//心电计时器回调
void ECG_TickerCallback(){
  if(ECG_EN){
    ecgValue = String(analogRead(36));
    ecgCharacteristic->setValue(ecgValue.c_str());
    ecgCharacteristic->notify();
    //Serial.println(ecgValue);
  }
}
//温度计时器回调
void TEMP_TickerCallback(){
  if(TEMP_EN){
    temp.requestTemperatures();
    tempValue = String(temp.getTempCByIndex(0));
    tempCharacteristic->setValue(tempValue.c_str());
    tempCharacteristic->notify();
    // Serial.println(tempValue);
  }
}
//心率计时器回调
void MAX30102_TickerCallback(){
  // if (MAX30102_EN){
    // unsigned long t1 =millis();
    long irValue = MAX30102.getIR();
    
      //We sensed a beat!
    if (checkForBeat(irValue))
    { 
      MAX30102Value=""; 
      
      delta = millis() - lastBeat;
      lastBeat = millis();
      beatsPerMinute = 60 / (delta / 1000.0);
      // Serial.print("beatsPerMinute: ");Serial.println(beatsPerMinute);
      if (beatsPerMinute < 255 && beatsPerMinute > 20)
      {
        rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE; //Wrap variable

        //Take average of readings
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
        MAX30102Value.concat(textIR);
        MAX30102Value.concat(irValue);
        MAX30102Value.concat(textAVGBPM);
        MAX30102Value.concat(beatAvg);
        
        // t1 = millis()-t1;Serial.println(t1);
        
      }
      else{
        MAX30102Value=""; 
        MAX30102Value.concat("beatsPerMinute is abnormal,please hold on");
      }
      MAX30102Characteristic->setValue(MAX30102Value.c_str());
      MAX30102Characteristic->notify();        
    }
    if (irValue<50000){
      MAX30102Value="-"; 
      MAX30102Value.concat("put your finger\t");
      MAX30102Value.concat(textIR);
      MAX30102Value.concat(irValue);
      MAX30102Characteristic->setValue(MAX30102Value.c_str());
      MAX30102Characteristic->notify();        
    } 

    //Serial.println(MAX30102Value);
  // }
}

//心电特性回调
class ecgCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *ecgCharacteristic)
  {
    std::string ECG_rxValue = ecgCharacteristic->getValue();
    if (ECG_rxValue.length() > 0){
      rxload = "";
      // for (int i = 0; i < ECG_rxValue.length(); i++)
      // {
      //   rxload += (char)ECG_rxValue[i];
      //   Serial.print(ECG_rxValue[i]);
      // }

      if(ECG_rxValue=="start"){
        ECG_EN = true;MAX30102_EN=false;
        ECG_SendTicker.attach_ms(ECG_Sample,ECG_TickerCallback);
        Serial.print("ECG Notify have started.");
      }
      else if(ECG_rxValue=="stop"){
        ECG_EN=false;
        ECG_SendTicker.detach();
        Serial.print("ECG Notify have stopped.");
      }
      else if(ECG_rxValue=="disconnect"){
        pServer->disconnect(0);
        Serial.print("0 have disconnected.");
      }

      Serial.println("");
    }
  }
};

//体温特性回调
class tempCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *tempCharacteristic)
  {
    std::string TEMP_rxValue = tempCharacteristic->getValue();
    if (TEMP_rxValue.length() > 0){
      rxload = "";
      if(TEMP_rxValue=="start"){
        TEMP_EN=true;
        TEMP_SendTicker.attach_ms(TEMP_Sample,TEMP_TickerCallback);
        Serial.print("TEMP Notify have started.");
      }
      else if(TEMP_rxValue=="stop"){
        TEMP_EN=false;
        TEMP_SendTicker.detach();
        Serial.print("TEMP Notify have stopped.");
      }
      else if(TEMP_rxValue=="disconnect"){
        pServer->disconnect(0);
        Serial.print("0 have disconnected.");
      }

      Serial.println("");
    }
  }
};

//心率特性回调
class MAX30102Callbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *MAX30102Characteristic)
  {
    std::string MAX30102_rxValue = MAX30102Characteristic->getValue();
    if (MAX30102_rxValue.length() > 0){
      rxload = "";

      if(MAX30102_rxValue=="start"){
        MAX30102_EN=true;ECG_EN=false;
        MAX30102_SendTicker.attach_ms(MAX30102_Sample,MAX30102_TickerCallback);
        Serial.print("MAX30102 Notify have started.");
      }
      else if(MAX30102_rxValue=="stop"){
        MAX30102_EN=false;
        MAX30102_SendTicker.detach();
        Serial.print("MAX30102 Notify have stopped.");
      }
      else if(MAX30102_rxValue=="disconnect"){
        pServer->disconnect(0);
        Serial.print("0 have disconnected.");
      }

      Serial.println("");
    }
  }
};

//启动BLE
void setupBLE(String BLEName)
{
  const char *ble_name = BLEName.c_str();
  BLEDevice::init(ble_name);
  BLEServer *pServer = BLEDevice::createServer(); 
  pServer->setCallbacks(new MyServerCallbacks()); 
  BLEService *ECGService = pServer->createService(ECG_SERVICE_UUID); 
  BLEService *TEMPService = pServer->createService(TEMP_SERVICE_UUID); 
  BLEService *MAX30102Service = pServer->createService(MAX30102_SERVICE_UUID); 

  //心电特征设置
  ecgCharacteristic = ECGService->createCharacteristic(ECG_CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY|BLECharacteristic::PROPERTY_READ); 
  ecgCharacteristic->addDescriptor(new BLE2902());
  ecgCharacteristic->addDescriptor(new BLE2904());
  BLECharacteristic *ecgCharacteristic = ECGService->createCharacteristic(ECG_CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  ecgCharacteristic->setCallbacks(new ecgCallbacks());

  //温度特征设置
  tempCharacteristic = TEMPService->createCharacteristic(TEMP_CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY|BLECharacteristic::PROPERTY_READ); 
  tempCharacteristic->addDescriptor(new BLE2902());
  tempCharacteristic->addDescriptor(new BLE2904());
  BLECharacteristic *tempCharacteristic = TEMPService->createCharacteristic(TEMP_CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  tempCharacteristic->setCallbacks(new tempCallbacks());

  //心率特征设置
  MAX30102Characteristic = MAX30102Service->createCharacteristic(MAX30102_CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY|BLECharacteristic::PROPERTY_READ); 
  MAX30102Characteristic->addDescriptor(new BLE2902());
  MAX30102Characteristic->addDescriptor(new BLE2904());
  BLECharacteristic *MAX30102Characteristic = MAX30102Service->createCharacteristic(MAX30102_CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  MAX30102Characteristic->setCallbacks(new MAX30102Callbacks());

  ECGService->start();                  //开启服务
  TEMPService->start();                  //开启服务
  MAX30102Service->start();                  //开启服务
  pServer->getAdvertising()->start(); //服务器开始广播
  Serial.println("Waiting a client connection to notify...");
}

//设置外围设备
void setupECG(){
  pinMode(36,INPUT);//设置心电模块输出
}
void setupTemp(){
  temp.begin();
}

void setupMAX30102(){
  MAX30102.begin(Wire, I2C_SPEED_FAST); //Use default I2C port, 400kHz speed
  MAX30102.setup(); //Configure sensor with default settings
  MAX30102.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  MAX30102.setPulseAmplitudeGreen(0); //Turn off Green LED
}
void setup()
{
  Serial.begin(115200);
  setupECG();
  setupTemp();
  setupMAX30102();
  setupBLE("GOUESP32BLE"); //设置蓝牙名称
}

void loop()
{

}
